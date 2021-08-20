cmake_minimum_required(VERSION 3.20)
project(chess_engine_c)

set(CMAKE_CXX_STANDARD 20)

set(SKIACC_SHARED OFF)
set(SKIACC_ALL_MODULES ON)
set(SKIACC_COPY_INCLUDE ON)
set(SKIACC_COMMIT "master")
set(SKIACC_ARGS "skia_enable_svg=true")
add_subdirectory(libs/skia_mess/)
include_directories(libs/skia_mess/skia/)

find_package(glfw3 3.3 REQUIRED)

add_executable(chess_engine_c
        # entry
        src/main.cpp

        # engine
        src/engine/random_engine.cpp

        # game
        src/game/board.cpp
        src/game/find_moves.cpp

        # ui
        src/ui/rendermain.cpp
        src/ui/layers/background_layer.cpp
        src/ui/layers/piece_layer.cpp
        src/ui/layers/moves_layer.cpp

        # utils
        src/util/random.cpp
        src/engine/minmax_engine.cpp src/engine/minmax_engine.h src/engine/board_evaluation.cpp src/engine/board_evaluation.h)

target_link_libraries(chess_engine_c glfw skiacc)
