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


# Use find_package with the specified Protobuf root

# Ensure the Protobuf include directories are added

# Link Protobuf to your project

include_directories(
        ${WALLET_CORE}/include
        ${WALLET_CORE}/src
        ${WALLET_CORE}/build/local/include
        ${WALLET_CORE}/generated
        include
        include/helpers
        include/solana
        include/wallet
        include/crypto
)
include_directories(${Protobuf_INCLUDE_DIRS})

link_directories(
        ${WALLET_CORE}/build
        ${WALLET_CORE}/build/trezor-crypto
        ${WALLET_CORE}/build/local/lib
        ${WALLET_CORE}/rust/target/release
)

find_package(OpenSSL REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(CURL REQUIRED)
find_package(Protobuf REQUIRED)
find_library(SECP256K1_LIB NAMES secp256k1)
if (NOT SECP256K1_LIB)
    message(FATAL_ERROR "Could not find libsecp256k1 (install libsecp256k1-dev)")
endif()


set(Qt6_DIR ~/Qt/6.9.0/gcc_64/lib/cmake/Qt6)
find_package(Qt6 COMPONENTS Core Widgets Gui REQUIRED)

# Add UI Files (mainwindow.ui)
set(UIS_FILES src/ui/mainwindow.ui)
qt6_wrap_ui(UIS_HEADERS ${UIS_FILES})

# Ensure MOC (Meta-Object Compiler) is set up
set(MOC_FILES   src/ui/mainwindow.h)
qt6_wrap_cpp(MOC_OUTPUT ${MOC_FILES})  # Generate moc files

# Include your source files (helpers, solana, and mainwindow.cpp)
file(GLOB HELPERS src/helpers/*.cpp)
file(GLOB SOLANA src/solana/*.cpp)
file(GLOB WALLET src/wallet/*.cpp)
file(GLOB CRYPTO src/crypto/*.cpp)

set(SRC_FILES
        main_qt.cpp
        ${HELPERS}
        ${SOLANA}
        ${WALLET}
        ${CRYPTO}
        src/ui/mainwindow.cpp   # Make sure this is included
        ${UIS_HEADERS}
        ${MOC_OUTPUT}  # Ensure moc files are linked
        include/bitoc/BitCoinHelper.hpp
        src/bitoc/BitCoinHelper.cpp
        include/encoding/Base58.hpp
        src/encoding/Base58.hpp.cpp
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

# Ensure the generated header file is included
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Protobuf_LIBRARIES})


# Link Trust Wallet Core and Qt
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
        Qt6::Core
        Qt6::Widgets
        Qt6::Gui
        ${SECP256K1_LIB}
)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)