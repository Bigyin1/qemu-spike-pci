#ifndef SPIKE_WRAPPER_H
#define SPIKE_WRAPPER_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Публичный интерфейс для C
typedef struct spike_sim spike_sim_t;

// Создание и уничтожение экземпляра Spike
spike_sim_t *spike_create(const char *isa, const char *priv, int num_harts);
void spike_del(spike_sim_t *instance);

// Загрузка ELF файла
bool spike_load_elf(spike_sim_t *instance, const char *elf_path);

// Запуск симуляции
void spike_run(spike_sim_t *instance);

#ifdef __cplusplus
}
#endif

#endif // SPIKE_WRAPPER_H
