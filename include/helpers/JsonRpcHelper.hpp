//
// Created by denhumen on 5/13/25.
//

#ifndef JSONRPCHELPER_HPP
#define JSONRPCHELPER_HPP

#include <string>
#include <nlohmann/json.hpp>

class JsonRpcHelper {
public:
    static std::string sendJsonRpcRequest(const std::string& url, const nlohmann::json& payload);
    static std::string sendApiRequest(const std::string& url, const std::string& api_token="");
    static std::string getOAuthToken(const std::string& clientId,
                                     const std::string& clientSecret,
                                     const std::string& tokenUrl);
};

#endif //JSONRPCHELPER_HPP
