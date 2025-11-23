add_library(spikeb spike-bind.cpp)
target_link_libraries(spikeb PRIVATE fesvr riscv softfloat disasm)


add_executable(test main.c)
target_link_libraries(test PRIVATE spikeb)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined")
