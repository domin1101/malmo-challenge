cmake_minimum_required(VERSION 3.6)
project(Malmo)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set (CMAKE_CXX_FLAGS "--std=gnu++11 ${CMAKE_CXX_FLAGS}")
    endif ()

file(GLOB_RECURSE SOURCES src/*.cpp src/*.hpp)
include_directories(src)

set(wxWidgets_CONFIGURATION mswu)
find_package(wxWidgets COMPONENTS core base richtext adv html xml aui REQUIRED)
include(${wxWidgets_USE_FILE})

add_executable(Malmo ${SOURCES})
target_link_libraries(Malmo LightBulbAppd LightBulbd freechart ${wxWidgets_LIBRARIES})
