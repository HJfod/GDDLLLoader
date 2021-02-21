cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_NAME ModLdr)

project(${PROJECT_NAME} VERSION 1.0.0)

find_file(WINDOWS_HEADER windows.h)
if (NOT WINDOWS_HEADER)
  message(FATAL_ERROR "Can't find windows.h!")
endif()

file(GLOB_RECURSE SOURCES main.cpp src/*.cpp)

set(WIN32 ON)
add_library(${PROJECT_NAME} SHARED ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

add_subdirectory(submodules/MinHook)
target_link_libraries(${PROJECT_NAME} submodules/MinHook/Release/minhook.x32)

target_include_directories(
    ${PROJECT_NAME} PRIVATE
    submodules/CappuccinoSDK/template/default-cpp/cappuccino-sdk/incl/cocos2dx
    submodules/CappuccinoSDK/template/default-cpp/cappuccino-sdk/incl/cocos2dx/include
    submodules/CappuccinoSDK/template/default-cpp/cappuccino-sdk/incl/cocos2dx/kazmath/include
    submodules/CappuccinoSDK/template/default-cpp/cappuccino-sdk/incl/cocos2dx/platform/third_party/win32/OGLES
    submodules/CappuccinoSDK/template/default-cpp/cappuccino-sdk/incl/cocos2dx/platform/win32
    submodules/MinHook/include
)

target_link_libraries(
    ${PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/submodules/CappuccinoSDK/template/default-cpp/cappuccino-sdk/lib/cappuccino.lib
)

