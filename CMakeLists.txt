cmake_minimum_required(VERSION 3.22)
project(projeto_so_2 C)

set(CMAKE_C_STANDARD 11)

include_directories(fs)
include_directories(utils)

add_executable(mbroker
        fs/config.h
        fs/operations.c
        fs/operations.h
        fs/state.c
        fs/state.h
        mbroker/mbroker.c
        utils/betterassert.h
        utils/logging.c
        utils/logging.h)
