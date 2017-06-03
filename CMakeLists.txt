cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 11)

project(MeanFilterINTELx86)

set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)
set(CMAKE_C_FLAGS -m32)
set(CMAKE_CXX_FLAGS -m32)
enable_language(ASM_NASM)


# Define sources and executable
set(SOURCE_FILES main.cpp mean_filter.asm mean_filter_cpp_ugly.h)

set(EXECUTABLE_NAME "MeanFilterINTELx86")
add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})

# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
#set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML 2 REQUIRED network audio graphics window system)
if(SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIR})
  target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
endif()