#include "bitoc/BitCoinHelper.hpp"
#include "JsonRpcHelper.hpp"
#include <nlohmann/json.hpp>
#include <curl/curl.h>

using json = nlohmann::json;

const std::string BITOC_API_URL = "https://enterprise.blockstream.info/api/";
const std::string CLIENT_ID     = "9aaabd6f-b56c-4be0-847e-75dd0829ef87";
const std::string CLIENT_SECRET = "hOFGtgB8ABwADcI4g0ZTKV70jxmK2HBQ";
const std::string OAUTH_TOKEN_URL = "https://login.blockstream.com/realms/blockstream-public/protocol/openid-connect/token";


std::string BitCoinHelper::getBalance(const std::string &address) {
    if (address.empty()) {
        return "Error: Address is empty";
    }

    std::string token = JsonRpcHelper::getOAuthToken(
            CLIENT_ID, CLIENT_SECRET, OAUTH_TOKEN_URL
    );
    if (token.empty()) {
        return "Error: failed to obtain OAuth token";
    }

    std::string url = BITOC_API_URL + "address/" + address;
    auto response = JsonRpcHelper::sendApiRequest(url, token);
    return response;
}

