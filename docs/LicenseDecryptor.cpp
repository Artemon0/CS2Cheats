#include "LicenseDecryptor.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <windows.h>
#include <regex>

// Simple XOR decryption
std::string LicenseDecryptor::xor_decrypt(const std::string& encrypted, const std::string& key) {
    std::string decrypted = encrypted;
    for (size_t i = 0; i < encrypted.length(); ++i) {
        decrypted[i] ^= key[i % key.length()];
    }
    return decrypted;
}

// Base64 decoding implementation
std::string LicenseDecryptor::base64_decode(const std::string& encoded) {
    const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string decoded;
    int val = 0, valb = -8;
    
    for (unsigned char c : encoded) {
        if (c == '=') break;
        if (chars.find(c) == std::string::npos) continue;
        
        val = (val << 6) + chars.find(c);
        valb += 6;
        if (valb >= 0) {
            decoded.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return decoded;
}

// Generate hardware-based key
std::string LicenseDecryptor::generate_key_from_hwid() {
    DWORD volumeSerialNumber;
    GetVolumeInformationA("C:\\", NULL, 0, &volumeSerialNumber, NULL, NULL, NULL, 0);
    
    std::string hwid = std::to_string(volumeSerialNumber);
    std::string key = "CS2_LICENSE_KEY_" + hwid.substr(0, 8);
    return key;
}

LicenseInfo LicenseDecryptor::decrypt_license(const std::string& encrypted_data) {
    LicenseInfo info;
    
    try {
        // Step 1: Base64 decode
        std::string base64_decoded = base64_decode(encrypted_data);
        
        // Step 2: XOR decrypt with hardware-based key
        std::string key = generate_key_from_hwid();
        std::string json_data = xor_decrypt(base64_decoded, key);
        
        // Step 3: Parse JSON
        Json::Value root;
        Json::Reader reader;
        
        if (!reader.parse(json_data, root)) {
            throw std::runtime_error("Failed to parse license JSON");
        }
        
        // Extract license information
        if (root.isMember("license_key")) {
            info.license_key = root["license_key"].asString();
        }
        if (root.isMember("telegram_id")) {
            info.telegram_id = root["telegram_id"].asInt64();
        }
        if (root.isMember("username")) {
            info.username = root["username"].asString();
        }
        if (root.isMember("plan")) {
            info.plan = root["plan"].asString();
        }
        if (root.isMember("hwid")) {
            info.hwid = root["hwid"].asString();
        }
        if (root.isMember("is_active")) {
            info.is_active = root["is_active"].asBool();
        }
        if (root.isMember("created_at")) {
            info.created_at = root["created_at"].asString();
        }
        if (root.isMember("expires_at")) {
            info.expires_at = root["expires_at"].asString();
        }
        if (root.isMember("activated_at")) {
            info.activated_at = root["activated_at"].asString();
        }
        
    } catch (const std::exception& e) {
        std::cerr << "License decryption error: " << e.what() << std::endl;
        // Return empty license info on error
        return LicenseInfo{};
    }
    
    return info;
}

bool LicenseDecryptor::validate_license(const LicenseInfo& license) {
    if (license.license_key.empty() || !license.is_active) {
        return false;
    }
    
    // Check if license is expired
    if (!license.expires_at.empty() && license.expires_at != "null") {
        // Simple date validation - in production, use proper date parsing
        if (license.expires_at.find("2025") == std::string::npos) {
            // This is a simplified check - implement proper date validation
        }
    }
    
    return true;
}

std::vector<LicenseInfo> LicenseDecryptor::load_licenses_from_file(const std::string& file_path) {
    std::vector<LicenseInfo> licenses;
    
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Failed to open license file: " << file_path << std::endl;
        return licenses;
    }
    
    try {
        Json::Value root;
        Json::Reader reader;
        
        if (!reader.parse(file, root)) {
            std::cerr << "Failed to parse license file JSON" << std::endl;
            return licenses;
        }
        
        if (root.isMember("licenses") && root["licenses"].isArray()) {
            for (const auto& license_json : root["licenses"]) {
                if (license_json.isMember("encrypted_data")) {
                    std::string encrypted_data = license_json["encrypted_data"].asString();
                    LicenseInfo license = decrypt_license(encrypted_data);
                    if (!license.license_key.empty()) {
                        licenses.push_back(license);
                    }
                }
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading licenses: " << e.what() << std::endl;
    }
    
    file.close();
    return licenses;
}

bool LicenseDecryptor::is_license_valid_for_current_hwid(const LicenseInfo& license) {
    // Get current HWID
    DWORD volumeSerialNumber;
    GetVolumeInformationA("C:\\", NULL, 0, &volumeSerialNumber, NULL, NULL, NULL, 0);
    
    // Convert to hex string (same format as in the license)
    char current_hwid[32];
    sprintf_s(current_hwid, "%08x", volumeSerialNumber);
    
    // Compare with license HWID (case insensitive)
    return _stricmp(current_hwid, license.hwid.c_str()) == 0;
}
