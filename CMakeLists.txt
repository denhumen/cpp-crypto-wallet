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
file(GLOB HEADERS ${CMAKE_SOURCE_DIR}/include/*.hpp ${CMAKE_SOURCE_DIR}/include/*.h)
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME}
        main.cpp
        ${HEADERS}
        ${SOURCES}
        src/solana/solana_key_manager.cpp
        include/solana/solana_key_manager.hpp
        src/solana/solana_network.cpp
        include/solana/solana_network.hpp
        src/solana/solana_wallet.cpp
        include/solana/solana_wallet.hpp
        src/solana/solana_utils.cpp
        include/solana/solana_utils.hpp
        include/base58.cpp
        include/base58.hpp
)

add_executable(${PROJECT_NAME}2
        main2.cpp
        ${HEADERS}
        ${SOURCES}
        src/solana/solana_key_manager.cpp
        include/solana/solana_key_manager.hpp
        src/solana/solana_network.cpp
        include/solana/solana_network.hpp
        src/solana/solana_wallet.cpp
        include/solana/solana_wallet.hpp
        src/solana/solana_utils.cpp
        include/solana/solana_utils.hpp
        include/base58.cpp
        include/base58.hpp
        main2.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        OpenSSL::Crypto
        nlohmann_json::nlohmann_json
        CURL::libcurl
        sodium
)
target_link_libraries(${PROJECT_NAME}2 PRIVATE
        OpenSSL::Crypto
        nlohmann_json::nlohmann_json
        CURL::libcurl
        sodium
)


target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME}2 PRIVATE include)

INSTALL(PROGRAMS
        $<TARGET_FILE:${PROJECT_NAME}>
        DESTINATION bin)

INSTALL(PROGRAMS
        $<TARGET_FILE:${PROJECT_NAME}2>
        DESTINATION bin2)