#include "Utils.h"

std::vector<unsigned char>Utils::MACsToUCharVector(std::vector<std::string> MACs)
{
    std::vector<unsigned char> binary_data;

    for (const auto& mac : MACs)
    {
        auto bytes = parseMAC(mac);
        binary_data.insert(binary_data.end(), bytes.begin(), bytes.end());
    }
    return binary_data;
}
std::vector<unsigned char> Utils::parseMAC(const std::string& mac_str) 
{
    std::vector<unsigned char> mac_bytes;
    std::istringstream iss(mac_str);
    std::string byte_str;

    while (std::getline(iss, byte_str, ':')) {
        int byte = std::stoi(byte_str, nullptr, 16);
        mac_bytes.push_back(static_cast<unsigned char>(byte));
    }

    if (mac_bytes.size() != 6) {
        throw std::invalid_argument("Invalid MAC address format");
    }
    return mac_bytes;
}