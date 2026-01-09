cmake_minimum_required(VERSION 3.12)
####################################################
#                XCOMPUTE-MESSAGES                 #
####################################################

project(xcmessages)
set(LOCAL /usr/local) #modify depending on dpfx install location

# define output paths
#
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/static)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/shared)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)


# geo-location
#
set(PROJECT_ROOT "${CMAKE_SOURCE_DIR}")


# debug helper
#
#set(CMAKE_VERBOSE_MAKEFILE ON)



# ---- 8>< ---- Probe for Protobuf Pathspec ------------------------------------------------------------ 8>< ---- #
# ---- 8>< --------------------------------------------------------------------------------------------- 8>< ---- #


execute_process(COMMAND bash -c "[ -f ${PROJECT_ROOT}/CMakeCache.txt ] && cat ${PROJECT_ROOT}/CMakeCache.txt | grep Protobuf_LIBRARY:FILEPATH= | cut -d'=' -f2"
                OUTPUT_VARIABLE pb_lib
                OUTPUT_STRIP_TRAILING_WHITESPACE
)
if ("${pb_lib} " STREQUAL " ")
    execute_process(COMMAND bash -c "[ -f ${PROJECT_ROOT}/CMakeCache.txt ] && cat ${PROJECT_ROOT}/CMakeCache.txt | grep Protobuf_LIBRARY:UNINITIALIZED= | cut -d'=' -f2"
                    OUTPUT_VARIABLE pb_lib
                    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if ("${pb_lib} " STREQUAL " ")
        execute_process(COMMAND bash -c "[ -f ${PROJECT_ROOT}/CMakeCache.txt ] && cat ${PROJECT_ROOT}/CMakeCache.txt | grep PROTOBUF_LIBRARY:FILEPATH= | cut -d'=' -f2"
                        OUTPUT_VARIABLE pb_lib
                        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if ("${pb_lib} " STREQUAL " ")
            execute_process(COMMAND bash -c "[ -f ${PROJECT_ROOT}/CMakeCache.txt ] && cat ${PROJECT_ROOT}/CMakeCache.txt | grep PROTOBUF_LIBRARY:UNINITIALIZED= | cut -d'=' -f2"
                            OUTPUT_VARIABLE pb_lib
                            OUTPUT_STRIP_TRAILING_WHITESPACE
            )
        endif()
    endif()
endif()


execute_process(COMMAND bash -c "[ -f ${PROJECT_ROOT}/CMakeCache.txt ] && cat ${PROJECT_ROOT}/CMakeCache.txt | grep Protobuf_INCLUDE_DIR:PATH= | cut -d'=' -f2"
                OUTPUT_VARIABLE pb_incl
                OUTPUT_STRIP_TRAILING_WHITESPACE
)
if ("${pb_incl} " STREQUAL " ")
    execute_process(COMMAND bash -c "[ -f ${PROJECT_ROOT}/CMakeCache.txt ] && cat ${PROJECT_ROOT}/CMakeCache.txt | grep Protobuf_INCLUDE_DIR:UNINITIALIZED= | cut -d'=' -f2"
                    OUTPUT_VARIABLE pb_incl
                    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if ("${pb_incl} " STREQUAL " ")
        execute_process(COMMAND bash -c "[ -f ${PROJECT_ROOT}/CMakeCache.txt ] && cat ${PROJECT_ROOT}/CMakeCache.txt | grep PROTOBUF_INCLUDE_DIR:FILE= | cut -d'=' -f2"
                        OUTPUT_VARIABLE pb_incl
                        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if ("${pb_incl} " STREQUAL " ")
            execute_process(COMMAND bash -c "[ -f ${PROJECT_ROOT}/CMakeCache.txt ] && cat ${PROJECT_ROOT}/CMakeCache.txt | grep PROTOBUF_INCLUDE_DIR:UNINITIALIZED= | cut -d'=' -f2"
                            OUTPUT_VARIABLE pb_incl
                            OUTPUT_STRIP_TRAILING_WHITESPACE
            )
        endif()
    endif()
endif()


if (NOT "${pb_lib} " STREQUAL " ")
    set(PROTOBUF_LIBRARY "${pb_lib}")
    set(Protobuf_LIBRARY "${pb_lib}")
endif()
if (NOT "${pb_incl} " STREQUAL " ")
    set(Protobuf_INCLUDE_DIR "${pb_incl}")
    set(PROTOBUF_INCLUDE_DIR "${pb_incl}")
endif()



#force static libraries as default (if possible)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" ".lib")
set(BUILD_SHARED_LIBS OFF)




# ---- 8>< ---- Package Dependencies ------------------------------------------------------------------- 8>< ---- #
# ---- 8>< --------------------------------------------------------------------------------------------- 8>< ---- #


find_package(Protobuf REQUIRED)



# ---- 8>< ---- Flags ---------------------------------------------------------------------------------- 8>< ---- #
# ---- 8>< --------------------------------------------------------------------------------------------- 8>< ---- #


# c++ standard
#
if (NOT "${CMAKE_CXX_STANDARD}")
    set(CMAKE_CXX_STANDARD 20)
endif()

if (NOT (DEFINED ENV{XC_CMAKE_VERBOSE_BUILD}))
    message("== Target arch :  ${CMAKE_SYSTEM_PROCESSOR}")
    message("== CXXFLAGS env:  ${CMAKE_CXX_FLAGS}")
elseif ($ENV{XC_CMAKE_VERBOSE_BUILD})
    message("== Target arch :  ${CMAKE_SYSTEM_PROCESSOR}")
    message("== CXXFLAGS env:  ${CMAKE_CXX_FLAGS}")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -ggdb -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3")

if (NOT DEFINED APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++${CMAKE_CXX_STANDARD} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-ignored-attributes -Wall -Wextra  -pedantic-errors -Werror -Wno-unknown-pragmas -Wno-pragmas -Wno-unknown-warning-option -Wno-unused-parameter -Wno-deprecated-declarations")

if (NOT (DEFINED ENV{XC_CMAKE_VERBOSE_BUILD}))
    message("== CXXFLAGS use:  ${CMAKE_CXX_FLAGS}")
elseif ($ENV{XC_CMAKE_VERBOSE_BUILD})
    message("== CXXFLAGS use:  ${CMAKE_CXX_FLAGS}")
endif()



# ---- 8>< ---- Include Pathspec ----------------------------------------------------------------------- 8>< ---- #
# ---- 8>< --------------------------------------------------------------------------------------------- 8>< ---- #


# ------------------------------------------------ #
#      project includes                            #
# ------------------------------------------------ #

include_directories(${CMAKE_CURRENT_BINARY_DIR})


# ------------------------------------------------ #
#      system includes                             #
# ------------------------------------------------ #

#include_directories(${LOCAL}/include)
#include_directories(${LOCAL}/include/xcompute)
include_directories(${PROTOBUF_INCLUDE_DIR})



# ---- 8>< ---- Library Pathspec ----------------------------------------------------------------------- 8>< ---- #
# ---- 8>< --------------------------------------------------------------------------------------------- 8>< ---- #


# ------------------------------------------------ #
#      protobuf library                            #
# ------------------------------------------------ #

if (DEFINED ENV{PROTOBUF_LIBDIR})                                                           # yay - use env variable path
    set(PB_STATIC_LIB $ENV{PROTOBUF_LIBDIR}/libprotobuf.a)

    if (DEFINED APPLE)
        set(PB_SHARED_LIB $ENV{PROTOBUF_LIBDIR}/libprotobuf.dylib)
    elseif(DEFINED WIN32)
        set(PB_SHARED_LIB $ENV{PROTOBUF_LIBDIR}/libprotobuf.dll.a)
    elseif(DEFINED UNIX)
        set(PB_SHARED_LIB $ENV{PROTOBUF_LIBDIR}/libprotobuf.so)
    endif()

    message("found env var PB_STATIC_LIB pathspec: ${PB_STATIC_LIB}")

else()
    set(PB_STATIC_LIB "${LOCAL}/lib/libprotobuf.a")                                       # back-up path

    if (DEFINED APPLE)
        set(PB_SHARED_LIB "${LOCAL}/lib/libprotobuf.dylib")
    elseif(DEFINED WIN32)
        set(PB_SHARED_LIB "${LOCAL}/lib/libprotobuf.dll.a")
    elseif(DEFINED UNIX)
        set(PB_SHARED_LIB "${LOCAL}/lib/libprotobuf.so")
    endif()


endif()

message("PROTOBUF_INCLUDE_DIR: ${PROTOBUF_INCLUDE_DIR}")
message("PB_STATIC_LIB: ${PB_STATIC_LIB}")
message("PB_SHARED_LIB: ${PB_SHARED_LIB}")

# ---- 8>< ---- Code Generation ------------------------------------------------------------------------ 8>< ---- #
# ---- 8>< --------------------------------------------------------------------------------------------- 8>< ---- #


if (NOT (DEFINED ENV{XC_CMAKE_VERBOSE_BUILD}))
    message("* * * * * * * * generating xcompute protocol buffers * * * * * * * * ")
elseif ($ENV{XC_CMAKE_VERBOSE_BUILD})
    message("* * * * * * * * generating xcompute protocol buffers * * * * * * * * ")
endif()


file(GLOB ProtoFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.proto")
protobuf_generate_cpp(ProtoSources ProtoHeaders ${ProtoFiles})



# ---- 8>< ---- Library Creation ----------------------------------------------------------------------- 8>< ---- #
# ---- 8>< --------------------------------------------------------------------------------------------- 8>< ---- #


message("target architecture: ${CMAKE_SYSTEM_PROCESSOR}")



# ~~ [[ Object libraries ]] ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #


# ------------------------------------------------ #
#      libxcmessages                               #
# ------------------------------------------------ #

add_library(message-objs OBJECT
            ${ProtoHeaders}
            ${ProtoSources}
)



# ~~ [[ Shared lib definitions & Properties ]] ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #


# shared-libs need -fPIC
#
set_target_properties(message-objs PROPERTIES
                      POSITION_INDEPENDENT_CODE 1
)


add_library(xcmessages-shared SHARED
            $<TARGET_OBJECTS:message-objs>
)
set_target_properties(xcmessages-shared PROPERTIES
                      OUTPUT_NAME "xcmessages"
)



# ~~ [[ Static lib definitions & Properties ]] ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #


add_library(xcmessages-static STATIC
            $<TARGET_OBJECTS:message-objs>
)
set_target_properties(xcmessages-static PROPERTIES
                      OUTPUT_NAME "xcmessages"
)



# ~~ [[ Link shared libs ]] ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #


target_link_libraries(xcmessages-shared PRIVATE ${PB_SHARED_LIB})



# ~~ [[ Link static libs ]] ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #


target_link_libraries(xcmessages-static PRIVATE ${PB_STATIC_LIB})



# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #



# ---- 8>< --------------------------------------------------------------------------------------------- 8>< ---- #
# ---- 8>< --------------------------------------------------------------------------------------------- 8>< ---- #
