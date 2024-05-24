cmake_minimum_required(VERSION 3.16)

project(bind_this C)
set(CMAKE_C_STANDARD 23)

set(CMAKE_BUILD_TYPE Release)
add_executable(${PROJECT_NAME} 
    main.c 
    self_mod.c
    )

add_library(flag1 SHARED flag1.c)
target_link_options(flag1 PRIVATE "-Wl,-Bsymbolic")

target_link_libraries(${PROJECT_NAME} flag1)

add_library(flag2 SHARED flag2.c)

install(TARGETS ${PROJECT_NAME} flag1 flag2)