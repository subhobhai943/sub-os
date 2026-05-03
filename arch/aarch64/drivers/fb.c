/* SUB OS - VirtIO GPU Framebuffer Driver
 * Targets QEMU virt machine virtio-gpu-pci at MMIO base 0x0a000000
 *
 * VirtIO GPU minimal flow (display only, no cursor):
 *   1. Read device features, set DRIVER_OK
 *   2. CMD_GET_DISPLAY_INFO  -> get width/height
 *   3. CMD_RESOURCE_CREATE_2D (resource_id=1, RGBX8888, WxH)
 *   4. CMD_RESOURCE_ATTACH_BACKING (point to RAM pixel buffer)
 *   5. CMD_SET_SCANOUT (scanout 0, resource 1, WxH)
 *   6. To flush: CMD_TRANSFER_TO_HOST_2D + CMD_RESOURCE_FLUSH
 */

#include "../include/fb.h"
#include "../include/types.h"

/* VirtIO MMIO register offsets */
#define VIRTIO_MMIO_BASE        0x0a000000UL
#define VIRTIO_MMIO_MAGIC       0x000
#define VIRTIO_MMIO_VERSION     0x004
#define VIRTIO_MMIO_DEVICE_ID   0x008
#define VIRTIO_MMIO_VENDOR_ID   0x00C
#define VIRTIO_MMIO_DEV_FEAT    0x010
#define VIRTIO_MMIO_DRV_FEAT    0x020
#define VIRTIO_MMIO_QUEUE_SEL   0x030
#define VIRTIO_MMIO_QUEUE_MAX   0x034
#define VIRTIO_MMIO_QUEUE_NUM   0x038
#define VIRTIO_MMIO_QUEUE_READY 0x044
#define VIRTIO_MMIO_QUEUE_NOTIFY 0x050
#define VIRTIO_MMIO_INT_STATUS  0x060
#define VIRTIO_MMIO_INT_ACK     0x064
#define VIRTIO_MMIO_STATUS      0x070
#define VIRTIO_MMIO_QUEUE_DESC_LO  0x080
#define VIRTIO_MMIO_QUEUE_DESC_HI  0x084
#define VIRTIO_MMIO_QUEUE_AVAIL_LO 0x090
#define VIRTIO_MMIO_QUEUE_AVAIL_HI 0x094
#define VIRTIO_MMIO_QUEUE_USED_LO  0x0a0
#define VIRTIO_MMIO_QUEUE_USED_HI  0x0a4
#define VIRTIO_MMIO_CONFIG_GEN  0x0fc
#define VIRTIO_MMIO_CONFIG      0x100

/* VirtIO status bits */
#define VIRTIO_STATUS_ACK        1
#define VIRTIO_STATUS_DRIVER     2
#define VIRTIO_STATUS_FEATURES_OK 8
#define VIRTIO_STATUS_DRIVER_OK  4
#define VIRTIO_STATUS_FAILED     128

/* VirtIO GPU command types */
#define VIRTIO_GPU_CMD_GET_DISPLAY_INFO    0x0100
#define VIRTIO_GPU_CMD_RESOURCE_CREATE_2D  0x0101
#define VIRTIO_GPU_CMD_RESOURCE_UNREF      0x0102
#define VIRTIO_GPU_CMD_SET_SCANOUT         0x0103
#define VIRTIO_GPU_CMD_RESOURCE_FLUSH      0x0104
#define VIRTIO_GPU_CMD_TRANSFER_TO_HOST_2D 0x0105
#define VIRTIO_GPU_CMD_RESOURCE_ATTACH_BACKING 0x0106
#define VIRTIO_GPU_CMD_RESOURCE_DETACH_BACKING 0x0107
#define VIRTIO_GPU_RESP_OK_NODATA          0x1100
#define VIRTIO_GPU_RESP_OK_DISPLAY_INFO    0x1101

/* VirtIO GPU format */
#define VIRTIO_GPU_FORMAT_B8G8R8X8_UNORM   2
#define VIRTIO_GPU_FORMAT_B8G8R8A8_UNORM   1

