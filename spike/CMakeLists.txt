add_library(spikeb spike-bind.cpp)
target_link_libraries(spikeb PRIVATE fesvr riscv softfloat disasm)
target_include_directories(spikeb PUBLIC include)

install(TARGETS spikeb DESTINATION lib)

install(FILES include/spike-bind.h DESTINATION include)

add_executable(test main.c)
target_link_libraries(test PRIVATE spikeb)
