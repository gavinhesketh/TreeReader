#
cmake_minimum_required(VERSION 3.17)
project(TreeReader CXX)

find_package(ROOT REQUIRED COMPONENTS Core RIO Tree Hist)

# mu3eUtil is optional: if found, TreeReader uses mu3e::RootFile,
# otherwise it falls back to plain TFile.
find_package(mu3eUtil QUIET)
if(mu3eUtil_FOUND)
    message(STATUS "Found mu3eUtil: using mu3e::RootFile")
else()
    message(STATUS "mu3eUtil not found: falling back to TFile")
endif()

set(TARGETS TreeReader)

foreach(TARGET ${TARGETS})
    add_executable(UCL${TARGET} ${TARGET}.cpp)
    target_include_directories(UCL${TARGET} PUBLIC include)
    target_link_libraries(UCL${TARGET} PRIVATE ROOT::Core ROOT::RIO ROOT::Tree ROOT::Hist)
    if(mu3eUtil_FOUND)
        target_compile_definitions(UCL${TARGET} PRIVATE HAVE_MU3EUTIL)
        if(TARGET mu3eUtil::mu3eUtil)
            target_link_libraries(UCL${TARGET} PRIVATE mu3eUtil::mu3eUtil)
        else()
            target_include_directories(UCL${TARGET} PRIVATE ${mu3eUtil_INCLUDE_DIRS})
            target_link_libraries(UCL${TARGET} PRIVATE ${mu3eUtil_LIBRARIES})
        endif()
    endif()
    install(TARGETS UCL${TARGET} DESTINATION bin)
endforeach()
