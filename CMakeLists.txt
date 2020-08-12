message("* * * * * * * *  generating xcompute protocol buffers * * * * * * * * ")
cmake_minimum_required(VERSION 3.16)
project(xcmessages)
set(PROJECT_ROOT "${CMAKE_SOURCE_DIR}")

execute_process(
    COMMAND bash -c "[ -f ${PROJECT_ROOT}/CMakeCache.txt ] && cat ${PROJECT_ROOT}/CMakeCache.txt | grep Protobuf_LIBRARY:FILEPATH= | cut -d'=' -f2"
    OUTPUT_VARIABLE pb_lib
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if("${pb_lib} " STREQUAL " ")
    execute_process(
        COMMAND bash -c "[ -f ${PROJECT_ROOT}/CMakeCache.txt ] && cat ${PROJECT_ROOT}/CMakeCache.txt | grep Protobuf_LIBRARY:UNINITIALIZED= | cut -d'=' -f2"
        OUTPUT_VARIABLE pb_lib
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if("${pb_lib} " STREQUAL " ")
        execute_process(
            COMMAND bash -c "[ -f ${PROJECT_ROOT}/CMakeCache.txt ] && cat ${PROJECT_ROOT}/CMakeCache.txt | grep PROTOBUF_LIBRARY:FILEPATH= | cut -d'=' -f2"
            OUTPUT_VARIABLE pb_lib
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if("${pb_lib} " STREQUAL " ")
            execute_process(
                COMMAND bash -c "[ -f ${PROJECT_ROOT}/CMakeCache.txt ] && cat ${PROJECT_ROOT}/CMakeCache.txt | grep PROTOBUF_LIBRARY:UNINITIALIZED= | cut -d'=' -f2"
                OUTPUT_VARIABLE pb_lib
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
        endif()
    endif()
endif()

execute_process(
    COMMAND bash -c "[ -f ${PROJECT_ROOT}/CMakeCache.txt ] && cat ${PROJECT_ROOT}/CMakeCache.txt | grep Protobuf_INCLUDE_DIR:PATH= | cut -d'=' -f2"
    OUTPUT_VARIABLE pb_incl
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if("${pb_incl} " STREQUAL " ")
    execute_process(
        COMMAND bash -c "[ -f ${PROJECT_ROOT}/CMakeCache.txt ] && cat ${PROJECT_ROOT}/CMakeCache.txt | grep Protobuf_INCLUDE_DIR:UNINITIALIZED= | cut -d'=' -f2"
        OUTPUT_VARIABLE pb_incl
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if("${pb_incl} " STREQUAL " ")
        execute_process(
            COMMAND bash -c "[ -f ${PROJECT_ROOT}/CMakeCache.txt ] && cat ${PROJECT_ROOT}/CMakeCache.txt | grep PROTOBUF_INCLUDE_DIR:FILE= | cut -d'=' -f2"
            OUTPUT_VARIABLE pb_incl
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if("${pb_incl} " STREQUAL " ")
            execute_process(
                COMMAND bash -c "[ -f ${PROJECT_ROOT}/CMakeCache.txt ] && cat ${PROJECT_ROOT}/CMakeCache.txt | grep PROTOBUF_INCLUDE_DIR:UNINITIALIZED= | cut -d'=' -f2"
                OUTPUT_VARIABLE pb_incl
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
        endif()
    endif()
endif()

if(NOT "${pb_lib} " STREQUAL " ")
    set(PROTOBUF_LIBRARY "${pb_lib}")
    set(Protobuf_LIBRARY "${pb_lib}")
endif()
if(NOT "${pb_incl} " STREQUAL " ")
    set(Protobuf_INCLUDE_DIR "${pb_incl}")
    set(PROTOBUF_INCLUDE_DIR "${pb_incl}")
endif()



find_package(Protobuf REQUIRED)

if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpmath=sse -msse2 -march=native -fopenmp -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-ignored-attributes -Wall -Wextra -Werror -Wno-unknown-pragmas -Wno-pragmas -Wno-unknown-warning-option -Wno-unused-parameter -Wno-deprecated-declarations")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS} -g -Os")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3")

message("include path: ${PROTOBUF_INCLUDE_DIR}")
include_directories(${PROTOBUF_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# protocol buffer static libraries
if(DEFINED ENV{PROTOBUF_LIBDIR})#yay - use env variable path
	set(PB_STATIC_LIB_PATH $ENV{PROTOBUF_LIBDIR}/libprotobuf.a)
	message("found PB_STATIC_LIB_PATH path: ")
else()
	set(PB_STATIC_LIB_PATH "/usr/local/lib/libprotobuf.a")#static backup path
	message("PB_STATIC_LIB_PATH not set - trying default path:")
endif()
message(${PB_STATIC_LIB_PATH})


file(GLOB ProtoFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.proto")
protobuf_generate_cpp(ProtoSources ProtoHeaders ${ProtoFiles})

add_library(xcmessages STATIC ${ProtoSources} ${ProtoHeaders})
target_link_libraries(xcmessages ${PB_STATIC_LIB_PATH})


#set(CMAKE_VERBOSE_MAKEFILE ON)