/* Screen dimensions (fixed safe defaults for QEMU virt) */
#define FB_W  800
#define FB_H  600

/* Global framebuffer exported to rest of kernel */
u32 *fb_pixels = (u32*)0x48000000UL;  /* Place pixel buffer at 0x48000000 */
u32  fb_width  = FB_W;
u32  fb_height = FB_H;
u32  fb_pitch  = FB_W * 4;

static inline void mmio_w32(u64 base, u32 off, u32 val) {
    *(volatile u32*)(base + off) = val;
}
static inline u32 mmio_r32(u64 base, u32 off) {
    return *(volatile u32*)(base + off);
}

/* ---- VirtIO GPU command structs (packed) ---- */

typedef struct __attribute__((packed)) {
    u32 type;
    u32 flags;
    u64 fence_id;
    u32 ctx_id;
    u32 padding;
} virtio_gpu_ctrl_hdr;

typedef struct __attribute__((packed)) {
    u32 x, y, width, height;
} virtio_gpu_rect;

typedef struct __attribute__((packed)) {
    virtio_gpu_ctrl_hdr hdr;
    u32 resource_id;
    u32 format;
    u32 width;
    u32 height;
} virtio_gpu_resource_create_2d;

typedef struct __attribute__((packed)) {
    virtio_gpu_ctrl_hdr hdr;
    u32 resource_id;
    u32 nr_entries;
} virtio_gpu_resource_attach_backing;

typedef struct __attribute__((packed)) {
    u64 addr;
    u32 length;
    u32 padding;
} virtio_gpu_mem_entry;

typedef struct __attribute__((packed)) {
    virtio_gpu_ctrl_hdr hdr;
    virtio_gpu_rect     r;
    u64 offset;
    u32 resource_id;
    u32 padding;
} virtio_gpu_transfer_to_host_2d;

typedef struct __attribute__((packed)) {
    virtio_gpu_ctrl_hdr hdr;
    virtio_gpu_rect     r;
    u32 resource_id;
    u32 padding;
} virtio_gpu_resource_flush;

typedef struct __attribute__((packed)) {
    virtio_gpu_ctrl_hdr hdr;
    virtio_gpu_rect     r;
    u32 scanout_id;
    u32 resource_id;
} virtio_gpu_set_scanout;

/* ---- VirtIO split-ring queue (minimal, 1 descriptor at a time) ---- */
#define QUEUE_SIZE 16

typedef struct __attribute__((packed, aligned(16))) {
    u64 addr;
    u32 len;
    u16 flags;
    u16 next;
} virtq_desc;

typedef struct __attribute__((packed, aligned(2))) {
    u16 flags;
    u16 idx;
    u16 ring[QUEUE_SIZE];
} virtq_avail;

typedef struct __attribute__((packed)) {
    u32 id;
    u32 len;
} virtq_used_elem;

typedef struct __attribute__((packed, aligned(4))) {
    u16 flags;
    u16 idx;
    virtq_used_elem ring[QUEUE_SIZE];
} virtq_used;

/* Place queues at known physical addresses */
#define CONTROLQ_DESC_PA  0x47000000UL
#define CONTROLQ_AVAIL_PA (CONTROLQ_DESC_PA + QUEUE_SIZE*16)
#define CONTROLQ_USED_PA  (CONTROLQ_AVAIL_PA + sizeof(virtq_avail) + 2)

static virtq_desc  *desc  = (virtq_desc*)CONTROLQ_DESC_PA;
static virtq_avail *avail = (virtq_avail*)CONTROLQ_AVAIL_PA;
static virtq_used  *used  = (virtq_used*)CONTROLQ_USED_PA;

/* Command/response bounce buffers */
#define CMD_BUF_PA  0x47010000UL
#define RSP_BUF_PA  0x47011000UL
static u8 *cmd_buf = (u8*)CMD_BUF_PA;
static u8 *rsp_buf = (u8*)RSP_BUF_PA;

static u16 avail_idx = 0;
static u16 last_used = 0;

