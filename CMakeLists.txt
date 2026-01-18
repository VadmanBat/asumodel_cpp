cmake_minimum_required(VERSION 3.20)
project(asumodel_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Оптимизация скорости
add_compile_options(-O3 -march=native -funroll-loops -ffast-math)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/pps_tren.cpp
        src/asu_monitor.cpp
        src/udp_handler.cpp
        src/dbf_reader.cpp
        src/models/valm00.cpp
        src/models/valve_kruzap.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE include)

# Многопоточность (обязательно для MinGW)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

# Сокеты Windows
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
endif()

# Копирование DLL
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/lib/ppsdll.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Копируем ppsdll.dll"
)