cmake_minimum_required(VERSION 3.20)

project(mu3eSort)

#include debug hooks:
#SET(CMAKE_BUILD_TYPE "Debug")


##ROOT stuff:
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS)

## BOOST:
set(Boost_USE_STATIC_LIBS OFF)
find_package(Boost 1.74 REQUIRED COMPONENTS program_options )
message(STATUS "[mu3eSort] Boost_VERSION: ${Boost_VERSION}")


add_executable(mu3eSort mu3eSort.cpp)
target_link_libraries(mu3eSort PUBLIC ${Boost_LIBRARIES}  ${ROOT_LIBRARIES})
target_include_directories(mu3eSort SYSTEM PUBLIC ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})


install(TARGETS mu3eSort DESTINATION bin)