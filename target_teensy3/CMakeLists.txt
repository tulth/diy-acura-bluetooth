cmake_minimum_required (VERSION 2.8)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
project (mbus)

##############################
# Cross compile...
INCLUDE(CMakeForceCompiler)
 
SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_PROCESSOR arm)
SET(CMAKE_SYSTEM_VERSION 1)
 
# specify the cross compiler
CMAKE_FORCE_C_COMPILER(arm-none-eabi-gcc GNU)
CMAKE_FORCE_CXX_COMPILER(arm-none-eabi-g++ GNU)

set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "") 
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "") 
set(CMAKE_SHARED_LIBRARY_LINK_ASM_FLAGS "")

# Set library options
set(SHARED_LIBS OFF)
set(STATIC_LIBS ON)

SET(COMMON_FLAGS "-mthumb -mcpu=cortex-m4 -ffunction-sections -fdata-sections  -Wpedantic -Wextra -Os")
SET(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=gnu++0x -felide-constructors -fno-exceptions -fno-rtti")
# SET(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu99")
SET(CMAKE_C_FLAGS "${COMMON_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections -specs=nano.specs -specs=nosys.specs -T../teensy3_lib/mk20dx256.ld ")

##############################
include_directories("./teensy3_lib" "../common/mbus" "./arm_lib/include" )

add_definitions(-DUSB_SERIAL -DLAYOUT_US_ENGLISH -DTEENSYDUINO=124 -DF_CPU=48000000 -D__MK20DX256__ )

file(GLOB MBUS_SRC "../common/mbus/*.cpp" "../common/mbus/*.c" "teensy3_lib/*.c" "teensy3_lib/*.cpp")
add_executable(mbus_main.elf src/mbus_main.cpp ${MBUS_SRC})

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/mbus_main.hex
                   COMMAND arm-none-eabi-objcopy -O ihex -R .eeprom mbus_main.elf mbus_main.hex
                   DEPENDS mbus_main.elf
                   COMMENT "objcopying mbus_main to mbus_main.hex")
add_custom_target(mbus_main.hex ALL DEPENDS ${CMAKE_BINARY_DIR}/mbus_main.hex)

add_executable(vcd_main.elf src/vcd_main.cpp ${MBUS_SRC})

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/vcd_main.hex
                   COMMAND arm-none-eabi-objcopy -O ihex -R .eeprom vcd_main.elf vcd_main.hex
                   DEPENDS vcd_main.elf
                   COMMENT "objcopying vcd_main to vcd_main.hex")
add_custom_target(vcd_main.hex ALL DEPENDS ${CMAKE_BINARY_DIR}/vcd_main.hex)
