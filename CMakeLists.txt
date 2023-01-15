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
        utils/logging.h
        protocol/default_sizes.h
        protocol/parser.c
        protocol/parser.h
        protocol/codes.h
        mbroker/protocol_mbroker.c
        mbroker/protocol_mbroker.h
        protocol/protocol.h
        utils/fifo.h
        utils/fifo.c mbroker/request_handler.c
        mbroker/request_handler.h
        mbroker/box_list.h
        mbroker/box_list.c
        producer-consumer/producer-consumer.h
        producer-consumer/producer-consumer.c
        manager/protocol_manager.c
        manager/protocol_manager.h)

add_executable(publisher
        publisher/pub.c
        utils/betterassert.h
        utils/logging.c
        utils/logging.h
        protocol/default_sizes.h
        protocol/codes.h
        protocol/protocol.c
        protocol/protocol.h
        utils/fifo.h
        utils/fifo.c)
