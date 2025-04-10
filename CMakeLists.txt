cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME cpp_crypto_wallet)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

include_directories(include)

find_package(OpenSSL REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(CURL REQUIRED)

FetchContent_Declare(
        libsodium
        GIT_REPOSITORY https://github.com/jedisct1/libsodium.git
        GIT_TAG stable
)
FetchContent_MakeAvailable(libsodium)

file(GLOB INTERFACES ${CMAKE_SOURCE_DIR}/include/interfaces/*.hpp)
file(GLOB SOLANA ${CMAKE_SOURCE_DIR}/include/solana/*.hpp ${CMAKE_SOURCE_DIR}/src/solana/*.cpp)
file(GLOB ENC ${CMAKE_SOURCE_DIR}/include/encoding/*.hpp ${CMAKE_SOURCE_DIR}/src/encoding/*.cpp)
file(GLOB CLI ${CMAKE_SOURCE_DIR}/include/cli/*.hpp ${CMAKE_SOURCE_DIR}/src/cli/*.cpp)

add_executable(${PROJECT_NAME}
        main.cpp
        ${INTERFACES}
        ${SOLANA}
        ${ENC}
        ${CLI}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        OpenSSL::Crypto
        nlohmann_json::nlohmann_json
        CURL::libcurl
        sodium
)

target_include_directories(${PROJECT_NAME} PRIVATE include
        include/interfaces
        include/solana
        include/encoding
        include/cli)

INSTALL(PROGRAMS
        $<TARGET_FILE:${PROJECT_NAME}>
        DESTINATION bin)
