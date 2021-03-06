cmake_minimum_required(VERSION 2.8.12)

if (NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
  if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
  endif()
endif()

project(ttuilite)

set(PROJECT_NAME ttuilite)
set(SPL_VERSION_MAJOR 0)
set(SPL_VERSION_MINOR 1)
set(SPL_VERSION_UPDATE 0)
set(SPL_VERSION "${SPL_VERSION_MAJOR}.${SPL_VERSION_MINOR}.${SPL_VERSION_UPDATE}")


if(CMAKE_SYSTEM_NAME STREQUAL "Darwin") 
    set(EXECUTABLE_NAME TTuiLite)
    if(NOT CMAKE_OSX_ARCHITECTURES) 
        set(CMAKE_OSX_ARCHITECTURES "x86_64")
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(EXECUTABLE_NAME ttuilite)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message("windows not supported at this time")
endif()

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -ffast-math -funroll-loops -fomit-frame-pointer")


if(ZERORAC)
    set(USE_ZERORAC 1)
else(ZERORAC)
    set(USE_ZERORAC 0)
endif(ZERORAC)



if (SALT)
    set(BELA on)
endif (SALT)


if (PEPPER)
    set(BELA on)
endif (PEPPER)


if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "^armv6l")
    set(ARM6 on)
endif() 

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Processor: ${CMAKE_SYSTEM_PROCESSOR}")
if (NOT DEFINED CPU_OPTIMIZATION_FLAGS)
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin") 
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "^arm")
            if (BELA)
                message(STATUS "BELA optimized")
                # /usr/xenomai/bin/xeno-config --skin=cobalt --no-mode-check --cflags
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/xenomai/include/cobalt -I/usr/xenomai/include -march=armv7-a -D_GNU_SOURCE -D_REENTRANT -D__COBALT__")
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,@${PROJECT_SOURCE_DIR}/xenomai.wrappers")
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsingle-precision-constant -march=armv7-a -mtune=cortex-a8 -mfloat-abi=hard -mfpu=neon -ftree-vectorize --fast-math")
                if (SALT)
                    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSALT")
                endif (SALT)
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBELA")
                #  /usr/xenomai/bin/xeno-config --skin=cobalt --no-auto-init --no-mode-check --ldflags
                set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-as-needed -L/usr/xenomai/lib -lcobalt -lmodechk -lpthread -lrt ")
                set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--no-as-needed -L/usr/xenomai/lib -lcobalt -lmodechk -lpthread -lrt ")
                set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-as-needed -L/usr/xenomai/lib -lcobalt -lmodechk -lpthread -lrt ")
                #set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -Wl,--no-as-needed -L/usr/xenomai/lib -lcobalt -lmodechk -lpthread -lrt ")
            elseif (ARM6)
                message(STATUS "arm6 build")
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv6+fp -mfpu=vfp -mfloat-abi=hard")
            else ()
                message(STATUS "arm7 build")
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv7-a -mfpu=neon -mfloat-abi=hard")
            endif ()
        else ()
            #        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Wall")
        endif ()
    endif ()
else ()
    message(STATUS "Using provided optimization flags '${CPU_OPTIMIZATION_FLAGS}'")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CPU_OPTIMIZATION_FLAGS}")
endif ()

if (NOT DISABLE_RPATH_OVERRIDE)
  ## setup rpath for linux and macos
  set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

  if(CMAKE_SYSTEM_NAME STREQUAL "Darwin") 
    set(CMAKE_INSTALL_NAME_DIR "@rpath")
    set(CMAKE_INSTALL_RPATH "@executable_path/../lib;@loader_path")
    set(CMAKE_BUILD_WITH_INSTALL_NAME_DIR TRUE)
  else()
    set(CMAKE_INSTALL_RPATH "$ORIGIN/:$ORIGIN/../lib")
  endif()
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/release/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/release/bin)

############
add_subdirectory(external/oscpack oscpack)
add_subdirectory(external/moodycamel moodycamel)
add_subdirectory(external/cJSON cjson)

if(UNIX AND NOT APPLE)
    add_subdirectory(external/ArduiPi_OLED arduiPi_OLED)
endif()
	
add_subdirectory(ttuilite)
add_subdirectory(sidekick)
#add_subdirectory(pd)

