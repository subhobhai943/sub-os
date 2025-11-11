// SUB OS - ATA/IDE Disk Driver Implementation
// Copyright (c) 2025 SUB OS Project

#include "ata.h"
#include "kernel.h"

static ata_device_t ata_devices[4];
static int ata_device_count = 0;

static void ata_wait_bsy(unsigned short io_base) {
    while (inb(io_base + ATA_REG_STATUS) & ATA_SR_BSY);
}
static void ata_wait_drq(unsigned short io_base) {
    while (!(inb(io_base + ATA_REG_STATUS) & ATA_SR_DRQ));
}
static void ata_delay_400ns(unsigned short io_base) {
    for (int i = 0; i < 4; i++) {
        inb(io_base + ATA_REG_STATUS);
    }
}
static void ata_soft_reset(unsigned short control_base) {
    outb(control_base, 0x04);
    ata_delay_400ns(control_base - 0x206);
    outb(control_base, 0x00);
}
int ata_identify(unsigned char drive, ata_device_t* device) {
    unsigned short io_base = (drive < 2) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;
    unsigned char drive_sel = (drive % 2 == 0) ? ATA_MASTER : ATA_SLAVE;
    outb(io_base + ATA_REG_DRIVE, drive_sel);
    ata_delay_400ns(io_base);
    outb(io_base + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
    ata_delay_400ns(io_base);
    unsigned char status = inb(io_base + ATA_REG_STATUS);
    if (status == 0) {
        return -1;
    }
    ata_wait_bsy(io_base);
    status = inb(io_base + ATA_REG_STATUS);
    if (status & ATA_SR_ERR) {
        return -1;
    }
    ata_wait_drq(io_base);
    unsigned short identify_data[256];
    for (int i = 0; i < 256; i++) {
        identify_data[i] = inw(io_base + ATA_REG_DATA);
    }
    device->io_base = io_base;
    device->control_base = (drive < 2) ? ATA_PRIMARY_CONTROL : ATA_SECONDARY_CONTROL;
    device->drive = drive;
    device->size = (identify_data[61] << 16) | identify_data[60];
    for (int i = 0; i < 20; i++) {
        device->model[i * 2] = identify_data[27 + i] >> 8;
        device->model[i * 2 + 1] = identify_data[27 + i] & 0xFF;
    }
    device->model[40] = 0;
    for (int i = 0; i < 10; i++) {
        device->serial[i * 2] = identify_data[10 + i] >> 8;
        device->serial[i * 2 + 1] = identify_data[10 + i] & 0xFF;
    }
    device->serial[20] = 0;
    return 0;
}
void ata_init() {
    print_string("[OK] Initializing ATA Driver...\n");
    ata_device_count = 0;
    for (unsigned char i = 0; i < 4; i++) {
        if (ata_identify(i, &ata_devices[ata_device_count]) == 0) {
            print_string("  Detected ATA drive ");
            print_dec(i);
            print_string(": ");
            print_string((const char*)ata_devices[ata_device_count].model);
            print_string(" (");
            print_dec(ata_devices[ata_device_count].size / 2048);
            print_string(" MB)\n");
            ata_device_count++;
        }
    }
    if (ata_device_count == 0) {
        print_string("  [WARN] No ATA drives detected\n");
    }
    print_string("[OK] ATA Driver initialized (");
    print_dec(ata_device_count);
    print_string(" drives)\n");
}
int ata_read_sectors(unsigned char drive, unsigned int lba, unsigned char count, void* buffer) {
    if (drive >= ata_device_count) return -1;
    ata_device_t* dev = &ata_devices[drive];
    unsigned short io_base = dev->io_base;
    unsigned char drive_sel = (dev->drive % 2 == 0) ? 0xE0 : 0xF0;
    ata_wait_bsy(io_base);
    outb(io_base + ATA_REG_DRIVE, drive_sel | ((lba >> 24) & 0x0F));
    ata_delay_400ns(io_base);
    outb(io_base + ATA_REG_SECCOUNT, count);
    outb(io_base + ATA_REG_LBA_LO, (unsigned char)(lba & 0xFF));
    outb(io_base + ATA_REG_LBA_MID, (unsigned char)((lba >> 8) & 0xFF));
    outb(io_base + ATA_REG_LBA_HI, (unsigned char)((lba >> 16) & 0xFF));
    outb(io_base + ATA_REG_COMMAND, ATA_CMD_READ_PIO);
    unsigned short* buf = (unsigned short*)buffer;
    for (int sector = 0; sector < count; sector++) {
        ata_wait_drq(io_base);
        unsigned char status = inb(io_base + ATA_REG_STATUS);
        if (status & ATA_SR_ERR) {
            print_string("[ATA] Read error\n");
            return -1;
        }
        for (int i = 0; i < 256; i++) {
            buf[sector * 256 + i] = inw(io_base + ATA_REG_DATA);
        }
    }
    return 0;
}
int ata_write_sectors(unsigned char drive, unsigned int lba, unsigned char count, const void* buffer) {
    if (drive >= ata_device_count) return -1;
    ata_device_t* dev = &ata_devices[drive];
    unsigned short io_base = dev->io_base;
    unsigned char drive_sel = (dev->drive % 2 == 0) ? 0xE0 : 0xF0;
    ata_wait_bsy(io_base);
    outb(io_base + ATA_REG_DRIVE, drive_sel | ((lba >> 24) & 0x0F));
    ata_delay_400ns(io_base);
    outb(io_base + ATA_REG_SECCOUNT, count);
    outb(io_base + ATA_REG_LBA_LO, (unsigned char)(lba & 0xFF));
    outb(io_base + ATA_REG_LBA_MID, (unsigned char)((lba >> 8) & 0xFF));
    outb(io_base + ATA_REG_LBA_HI, (unsigned char)((lba >> 16) & 0xFF));
    outb(io_base + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);
    const unsigned short* buf = (const unsigned short*)buffer;
    for (int sector = 0; sector < count; sector++) {
        ata_wait_drq(io_base);
        for (int i = 0; i < 256; i++) {
            outw(io_base + ATA_REG_DATA, buf[sector * 256 + i]);
        }
        outb(io_base + ATA_REG_COMMAND, ATA_CMD_CACHE_FLUSH);
        ata_wait_bsy(io_base);
    }
    return 0;
}
ata_device_t* ata_get_device(unsigned char drive) {
    if (drive >= ata_device_count) return 0;
    return &ata_devices[drive];
}
