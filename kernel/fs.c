// SUB OS - Simple File System Implementation
// Copyright (c) 2025 SUB OS Project

#include "fs.h"
#include "ata.h"
#include "kernel.h"
#include "heap.h"

static fs_superblock_t superblock;
static fs_dirent_t root_dir[FS_MAX_FILES];
static fs_file_t file_handles[16];
static unsigned char fs_bitmap[FS_BLOCK_SIZE];
static unsigned char mounted = 0;

static int strcmp(const char* s1, const char* s2){while(*s1&&(*s1==*s2)){s1++;s2++;}return*(unsigned char*)s1-*(unsigned char*)s2;}
static void strcpy(char*dest,const char*src){while(*src){*dest++=*src++;}*dest=0;}

static int fs_read_block(unsigned int block,void*buffer){return ata_read_sectors(0,block,1,buffer);}
static int fs_write_block(unsigned int block,const void*buffer){return ata_write_sectors(0,block,1,buffer);}

static void fs_bitmap_set(unsigned int block){fs_bitmap[block/8]|=(1<<(block%8));}
static void fs_bitmap_clear(unsigned int block){fs_bitmap[block/8]&=~(1<<(block%8));}
static int fs_bitmap_test(unsigned int block){return fs_bitmap[block/8]&(1<<(block%8));}

static int fs_flush_metadata(){
    if(fs_write_block(0,&superblock)!=0){print_string("[FS] Error writing superblock\n");return-1;}
    if(fs_write_block(superblock.bitmap_block,&fs_bitmap[0])!=0){print_string("[FS] Error writing bitmap\n");return-1;}
    if(fs_write_block(superblock.root_dir_block,&root_dir[0])!=0){print_string("[FS] Error writing root dir\n");return-1;}
    if(fs_write_block(superblock.root_dir_block+1,&root_dir[64])!=0){print_string("[FS] Error writing root dir\n");return-1;}
    return 0;
}

static int fs_allocate_block(fs_dirent_t* entry){
    if(entry->blocks==0){
        for(unsigned int block=4;block<superblock.total_blocks;block++){
            if(!fs_bitmap_test(block)){
                fs_bitmap_set(block);
                entry->first_block=block;
                entry->blocks=1;
                superblock.free_blocks--;
                return 0;
            }
        }
        print_string("[FS] No free blocks\n");
        return -1;
    }
    unsigned int next_block=entry->first_block+entry->blocks;
    if(next_block>=superblock.total_blocks){print_string("[FS] Disk full\n");return -1;}
    if(fs_bitmap_test(next_block)){print_string("[FS] Fragmentation prevents file growth\n");return -1;}
    fs_bitmap_set(next_block);
    entry->blocks++;
    superblock.free_blocks--;
    return 0;
}

static void fs_free_blocks(fs_dirent_t* entry){
    if(entry->blocks==0){return;}
    for(unsigned int i=0;i<entry->blocks;i++){
        unsigned int block=entry->first_block+i;
        if(block<superblock.total_blocks && fs_bitmap_test(block)){
            fs_bitmap_clear(block);
            superblock.free_blocks++;
        }
    }
    entry->blocks=0;
    entry->first_block=0;
    entry->size=0;
}

