#ifndef SPIKE_WRAPPER_H
#define SPIKE_WRAPPER_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

uint64_t spike_run(const char *elf_content, size_t sz);

#ifdef __cplusplus
}
#endif

#endif // SPIKE_WRAPPER_H
