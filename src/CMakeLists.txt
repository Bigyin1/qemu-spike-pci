add_library(spikeb spike-bind.cpp)

target_link_libraries(spikeb PRIVATE fesvr riscv softfloat disasm)



