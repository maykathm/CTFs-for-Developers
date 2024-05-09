set(Boost_USE_STATIC_LIBS OFF)
find_package(Boost COMPONENTS unit_test_framework)

set(test_exe ${PROJECT_NAME}_test)
add_executable(${test_exe})

target_link_libraries(${test_exe} PRIVATE ${PROJECT_NAME}::${PROJECT_NAME})
target_include_directories(${test_exe} PRIVATE "../src")

# Link to boost library for unit testing.
target_link_libraries(${test_exe} PRIVATE ${Boost_LIBRARIES})

target_sources(${test_exe} PRIVATE
  BoostMain.cpp
  AccountTest.cpp
)