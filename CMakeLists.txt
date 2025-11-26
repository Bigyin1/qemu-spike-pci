cmake_minimum_required(VERSION 3.17)
project(qemu-pcidev LANGUAGES C CXX)

add_subdirectory(spike)
add_subdirectory(pcidev)


set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined")