static void zero_mem(void *p, u32 n) {
    u8 *b = (u8*)p;
    while (n--) *b++ = 0;
}
static void copy_mem(void *dst, const void *src, u32 n) {
    u8 *d = (u8*)dst;
    const u8 *s = (const u8*)src;
    while (n--) *d++ = *s++;
}

/* Send command (cmd_buf) and receive response (rsp_buf)
 * Uses 2 descriptors: #0 = cmd (device-read), #1 = rsp (device-write) */
static void gpu_submit(u32 cmd_size, u32 rsp_size) {
    /* Descriptor 0: command */
    desc[0].addr  = CMD_BUF_PA;
    desc[0].len   = cmd_size;
    desc[0].flags = 1;  /* NEXT */
    desc[0].next  = 1;

    /* Descriptor 1: response */
    desc[1].addr  = RSP_BUF_PA;
    desc[1].len   = rsp_size;
    desc[1].flags = 2;  /* WRITE */
    desc[1].next  = 0;

    zero_mem(rsp_buf, rsp_size);

    /* Put desc 0 in avail ring */
    avail->ring[avail_idx % QUEUE_SIZE] = 0;
    __asm__ volatile("dsb sy" ::: "memory");
    avail->idx = ++avail_idx;
    __asm__ volatile("dsb sy" ::: "memory");

    /* Notify queue 0 */
    mmio_w32(VIRTIO_MMIO_BASE, VIRTIO_MMIO_QUEUE_NOTIFY, 0);

    /* Poll until used ring advances */
    while (used->idx == last_used)
        __asm__ volatile("dsb sy" ::: "memory");
    last_used = used->idx;
}

static void gpu_cmd_create_resource(void) {
    virtio_gpu_resource_create_2d cmd;
    zero_mem(&cmd, sizeof(cmd));
    cmd.hdr.type = VIRTIO_GPU_CMD_RESOURCE_CREATE_2D;
    cmd.resource_id = 1;
    cmd.format      = VIRTIO_GPU_FORMAT_B8G8R8X8_UNORM;
    cmd.width       = FB_W;
    cmd.height      = FB_H;
    copy_mem(cmd_buf, &cmd, sizeof(cmd));
    gpu_submit(sizeof(cmd), sizeof(virtio_gpu_ctrl_hdr));
}

static void gpu_cmd_attach_backing(void) {
    /* Header + 1 mem entry */
    virtio_gpu_resource_attach_backing hdr;
    virtio_gpu_mem_entry               entry;
    zero_mem(&hdr,   sizeof(hdr));
    zero_mem(&entry, sizeof(entry));
    hdr.hdr.type    = VIRTIO_GPU_CMD_RESOURCE_ATTACH_BACKING;
    hdr.resource_id = 1;
    hdr.nr_entries  = 1;
    entry.addr      = (u64)(u64)fb_pixels;
    entry.length    = FB_W * FB_H * 4;
    copy_mem(cmd_buf,                 &hdr,   sizeof(hdr));
    copy_mem(cmd_buf + sizeof(hdr),   &entry, sizeof(entry));
    gpu_submit(sizeof(hdr) + sizeof(entry), sizeof(virtio_gpu_ctrl_hdr));
}

static void gpu_cmd_set_scanout(void) {
    virtio_gpu_set_scanout cmd;
    zero_mem(&cmd, sizeof(cmd));
    cmd.hdr.type    = VIRTIO_GPU_CMD_SET_SCANOUT;
    cmd.r.x = 0; cmd.r.y = 0;
    cmd.r.width = FB_W; cmd.r.height = FB_H;
    cmd.scanout_id  = 0;
    cmd.resource_id = 1;
    copy_mem(cmd_buf, &cmd, sizeof(cmd));
    gpu_submit(sizeof(cmd), sizeof(virtio_gpu_ctrl_hdr));
}

