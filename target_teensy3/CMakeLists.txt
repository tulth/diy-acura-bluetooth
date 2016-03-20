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

SET(COMMON_FLAGS "-mthumb -mcpu=cortex-m4 -ffunction-sections -fdata-sections -Os")
SET(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=gnu++0x -felide-constructors -fno-exceptions -fno-rtti")
# SET(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu99")
SET(CMAKE_C_FLAGS "${COMMON_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections -specs=nano.specs -specs=nosys.specs -T../submodule_teensy_cores/teensy3/mk20dx256.ld ")
add_definitions(-DUSB_SERIAL -DLAYOUT_US_ENGLISH -DTEENSYDUINO=124 -DF_CPU=48000000 -D__MK20DX256__ )
add_definitions(-DEMBEDDED )
# add_definitions( -DDEBUG_MBUS_PHY )
# add_definitions( -DDEBUG_MBUS_LINK )
add_definitions( -DDEBUG_RN52 )

include_directories("./submodule_teensy_cores/teensy3" "../common" "../common/mbus" "../common/rn52" "./arm_lib/include" )

##############################
file(GLOB TEENSY3_SRC "submodule_teensy_cores/teensy3/*.c" "submodule_teensy_cores/teensy3/*.cpp")
list(REMOVE_ITEM TEENSY3_SRC "${CMAKE_CURRENT_SOURCE_DIR}/submodule_teensy_cores/teensy3/main.cpp")
add_library(teensy3 STATIC ${TEENSY3_SRC})

##############################
file(GLOB CMN_SRC "../common/*.cpp" "../common/*.c" "../common/mbus/*.cpp" "../common/mbus/*.c" "../common/rn52/*.cpp" "../common/rn52/*.c")
add_library(cmn STATIC ${CMN_SRC})
set_target_properties(cmn PROPERTIES COMPILE_FLAGS "-Wpedantic -Wextra -Werror")

##############################
foreach( top_src mbus_toggle_sniffer.cpp mbus_nibble_sniffer.cpp mbus_msg_sniffer.cpp mbus_toggle_bridge.cpp mbus_nibble_bridge.cpp bt_bridge.cpp bt_demo.cpp mbus_msg_ping_reply.cpp app_demo.cpp app_demo_toggle_sniffer.cpp )
    string( REPLACE ".cpp" ".elf" top_elf ${top_src} )
    add_executable( ${top_elf} src/${top_src} )
    target_link_libraries( ${top_elf} cmn teensy3 )
    set_target_properties( ${top_elf} PROPERTIES COMPILE_FLAGS "-Wpedantic -Wextra -Werror")
    string( REPLACE ".cpp" ".hex" top_hex ${top_src} )
    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/${top_hex}
                       COMMAND arm-none-eabi-objcopy -O ihex -R .eeprom ${top_elf} ${top_hex}
                       DEPENDS ${top_elf}
                       COMMENT "objcopying ${top_elf} to ${top_hex}")
    add_custom_target( build_${top_hex} ALL DEPENDS ${CMAKE_BINARY_DIR}/${top_hex})
endforeach( top_src ${APP_SOURCES} )
