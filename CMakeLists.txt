cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME cpp_crypto_wallet)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"))
    message(FATAL_ERROR "You should use clang compiler")
endif()

set(WALLET_CORE /home/denhumen/Downloads/wallet-core)

include_directories(
        ${WALLET_CORE}/include
        ${WALLET_CORE}/src
        ${WALLET_CORE}/build/local/include
        include
        include/helpers
        include/solana
#        include/interfaces
#        include/solana
#        include/encoding
#        include/cli
)

link_directories(
        ${WALLET_CORE}/build
        ${WALLET_CORE}/build/trezor-crypto
        ${WALLET_CORE}/build/local/lib
        ${WALLET_CORE}/rust/target/release
)

find_package(OpenSSL REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(CURL REQUIRED)

include(FetchContent)
FetchContent_Declare(
        libsodium
        GIT_REPOSITORY https://github.com/jedisct1/libsodium.git
        GIT_TAG stable
)
FetchContent_MakeAvailable(libsodium)

#file(GLOB INTERFACES ${CMAKE_SOURCE_DIR}/include/interfaces/*.hpp)
#file(GLOB SOLANA ${CMAKE_SOURCE_DIR}/include/solana/*.hpp ${CMAKE_SOURCE_DIR}/src/solana/*.cpp)
#file(GLOB ENC ${CMAKE_SOURCE_DIR}/include/encoding/*.hpp ${CMAKE_SOURCE_DIR}/src/encoding/*.cpp)
#file(GLOB CLI ${CMAKE_SOURCE_DIR}/include/cli/*.hpp ${CMAKE_SOURCE_DIR}/src/cli/*.cpp)
file(GLOB HELPERS src/helpers/*.cpp)
file(GLOB SOLANA src/solana/*.cpp)


add_executable(${PROJECT_NAME}
        main.cpp
        ${HELPERS}
        ${SOLANA}
)

# Link Trust Wallet Core and its dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE
        ${WALLET_CORE}/build/libTrustWalletCore.a
        ${WALLET_CORE}/build/trezor-crypto/libTrezorCrypto.a
        ${WALLET_CORE}/build/local/lib/libprotobuf.a
        ${WALLET_CORE}/rust/target/release/libwallet_core_rs.a
        OpenSSL::Crypto
        nlohmann_json::nlohmann_json
        CURL::libcurl
        sodium
        pthread
)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
