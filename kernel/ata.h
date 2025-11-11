// SUB OS - ATA/IDE Disk Driver Header
// Copyright (c) 2025 SUB OS Project

#ifndef ATA_H
#define ATA_H

#define ATA_PRIMARY_IO      0x1F0
#define ATA_PRIMARY_CONTROL 0x3F6
#define ATA_SECONDARY_IO    0x170
#define ATA_SECONDARY_CONTROL 0x376
#define ATA_REG_DATA       0
#define ATA_REG_ERROR      1
#define ATA_REG_FEATURES   1
#define ATA_REG_SECCOUNT   2
#define ATA_REG_LBA_LO     3
#define ATA_REG_LBA_MID    4
#define ATA_REG_LBA_HI     5
#define ATA_REG_DRIVE      6
#define ATA_REG_STATUS     7
#define ATA_REG_COMMAND    7
#define ATA_CMD_READ_PIO       0x20
#define ATA_CMD_READ_PIO_EXT   0x24
#define ATA_CMD_WRITE_PIO      0x30
#define ATA_CMD_WRITE_PIO_EXT  0x34
#define ATA_CMD_CACHE_FLUSH    0xE7
#define ATA_CMD_IDENTIFY       0xEC
#define ATA_SR_BSY   0x80
#define ATA_SR_DRDY  0x40
#define ATA_SR_DF    0x20
#define ATA_SR_DSC   0x10
#define ATA_SR_DRQ   0x08
#define ATA_SR_CORR  0x04
#define ATA_SR_IDX   0x02
#define ATA_SR_ERR   0x01
#define ATA_ER_BBK   0x80
#define ATA_ER_UNC   0x40
#define ATA_ER_MC    0x20
#define ATA_ER_IDNF  0x10
#define ATA_ER_MCR   0x08
#define ATA_ER_ABRT  0x04
#define ATA_ER_TK0NF 0x02
#define ATA_ER_AMNF  0x01
#define ATA_MASTER 0xE0
#define ATA_SLAVE  0xF0

typedef struct {
    unsigned short io_base;
    unsigned short control_base;
    unsigned char drive;
    unsigned int size;
    unsigned char model[41];
    unsigned char serial[21];
} ata_device_t;

void ata_init();
int ata_read_sectors(unsigned char drive, unsigned int lba, unsigned char count, void* buffer);
int ata_write_sectors(unsigned char drive, unsigned int lba, unsigned char count, const void* buffer);
int ata_identify(unsigned char drive, ata_device_t* device);
ata_device_t* ata_get_device(unsigned char drive);

#endif
