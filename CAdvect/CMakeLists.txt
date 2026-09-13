cmake_minimum_required(VERSION 3.23)

project(CAdvect)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# make commands file for clang
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# find addiitonal libraries
find_package(yaml-cpp REQUIRED)

# finding netcdf is a challenge
find_package(netCDF REQUIRED)
find_path(NETCDF_CXX_INCLUDE_DIR netcdf
    PATHS /opt/homebrew/opt/netcdf-cxx/include
)
find_library(NETCDF_CXX_LIBRARY
    NAMES netcdf-cxx4
    PATHS /opt/homebrew/opt/netcdf-cxx/lib
)

add_executable(CAdvect)
target_sources(CAdvect PRIVATE CAdvect.cpp Fields.cpp Particles.cpp)

target_include_directories(CAdvect PRIVATE ${NETCDF_CXX_INCLUDE_DIR})
target_link_libraries(CAdvect
    PRIVATE
        yaml-cpp::yaml-cpp
        netCDF::netcdf
        ${NETCDF_CXX_LIBRARY}
)