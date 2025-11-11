// SUB OS - Simple File System Header
// Copyright (c) 2025 SUB OS Project

#ifndef FS_H
#define FS_H

#define FS_MAGIC 0x53465330  // "SFS0"
#define FS_BLOCK_SIZE 512
#define FS_MAX_FILES 128
#define FS_MAX_FILENAME 32
#define FS_MAX_FILESIZE (1024 * 1024)  // 1MB per file
#define FS_TYPE_EMPTY 0
#define FS_TYPE_FILE  1
#define FS_TYPE_DIR   2

typedef struct {
    unsigned int magic;
    unsigned int block_size;
    unsigned int total_blocks;
    unsigned int free_blocks;
    unsigned int root_dir_block;
    unsigned int bitmap_block;
    unsigned char reserved[492];
} __attribute__((packed)) fs_superblock_t;

typedef struct {
    char name[FS_MAX_FILENAME];
    unsigned char type;
    unsigned int size;
    unsigned int first_block;
    unsigned int blocks;
    unsigned char reserved[19];
} __attribute__((packed)) fs_dirent_t;

typedef struct {
    unsigned char in_use;
    unsigned char mode;
    unsigned int position;
    fs_dirent_t* dirent;
    unsigned char buffer[FS_BLOCK_SIZE];
    unsigned int buffer_block;
} fs_file_t;

int fs_init();
int fs_format();
int fs_mount();
fs_file_t* fs_open(const char* path, const char* mode);
int fs_close(fs_file_t* file);
int fs_read(fs_file_t* file, void* buffer, unsigned int size);
int fs_write(fs_file_t* file, const void* buffer, unsigned int size);
int fs_seek(fs_file_t* file, unsigned int offset);
int fs_create(const char* path, unsigned char type);
int fs_delete(const char* path);
int fs_list(const char* path);

#endif
