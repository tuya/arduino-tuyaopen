##
# @file CMakeLists.txt
# @brief 
#/

# MODULE_PATH
set(MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

# MODULE_NAME
get_filename_component(MODULE_NAME ${MODULE_PATH} NAME)

# Arduino Board (t2/t3)
if(NOT DEFINED CONFIG_ARDUINO_BOARD)
    message(FATAL_ERROR "Use [menuconfig] choice [ARDUINO_BOARD].")
endif()
if(${CONFIG_ARDUINO_BOARD} STREQUAL "ARDUINO_TUYA_T3")
    set(ARDUINO_BOARD "t3")
    set(LD_FILE "${MODULE_PATH}/port/${PLATFORM_NAME}/bk7236_out.ld")
else()
    set(CONFIG_ARDUINO_BOARD "ARDUINO_TUYA_T2")
    set(ARDUINO_BOARD "t2")
    set(LD_FILE "${MODULE_PATH}/port/${PLATFORM_NAME}/bk7231n_ota.ld")
endif()

# LIB_SRCS
file(GLOB_RECURSE
    LIB_SRCS_CORE
    "${MODULE_PATH}/cores/tuya_open/*.cpp"
    "${MODULE_PATH}/cores/tuya_open/*.c"
    "${MODULE_PATH}/cores/tuya_open/api/*.cpp"
    )
file(GLOB_RECURSE
    LIB_SRCS_LIB
    "${MODULE_PATH}/libraries/Log/src/*.cpp"
    "${MODULE_PATH}/libraries/SPI/src/*.cpp"
    "${MODULE_PATH}/libraries/Ticker/src/*.cpp"
    "${MODULE_PATH}/libraries/TuyaIoT/src/*.cpp"
    "${MODULE_PATH}/libraries/TuyaIoT/src/*.c"
    )
file(GLOB_RECURSE
    LIB_SRCS_VAR
    "${MODULE_PATH}/variants/${ARDUINO_BOARD}/*.c"
    )
list(APPEND LIB_SRCS ${LIB_SRCS_CORE})
list(APPEND LIB_SRCS ${LIB_SRCS_LIB})
list(APPEND LIB_SRCS ${LIB_SRCS_VAR})

# LIB_PUBLIC_INC
set(LIB_PUBLIC_INC
    "${MODULE_PATH}/cores/tuya_open"
    "${MODULE_PATH}/variants/${ARDUINO_BOARD}"
    )

# FLAGS
set(CMAKE_CXX_FLAGS "@${MODULE_PATH}/port/${PLATFORM_NAME}/flags/cpp_flags.txt")

########################################
# Target Configure
########################################
add_library(${MODULE_NAME})

target_sources(${MODULE_NAME}
    PRIVATE
        ${LIB_SRCS}
    )

target_include_directories(${MODULE_NAME}
    PUBLIC
        ${LIB_PUBLIC_INC}
    )

target_compile_options(${MODULE_NAME}
    PRIVATE
    -D${CONFIG_ARDUINO_BOARD}
    )


########################################
# Layer Configure
########################################
list(APPEND COMPONENT_LIBS ${MODULE_NAME})
# set(COMPONENT_LIBS "${COMPONENT_LIBS}" PARENT_SCOPE)
list(APPEND COMPONENT_PUBINC ${LIB_PUBLIC_INC})
# set(COMPONENT_PUBINC "${COMPONENT_PUBINC}" PARENT_SCOPE)

