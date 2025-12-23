#pragma once
#include <string>
#include <vector>
#include <json/json.h>

struct LicenseInfo {
    std::string license_key;
    long long telegram_id;
    std::string username;
    std::string plan;
    std::string hwid;
    bool is_active;
    std::string created_at;
    std::string expires_at;
    std::string activated_at;
};

class LicenseDecryptor {
private:
    static std::string xor_decrypt(const std::string& encrypted, const std::string& key);
    static std::string base64_decode(const std::string& encoded);
    static std::string generate_key_from_hwid();
    
public:
    static LicenseInfo decrypt_license(const std::string& encrypted_data);
    static bool validate_license(const LicenseInfo& license);
    static std::vector<LicenseInfo> load_licenses_from_file(const std::string& file_path);
    static bool is_license_valid_for_current_hwid(const LicenseInfo& license);
};