int fs_init(){print_string("[OK] Initializing File System...\n");for(int i=0;i<16;i++){file_handles[i].in_use=0;}print_string("[OK] File System initialized\n");return 0;}
int fs_format(){print_string("[FS] Formatting disk with SFS...\n");superblock.magic=FS_MAGIC;superblock.block_size=FS_BLOCK_SIZE;superblock.total_blocks=1024;superblock.root_dir_block=2;superblock.bitmap_block=1;for(int i=0;i<FS_BLOCK_SIZE;i++){fs_bitmap[i]=0;}for(unsigned int block=0;block<4;block++){fs_bitmap_set(block);}superblock.free_blocks=superblock.total_blocks-4;if(fs_write_block(0,&superblock)!=0){print_string("[FS] Error writing superblock\n");return-1;}for(int i=0;i<FS_MAX_FILES;i++){root_dir[i].type=FS_TYPE_EMPTY;root_dir[i].name[0]=0;root_dir[i].size=0;root_dir[i].first_block=0;root_dir[i].blocks=0;}if(fs_write_block(superblock.bitmap_block,&fs_bitmap[0])!=0){print_string("[FS] Error writing bitmap\n");return-1;}if(fs_write_block(superblock.root_dir_block,&root_dir[0])!=0){print_string("[FS] Error writing root dir\n");return-1;}if(fs_write_block(superblock.root_dir_block+1,&root_dir[64])!=0){print_string("[FS] Error writing root dir\n");return-1;}print_string("[FS] Format complete\n");return 0;}
int fs_mount(){print_string("[FS] Mounting file system...\n");if(fs_read_block(0,&superblock)!=0){print_string("[FS] Error reading superblock\n");return-1;}if(superblock.magic!=FS_MAGIC){print_string("[FS] Invalid magic, formatting...\n");if(fs_format()!=0){return-1;}}if(fs_read_block(superblock.bitmap_block,&fs_bitmap[0])!=0){print_string("[FS] Error reading bitmap\n");return-1;}if(fs_read_block(superblock.root_dir_block,&root_dir[0])!=0){print_string("[FS] Error reading root dir\n");return-1;}if(fs_read_block(superblock.root_dir_block+1,&root_dir[64])!=0){print_string("[FS] Error reading root dir\n");return-1;}mounted=1;print_string("[FS] Mounted successfully\n");print_string("  Total blocks: ");print_dec(superblock.total_blocks);print_string("\n  Free blocks: ");print_dec(superblock.free_blocks);print_string("\n");return 0;}
static fs_dirent_t*fs_find_entry(const char*name){for(int i=0;i<FS_MAX_FILES;i++){if(root_dir[i].type!=FS_TYPE_EMPTY){if(strcmp(root_dir[i].name,name)==0){return&root_dir[i];}}}return 0;}
static fs_dirent_t*fs_find_free_entry(){for(int i=0;i<FS_MAX_FILES;i++){if(root_dir[i].type==FS_TYPE_EMPTY){return&root_dir[i];}}return 0;}
fs_file_t*fs_open(const char*path,const char*mode){if(!mounted)return 0;fs_file_t*file=0;for(int i=0;i<16;i++){if(!file_handles[i].in_use){file=&file_handles[i];break;}}if(!file){print_string("[FS] No free file handles\n");return 0;}fs_dirent_t*entry=fs_find_entry(path);if(mode[0]=='r'){if(!entry){print_string("[FS] File not found: ");print_string(path);print_string("\n");return 0;}file->mode='r';}else if(mode[0]=='w'){if(!entry){entry=fs_find_free_entry();if(!entry){print_string("[FS] No free directory entries\n");return 0;}strcpy(entry->name,path);entry->type=FS_TYPE_FILE;entry->size=0;entry->first_block=0;entry->blocks=0;}file->mode='w';}file->in_use=1;file->position=0;file->dirent=entry;file->buffer_block=0xFFFFFFFF;return file;}
int fs_close(fs_file_t*file){if(!file||!file->in_use)return-1;if(file->mode=='w'&&file->buffer_block!=0xFFFFFFFF){fs_write_block(file->buffer_block,file->buffer);}fs_flush_metadata();file->in_use=0;return 0;}
int fs_read(fs_file_t*file,void*buffer,unsigned int size){if(!file||!file->in_use||file->mode!='r')return-1;if(file->position+size>file->dirent->size){size=file->dirent->size-file->position;}unsigned char*buf=(unsigned char*)buffer;unsigned int read=0;while(read<size){unsigned int block=file->position/FS_BLOCK_SIZE;unsigned int offset=file->position%FS_BLOCK_SIZE;unsigned int to_read=FS_BLOCK_SIZE-offset;if(to_read>size-read){to_read=size-read;}unsigned int disk_block=file->dirent->first_block+block;if(file->buffer_block!=disk_block){fs_read_block(disk_block,file->buffer);file->buffer_block=disk_block;}for(unsigned int i=0;i<to_read;i++){buf[read++]=file->buffer[offset+i];}file->position+=to_read;}return read;}
int fs_write(fs_file_t*file,const void*buffer,unsigned int size){if(!file||!file->in_use||file->mode!='w')return-1;const unsigned char*buf=(const unsigned char*)buffer;unsigned int written=0;while(written<size){unsigned int block=file->position/FS_BLOCK_SIZE;unsigned int offset=file->position%FS_BLOCK_SIZE;unsigned int to_write=FS_BLOCK_SIZE-offset;if(to_write>size-written){to_write=size-written;}if(block>=file->dirent->blocks){if(fs_allocate_block(file->dirent)!=0){return -1;}}unsigned int disk_block=file->dirent->first_block+block;if(file->buffer_block!=disk_block){if(file->buffer_block!=0xFFFFFFFF){fs_write_block(file->buffer_block,file->buffer);}if(offset>0){fs_read_block(disk_block,file->buffer);}file->buffer_block=disk_block;}for(unsigned int i=0;i<to_write;i++){file->buffer[offset+i]=buf[written++];}file->position+=to_write;if(file->position>file->dirent->size){file->dirent->size=file->position;}}return written;}
int fs_list(const char*path){if(!mounted)return-1;print_string("\nDirectory listing:\n");print_string("------------------\n");int count=0;for(int i=0;i<FS_MAX_FILES;i++){if(root_dir[i].type==FS_TYPE_FILE){print_string(root_dir[i].name);print_string("  ");print_dec(root_dir[i].size);print_string(" bytes\n");count++;}}print_string("\nTotal files: ");print_dec(count);print_string("\n");return count;}
int fs_create(const char*path,unsigned char type){if(!mounted)return-1;if(fs_find_entry(path)){print_string("[FS] File already exists\n");return-1;}fs_dirent_t*entry=fs_find_free_entry();if(!entry){print_string("[FS] No free entries\n");return-1;}strcpy(entry->name,path);entry->type=type;entry->size=0;entry->first_block=0;entry->blocks=0;fs_write_block(superblock.root_dir_block,&root_dir[0]);fs_write_block(superblock.root_dir_block+1,&root_dir[64]);return 0;}
int fs_delete(const char*path){if(!mounted)return-1;fs_dirent_t*entry=fs_find_entry(path);if(!entry){print_string("[FS] File not found\n");return-1;}fs_free_blocks(entry);entry->type=FS_TYPE_EMPTY;entry->name[0]=0;fs_flush_metadata();return 0;}
int fs_seek(fs_file_t*file,unsigned int offset){if(!file||!file->in_use)return-1;if(offset>file->dirent->size){offset=file->dirent->size;}file->position=offset;return 0;}
