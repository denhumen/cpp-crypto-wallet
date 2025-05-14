//
// Created by denhumen on 5/14/25.
//

#include "SolanaWallet.hpp"
#include "JsonRpcHelper.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;
const std::string SOLANA_RPC_URL = "https://api.devnet.solana.com";

double SolanaWallet::getBalanceAsDouble(const std::string& address) const {
    json payload = {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getBalance"},
        {"params", {address}}
    };

    std::string response = JsonRpcHelper::sendJsonRpcRequest(SOLANA_RPC_URL, payload);
    try {
        json jsonResponse = json::parse(response);
        if (jsonResponse.contains("result") && jsonResponse["result"].contains("value")) {
            int64_t lamports = jsonResponse["result"]["value"];
            double solBalance = static_cast<double>(lamports) / 1e9; // Convert lamports to SOL
            return solBalance;
        }
    } catch (...) {
        return 0.0;
    }
    return 0.0;
}

std::string SolanaWallet::getBalanceAsString(const std::string& address) const {
    double balance = getBalanceAsDouble(address);

    std::ostringstream stream;
    stream << std::fixed << std::setprecision(5) << balance;
    std::string balanceStr = stream.str();

    balanceStr.erase(balanceStr.find_last_not_of('0') + 1);
    if (balanceStr.back() == '.') {
        balanceStr.pop_back();
    }

    balanceStr += " SOL";
    return balanceStr;
}

std::string SolanaWallet::signTransaction(TWHDWallet* wallet, const std::string& toAddress, double amountEth) const {
    return "Not implemented";
}
