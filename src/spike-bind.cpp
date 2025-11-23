#include "fesvr/elfloader.h"
#include "fesvr/memif.h"
#include "riscv/processor.h"
#include "riscv/sim.h"

#include "spike-bind.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

// class simple_htif_t : public htif_t {
// public:
//   simple_htif_t() : htif_t(0, nullptr) {}

//   void wait() { stop(); }
// };

struct spike_sim {
  cfg_t *cfg;
  sim_t *sim;

  spike_sim() {

    cfg = new cfg_t();

    std::vector<std::pair<reg_t, abstract_mem_t *>> mems;
    std::vector<device_factory_sargs_t> plugin_device_factories;

    std::vector<std::string> htif_args;
  }
};

extern "C" {

uint64_t spike_run() {
  cfg_t cfg;

  std::vector<std::pair<reg_t, abstract_mem_t *>> mems;
  std::vector<device_factory_sargs_t> plugin_device_factories;
  std::vector<std::string> htif_args;
  std::vector<std::string> args;

  debug_module_config_t dm_config;

  sim_t sim(&cfg, false, mems, plugin_device_factories, args, dm_config, nullptr,
            false, nullptr, false, nullptr);

  sim.load_program();

  return instance;
}

void spike_destroy(spike_sim_t *instance) {
  if (instance) {
    delete instance;
  }
}
}
