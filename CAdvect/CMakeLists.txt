cmake_minimum_required(VERSION 3.23)

project(CAdvect)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# make commands file for clang
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# find addiitonal libraries
find_package(yaml-cpp REQUIRED)

add_executable(CAdvect)
target_sources(CAdvect
    PRIVATE
        CAdvect.cpp
        Fields.cpp
        Particles.cpp
)



target_link_libraries(CAdvect
    PRIVATE
        yaml-cpp::yaml-cpp
)
