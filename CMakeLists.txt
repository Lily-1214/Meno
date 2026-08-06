cmake_minimum_required(VERSION 3.20)
project(GameObjectDemo
        VERSION 0.1.0
        LANGUAGES CXX
)

# ---------------------------------------------------------------------------
# 전역 설정
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

# ---------------------------------------------------------------------------
# 의존성: SFML (FetchContent)
# ---------------------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG        3.1.0
        GIT_SHALLOW    ON
)
# SFML 자체 예제/테스트는 빌드하지 않도록 옵션 끄기
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SFML)

# ---------------------------------------------------------------------------
# GameObjectDemo 라이브러리 타깃
# ---------------------------------------------------------------------------
file(GLOB_RECURSE GAMEOBJECTDEMO_SOURCES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gameobject_demo/*.cpp
)

add_library(GameObjectDemo STATIC ${GAMEOBJECTDEMO_SOURCES})
add_library(GameObjectDemo::GameObjectDemo ALIAS GameObjectDemo)

target_include_directories(GameObjectDemo
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(GameObjectDemo
        PUBLIC
        sfml-graphics
        sfml-window
        sfml-system
        sfml-audio
)

target_compile_features(GameObjectDemo PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(GameObjectDemo PRIVATE /W4)
else()
    target_compile_options(GameObjectDemo PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ---------------------------------------------------------------------------
# 데모 실행 파일 (src/main.cpp)
# ---------------------------------------------------------------------------
add_executable(GameObjectDemoApp src/main.cpp)
target_link_libraries(GameObjectDemoApp PRIVATE GameObjectDemo::GameObjectDemo)