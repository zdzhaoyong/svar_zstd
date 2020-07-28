cmake_minimum_required(VERSION 2.8)
project(svar_zstd)

include(PICMake)
set(CMAKE_CXX_STANDARD 11)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

INCLUDE_DIRECTORIES(src src/common src/compress src/decompress)

pi_add_target(svar_zstd SHARED src REQUIRED System LTO)
pi_add_target(zstd BIN src REQUIRED System)

pi_report_target()

# Now do make install
pi_install(TARGETS svar_zstd)
