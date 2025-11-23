#include "fesvr/elfloader.h"
#include "fesvr/memif.h"
#include "riscv/processor.h"
#include "riscv/sim.h"

#include "spike-bind.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

static std::string elf_to_tmpfile(const char *elf_content, size_t sz) {

  std::string fname = "spike_elf";

  std::ofstream wf(fname, std::ios::out | std::ios::binary);

  wf.write(elf_content, sz);

  return fname;
}

static std::vector<std::pair<reg_t, abstract_mem_t *>>
make_mems(const std::vector<mem_cfg_t> &layout) {
  std::vector<std::pair<reg_t, abstract_mem_t *>> mems;
  mems.reserve(layout.size());
  for (const auto &cfg : layout) {
    mems.push_back(std::make_pair(cfg.get_base(), new mem_t(cfg.get_size())));
  }
  return mems;
}

extern "C" {

uint64_t spike_run(const char *elf_content, size_t sz) {

  cfg_t cfg;

  cfg.mem_layout.push_back(mem_cfg_t(0x40000000, 0x8000));

  std::vector<std::pair<reg_t, abstract_mem_t *>> mems =
      make_mems(cfg.mem_layout);
  std::vector<device_factory_sargs_t> plugin_device_factories;
  debug_module_config_t dm_config;

  std::string name = elf_to_tmpfile(elf_content, sz);
  std::vector<std::string> args = {name};

  sim_t sim(&cfg, false, mems, plugin_device_factories, args, dm_config,
            nullptr, true, nullptr, false, NULL);

  // sim.set_debug(true);
  sim.run();

  for (auto& mem : mems)
    delete mem.second;

  return sim.get_core(0)->get_state()->XPR[10];
}
}
