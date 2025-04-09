cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME cpp_crypto_wallet)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

find_package(OpenSSL REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

file(GLOB HEADERS ${CMAKE_SOURCE_DIR}/include/*.hpp ${CMAKE_SOURCE_DIR}/include/*.h)
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME}
        main.cpp
        ${HEADERS}
        ${SOURCES}
)

target_link_libraries(${PROJECT_NAME} OpenSSL::Crypto)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

target_include_directories(${PROJECT_NAME} PRIVATE include)

INSTALL(PROGRAMS
        $<TARGET_FILE:${PROJECT_NAME}>
        DESTINATION bin)