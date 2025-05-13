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
};

#endif //JSONRPCHELPER_HPP
