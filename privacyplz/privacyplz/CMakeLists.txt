###################################################################################################################
##                                                                                                              ###
##                                                Privacy Plz                                                   ###
##                                                                                                              ###
###################################################################################################################

cmake_minimum_required(VERSION 3.16)

project(privacyplz)
set(CMAKE_CXX_STANDARD 20)

# Set a default build type if none was specified
set(default_build_type "Release")
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  set(default_build_type "Debug")
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Create library target
add_library(${PROJECT_NAME} SHARED)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Set the library version 
set_target_properties(${PROJECT_NAME}
    PROPERTIES SOVERSION 0
    VERSION 0.0.0
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDE_DIR}>
)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_subdirectory(src)
add_subdirectory(test)

###################################################################################################################
##                                                                                                              ###
##                                                    Install                                                   ###
##                                                                                                              ###
###################################################################################################################

include(GNUInstallDirs)

set(CMAKE_INSTALL_CMAKEDIR
    ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    CACHE STRING "Path to cpptemplate cmake files"
)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} 
        COMPONENT "${PROJECT_NAME}_Runtime"
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} 
        COMPONENT "${PROJECT_NAME}_Runtime"
        NAMELINK_COMPONENT "${PROJECT_NAME}_Development"
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT "${PROJECT_NAME}_Development"
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY
    include/${PROJECT_NAME}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

set(configFile ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake)
set(versionFile ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}.cmake.in
    ${configFile}
    INSTALL_DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
)

get_target_property(version ${PROJECT_NAME} VERSION)

write_basic_package_version_file(
    ${versionFile}
    VERSION ${version}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${configFile}
    ${versionFile}
    DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
)

install(EXPORT ${PROJECT_NAME}Targets
    DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
    NAMESPACE ${PROJECT_NAME}::
    FILE ${PROJECT_NAME}Targets.cmake
    COMPONENT ${PROJECT_NAME}_Development
)