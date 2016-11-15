###############################################################################
cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project("journal")
###############################################################################

#______________________________________________________________________________
#                                                                       VERSION
set(VERSION_MAJOR   0   CACHE STRING "Project major version number.")
set(VERSION_MINOR   1   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   0   CACHE STRING "Project patch version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)


#______________________________________________________________________________
#                                                                         DEBUG
message( STATUS "Include debug symbols with -DCMAKE_BUILD_TYPE=Debug")
set(CMAKE_C_FLAGS_DEBUG "-ggdb -O0")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb -O0")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-ggdb -O0")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-ggdb -O0")


#______________________________________________________________________________
#                                                                          ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS Core RIO Net Hist Graf Tree MathCore)
include(${ROOT_USE_FILE})
add_definitions(${ROOT_CXX_FLAGS})

#______________________________________________________________________________
#                                                                    GITVERSION
#list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/gitversion/")
#include(GetGitRevisionDescription)
#get_git_head_revision(GIT_REFSPEC GIT_HASH)
#git_describe(GIT_VERSION)
#MESSAGE(STATUS "git version: ${GIT_VERSION}")


#______________________________________________________________________________
#                                                                      INCLUDES
include_directories(${CMAKE_SOURCE_DIR}
                    ${CMAKE_SOURCE_DIR}/include
                    ${ROOT_INCLUDE_DIRS})


#______________________________________________________________________________
#                                                                         FLAGS
add_definitions("-Wall -Wextra -Wno-long-long -std=c++11")


#______________________________________________________________________________
#                                                                       TARGETS
add_subdirectory(src)
add_subdirectory(doc)

#______________________________________________________________________________
#                                                                       INSTALL
export(PACKAGE ${PROJECT_NAME})
#configure_file(cmake/${PROJECT_NAME}Config.cmake.in
#               "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
#               @ONLY)

install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        DESTINATION ${CMAKE_INSTALL_PREFIX}/cmake)

install(EXPORT ${PROJECT_NAME}LibraryDepends
        DESTINATION cmake )