int fb_init(void) {
    u64 base = VIRTIO_MMIO_BASE;

    /* Check magic */
    if (mmio_r32(base, VIRTIO_MMIO_MAGIC) != 0x74726976) return -1;
    if (mmio_r32(base, VIRTIO_MMIO_DEVICE_ID) != 16)     return -2;

    /* Reset + ACK + DRIVER */
    mmio_w32(base, VIRTIO_MMIO_STATUS, 0);
    mmio_w32(base, VIRTIO_MMIO_STATUS, VIRTIO_STATUS_ACK);
    mmio_w32(base, VIRTIO_MMIO_STATUS, VIRTIO_STATUS_ACK | VIRTIO_STATUS_DRIVER);

    /* Accept no special features */
    mmio_w32(base, VIRTIO_MMIO_DRV_FEAT, 0);
    mmio_w32(base, VIRTIO_MMIO_STATUS,
        VIRTIO_STATUS_ACK | VIRTIO_STATUS_DRIVER | VIRTIO_STATUS_FEATURES_OK);
    mmio_w32(base, VIRTIO_MMIO_STATUS,
        VIRTIO_STATUS_ACK | VIRTIO_STATUS_DRIVER | VIRTIO_STATUS_FEATURES_OK | VIRTIO_STATUS_DRIVER_OK);

    /* Set up control queue (queue 0) */
    zero_mem(desc,  QUEUE_SIZE * sizeof(virtq_desc));
    zero_mem(avail, sizeof(virtq_avail));
    zero_mem(used,  sizeof(virtq_used));

    mmio_w32(base, VIRTIO_MMIO_QUEUE_SEL, 0);
    mmio_w32(base, VIRTIO_MMIO_QUEUE_NUM, QUEUE_SIZE);
    mmio_w32(base, VIRTIO_MMIO_QUEUE_DESC_LO,  (u32)(CONTROLQ_DESC_PA & 0xFFFFFFFF));
    mmio_w32(base, VIRTIO_MMIO_QUEUE_DESC_HI,  (u32)(CONTROLQ_DESC_PA >> 32));
    mmio_w32(base, VIRTIO_MMIO_QUEUE_AVAIL_LO, (u32)(CONTROLQ_AVAIL_PA & 0xFFFFFFFF));
    mmio_w32(base, VIRTIO_MMIO_QUEUE_AVAIL_HI, (u32)(CONTROLQ_AVAIL_PA >> 32));
    mmio_w32(base, VIRTIO_MMIO_QUEUE_USED_LO,  (u32)(CONTROLQ_USED_PA & 0xFFFFFFFF));
    mmio_w32(base, VIRTIO_MMIO_QUEUE_USED_HI,  (u32)(CONTROLQ_USED_PA >> 32));
    mmio_w32(base, VIRTIO_MMIO_QUEUE_READY, 1);

    /* Zero pixel buffer */
    zero_mem(fb_pixels, FB_W * FB_H * 4);

    /* GPU commands */
    gpu_cmd_create_resource();
    gpu_cmd_attach_backing();
    gpu_cmd_set_scanout();

    return 0;
}

void fb_flush(void) {
    /* Transfer pixels from RAM to host */
    virtio_gpu_transfer_to_host_2d tcmd;
    virtio_gpu_resource_flush      fcmd;

    zero_mem(&tcmd, sizeof(tcmd));
    tcmd.hdr.type   = VIRTIO_GPU_CMD_TRANSFER_TO_HOST_2D;
    tcmd.r.x = 0; tcmd.r.y = 0;
    tcmd.r.width = FB_W; tcmd.r.height = FB_H;
    tcmd.offset     = 0;
    tcmd.resource_id = 1;
    copy_mem(cmd_buf, &tcmd, sizeof(tcmd));
    gpu_submit(sizeof(tcmd), sizeof(virtio_gpu_ctrl_hdr));

    zero_mem(&fcmd, sizeof(fcmd));
    fcmd.hdr.type   = VIRTIO_GPU_CMD_RESOURCE_FLUSH;
    fcmd.r.x = 0; fcmd.r.y = 0;
    fcmd.r.width = FB_W; fcmd.r.height = FB_H;
    fcmd.resource_id = 1;
    copy_mem(cmd_buf, &fcmd, sizeof(fcmd));
    gpu_submit(sizeof(fcmd), sizeof(virtio_gpu_ctrl_hdr));
}

void fb_fill(u32 color) {
    u32 i, total = FB_W * FB_H;
    for (i = 0; i < total; i++) fb_pixels[i] = color;
}
