#include "qemu/osdep.h"
#include "qemu/log.h"
#include "qemu/units.h"
#include "hw/pci/pci.h"
#include "hw/hw.h"
#include "hw/pci/msi.h"
#include "qemu/timer.h"
#include "qom/object.h"
#include "qemu/main-loop.h" /* iothread mutex */
#include "qemu/module.h"
#include "qapi/visitor.h"
#include "spike-bind.h"



#define TYPE_PCI_SPIKE_DEVICE "spike"
#define SPIKE_DEVICE_ID         0x1337
typedef struct spikeState spikeState;
DECLARE_INSTANCE_CHECKER(spikeState, Spike,
                         TYPE_PCI_SPIKE_DEVICE)

struct spikeState {
    PCIDevice pdev;
    MemoryRegion mem;
	unsigned char data[0x10000];
};

static void pci_spike_register_types(void);
static void spike_instance_init(Object *obj);
static void spike_class_init(ObjectClass *class, void *data);
static void pci_spike_realize(PCIDevice *pdev, Error **errp);
static void pci_spike_uninit(PCIDevice *pdev);

type_init(pci_spike_register_types)

static void pci_spike_register_types(void) {
    static InterfaceInfo interfaces[] = {
        { INTERFACE_CONVENTIONAL_PCI_DEVICE },
        { },
    };
    static const TypeInfo spike_info = {
        .name          = TYPE_PCI_SPIKE_DEVICE,
        .parent        = TYPE_PCI_DEVICE,
        .instance_size = sizeof(spikeState),
        .instance_init = spike_instance_init,
        .class_init    = spike_class_init,
        .interfaces = interfaces,
    };

    type_register_static(&spike_info);
}

static void spike_instance_init(Object *obj) {
    printf("spike instance init\n");
}

static void spike_class_init(ObjectClass *class, void *data) {
    PCIDeviceClass *k = PCI_DEVICE_CLASS(class);

    k->realize = pci_spike_realize;
    k->exit = pci_spike_uninit;
    k->vendor_id = PCI_VENDOR_ID_QEMU;
    k->device_id = SPIKE_DEVICE_ID;
    k->class_id = PCI_CLASS_OTHERS;
}

static uint64_t read_uint(unsigned char *buffer, unsigned size) {
    switch (size)
    {
    case 1:
        return *(uint8_t*)buffer;
    case 2:
        return *(uint16_t*)buffer;
    case 4:
        return *(uint32_t*)buffer;   
    case 8:
        return *(uint64_t*)buffer; 
    default:
        return 0;
    }
}

static uint64_t spike_mem_read(void *opaque, hwaddr addr, unsigned size) {
	spikeState *spike = opaque;

    printf("read at %ld  size: %d\n", addr, size);
    return read_uint(spike->data + addr, size);
}

static void write_uint(unsigned char *buffer, uint64_t val, unsigned size) {
    buffer[0] = (unsigned char)(val >> 0);
    if (size == 1)
        return;
    buffer[1] = (unsigned char)(val >> 8);
    if (size == 2)
        return;
    buffer[2] = (unsigned char)(val >> 16);
    buffer[3] = (unsigned char)(val >> 24);
    if (size == 4)
        return;
    buffer[4] = (unsigned char)(val >> 32);
    buffer[5] = (unsigned char)(val >> 40);
    buffer[6] = (unsigned char)(val >> 48);
    buffer[7] = (unsigned char)(val >> 56);
}

static void spike_mem_write(void *opaque, hwaddr addr, uint64_t val, unsigned size) {
	spikeState *spike = opaque;

    printf("write at %ld val: %ld size: %d\n", addr, val, size);

    if (addr == 0 && size == 4) {
        unsigned fsize = (unsigned)val;
        printf("got fsize %u\n", fsize);
        write_uint(spike->data + 4, spike_run(spike->data+12, fsize), 8);
        return;
    }
    write_uint(spike->data + addr, val, size);

}

static const MemoryRegionOps spike_mem_ops = {
    .read = spike_mem_read,
    .write = spike_mem_write,
};

static void pci_spike_realize(PCIDevice *pdev, Error **errp) {
    printf("spike Realize\n");

    spikeState *spike = Spike(pdev);
    memory_region_init_io(&spike->mem, OBJECT(spike), &spike_mem_ops, spike, "spike-mem", 64 * KiB);
    pci_register_bar(pdev, 0, PCI_BASE_ADDRESS_SPACE_MEMORY, &spike->mem);
}

static void pci_spike_uninit(PCIDevice *pdev) {
    printf("spike un-init\n");
}
