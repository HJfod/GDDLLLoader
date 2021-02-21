cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME ModLdr)

project(${PROJECT_NAME})

target_include_directories(
    ${PROJECT_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/submodules/CappuccinoSDK/template/default-cpp/cappuccino-sdk/incl/cocos2dx/include
    ${PROJECT_SOURCE_DIR}/submodules/CappuccinoSDK/template/default-cpp/cappuccino-sdk/incl/cocos2dx
    ${PROJECT_SOURCE_DIR}/submodules/CappuccinoSDK/template/default-cpp/cappuccino-sdk/incl/cocos2dx/include
    ${PROJECT_SOURCE_DIR}/submodules/CappuccinoSDK/template/default-cpp/cappuccino-sdk/incl/cocos2dx/kazmath/include
    ${PROJECT_SOURCE_DIR}/submodules/CappuccinoSDK/template/default-cpp/cappuccino-sdk/incl/cocos2dx/platform/win32
    ${PROJECT_SOURCE_DIR}/submodules/CappuccinoSDK/template/default-cpp/cappuccino-sdk/incl/cocos2dx/platform
    ${PROJECT_SOURCE_DIR}/submodules/MinHook/include
)

set(SOURCES
    ${PROJECT_SOURCE_DIR}/main.cpp
    ${PROJECT_SOURCE_DIR}/src/ModLdr.cpp
    ${PROJECT_SOURCE_DIR}/src/console.cpp
    ${PROJECT_SOURCE_DIR}/src/layer/SettingsLayer.cpp
    ${PROJECT_SOURCE_DIR}/submodules/CappuccinoSDK/template/default-cpp/cappuccino-sdk/incl/cocos2dx/include/cocos2d.h
)

add_executable(${PROJECT_NAME} ${SOURCES})


