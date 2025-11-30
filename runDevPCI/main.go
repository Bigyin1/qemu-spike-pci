package main

import (
	"encoding/binary"
	"fmt"
	"os"
	"syscall"
)

const (
	PCI_SPIKE_PHYS_ADDR = 0x00000000febf0000

	PCI_SPIKE_SIZE = 64 * 1024

	PCI_SPIKE_ELF_OFFSET = 12

	PCI_SPIKE_RES_OFFSET = 4
)

func main() {
	filePath := os.Args[1]

	fileData, err := os.ReadFile(filePath)
	if err != nil {
		fmt.Printf("ELF read error: %v\n", err)
		os.Exit(1)
	}

	f, err := os.OpenFile("/dev/mem", os.O_RDWR|os.O_SYNC, 0)
	if err != nil {
		fmt.Printf("Error /dev/mem. %v\n", err)
		os.Exit(1)
	}
	defer f.Close()

	mem, err := syscall.Mmap(
		int(f.Fd()),
		PCI_SPIKE_PHYS_ADDR,
		PCI_SPIKE_SIZE,
		syscall.PROT_READ|syscall.PROT_WRITE,
		syscall.MAP_SHARED,
	)

	if err != nil {
		fmt.Printf("Error mmap: %v\n", err)
		os.Exit(1)
	}
	defer syscall.Munmap(mem)

	copy(mem[PCI_SPIKE_ELF_OFFSET:], fileData)

	fmt.Printf("Writing file len: %d\n", uint32(len(fileData)))
	binary.LittleEndian.PutUint32(mem, uint32(len(fileData)))

	pres := binary.LittleEndian.Uint64(mem[PCI_SPIKE_RES_OFFSET:])
	fmt.Printf("Spike result: %d\n", pres)
}
