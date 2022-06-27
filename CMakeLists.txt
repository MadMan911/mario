cmake_minimum_required(VERSION 3.21)
project(platformer)

set(CMAKE_CXX_STANDARD 17)

add_executable(platformer src/main.cpp src/context.hpp src/Game.cpp src/Game.hpp src/InputController.hpp src/MenuState.cpp src/MenuState.hpp src/StateMachine.cpp src/StateMachine.hpp src/TextureManager.hpp)

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/SFML/lib/cmake/SFML)
find_package(SFML 2.5.1 COMPONENTS system window graphics audio network REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/SFML/include)
target_link_libraries(platformer sfml-system sfml-window sfml-graphics sfml-audio)

add_library(default INTERFACE)
target_compile_features(default INTERFACE cxx_std_17)
enable_testing()

# Taken from homework files
add_library(doctest STATIC external/doctest.cpp)
target_include_directories(doctest PUBLIC external/)
set_target_properties(doctest PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
add_library(doctest::doctest ALIAS doctest)
add_library(Catch2::Catch2 ALIAS doctest)

add_executable(project-tests tests/tests.cpp src/context.hpp src/Game.cpp src/Game.hpp src/InputController.hpp src/MenuState.cpp src/MenuState.hpp src/StateMachine.cpp src/StateMachine.hpp src/TextureManager.hpp)
target_link_libraries(project-tests PUBLIC doctest default sfml-system sfml-window sfml-graphics sfml-audio  )
#target_link_libraries(project-tests sfml-system sfml-window sfml-graphics sfml-audio)
add_test(NAME project-tests COMMAND project-tests --force-colors -d)
