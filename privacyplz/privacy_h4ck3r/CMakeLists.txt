cmake_minimum_required(VERSION 3.16)

project(privacy_user)
set(CMAKE_CXX_STANDARD 20)

find_package(privacyplz REQUIRED)

add_executable(privacy_user main.cpp)

target_link_libraries(privacy_user PRIVATE privacyplz::privacyplz)

