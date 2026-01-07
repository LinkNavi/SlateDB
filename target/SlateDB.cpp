// Auto-generated C++ code from Magolor

// User-provided C++ headers

    #include <cstring>

// Module-required includes
#include <openssl/rand.h>
#include <openssl/evp.h>

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <iostream>
#include <string>
#include <algorithm>
#include <functional>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <random>
#include <chrono>
#include <thread>
#include <cmath>
#include <stdexcept>
#include <cstring>
#include <unistd.h>

// =======================================================================
// Auto-generated Standard Library Implementations
// =======================================================================

// =======================================================================
// Std.Crypto (Auto-generated from stdlib)
// =======================================================================
namespace Crypto {

    class CryptoResult {
    public:
        bool success = false;
        std::vector<int64_t> data;
        std::string error;
        std::vector<int64_t> iv;
        std::vector<int64_t> tag;
        std::vector<int64_t> salt;
    };

    class KeyConfig {
    public:
        int64_t iterations = 0;
        int64_t keyLength = 0;
    };

    inline void create() {
        this->data = std::vector<int64_t>();
                    this->iv = std::vector<int64_t>();
                    this->tag = std::vector<int64_t>();
                    this->salt = std::vector<int64_t>();
    }

    inline void create() {
        // Implementation not found
    }

    inline KeyConfig standardConfig() {
        // Implementation not found
    }

    inline KeyConfig fastConfig() {
        // Implementation not found
    }

    inline KeyConfig paranoidConfig() {
        // Implementation not found
    }

    inline std::vector<int64_t> deriveKey(const std::string& password, const std::vector<int64_t>& salt, KeyConfig config) {
        std::vector<int64_t> key(config.keyLength);
                std::vector<unsigned char> saltBytes(salt.size());
                for (size_t i = 0; i < salt.size(); i++) {
                    saltBytes[i] = static_cast<unsigned char>(salt[i]);
                }
                
                std::vector<unsigned char> keyBytes(config.keyLength);
                PKCS5_PBKDF2_HMAC(
                    password.c_str(), password.length(),
                    saltBytes.data(), saltBytes.size(),
                    config.iterations,
                    EVP_sha256(),
                    config.keyLength, keyBytes.data()
                );
                
                for (int64_t i = 0; i < config.keyLength; i++) {
                    key[i] = keyBytes[i];
                }
                return key;
    }

    inline std::vector<int64_t> randomBytes(int64_t count) {
        std::vector<unsigned char> bytes(count);
                RAND_bytes(bytes.data(), count);
                
                std::vector<int64_t> result(count);
                for (int64_t i = 0; i < count; i++) {
                    result[i] = bytes[i];
                }
                return result;
    }

    inline std::vector<int64_t> generateSalt() {
        // Implementation not found
    }

    inline std::vector<int64_t> generateIV() {
        // Implementation not found
    }

    inline CryptoResult encrypt(const std::vector<int64_t>& plaintext, const std::string& password) {
        CryptoResult result;
                result.success = false;
                result.data = std::vector<int64_t>();
                result.iv = std::vector<int64_t>();
                result.tag = std::vector<int64_t>();
                result.salt = std::vector<int64_t>();
                
                // Generate salt and IV
                std::vector<unsigned char> salt(16);
                std::vector<unsigned char> iv(12);
                RAND_bytes(salt.data(), 16);
                RAND_bytes(iv.data(), 12);
                
                // Derive key
                std::vector<unsigned char> key(32);
                PKCS5_PBKDF2_HMAC(
                    password.c_str(), password.length(),
                    salt.data(), salt.size(),
                    100000, EVP_sha256(),
                    32, key.data()
                );
                
                // Setup encryption
                EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
                if (!ctx) {
                    result.error = "Failed to create cipher context";
                    return result;
                }
                
                if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, key.data(), iv.data()) != 1) {
                    EVP_CIPHER_CTX_free(ctx);
                    result.error = "Failed to init encryption";
                    return result;
                }
                
                // Convert input
                std::vector<unsigned char> plain(plaintext.size());
                for (size_t i = 0; i < plaintext.size(); i++) {
                    plain[i] = static_cast<unsigned char>(plaintext[i]);
                }
                
                // Encrypt
                std::vector<unsigned char> cipher(plain.size() + 16);
                int len = 0, cipherLen = 0;
                
                if (EVP_EncryptUpdate(ctx, cipher.data(), &len, plain.data(), plain.size()) != 1) {
                    EVP_CIPHER_CTX_free(ctx);
                    result.error = "Encryption failed";
                    return result;
                }
                cipherLen = len;
                
                if (EVP_EncryptFinal_ex(ctx, cipher.data() + len, &len) != 1) {
                    EVP_CIPHER_CTX_free(ctx);
                    result.error = "Encryption finalize failed";
                    return result;
                }
                cipherLen += len;
                
                // Get auth tag
                std::vector<unsigned char> tag(16);
                EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag.data());
                EVP_CIPHER_CTX_free(ctx);
                
                // Store results
                result.data.resize(cipherLen);
                for (int i = 0; i < cipherLen; i++) {
                    result.data[i] = cipher[i];
                }
                
                result.salt.resize(16);
                result.iv.resize(12);
                result.tag.resize(16);
                
                for (int i = 0; i < 16; i++) result.salt[i] = salt[i];
                for (int i = 0; i < 12; i++) result.iv[i] = iv[i];
                for (int i = 0; i < 16; i++) result.tag[i] = tag[i];
                
                result.success = true;
                return result;
    }

    inline CryptoResult decrypt(CryptoResult encrypted, const std::string& password) {
        CryptoResult result;
                result.success = false;
                result.data = std::vector<int64_t>();
                result.iv = std::vector<int64_t>();
                result.tag = std::vector<int64_t>();
                result.salt = std::vector<int64_t>();
                
                if (!encrypted.success || encrypted.data.empty()) {
                    result.error = "Invalid encrypted data";
                    return result;
                }
                
                // Convert arrays
                std::vector<unsigned char> salt(encrypted.salt.size());
                std::vector<unsigned char> iv(encrypted.iv.size());
                std::vector<unsigned char> tag(encrypted.tag.size());
                std::vector<unsigned char> cipher(encrypted.data.size());
                
                for (size_t i = 0; i < encrypted.salt.size(); i++) salt[i] = encrypted.salt[i];
                for (size_t i = 0; i < encrypted.iv.size(); i++) iv[i] = encrypted.iv[i];
                for (size_t i = 0; i < encrypted.tag.size(); i++) tag[i] = encrypted.tag[i];
                for (size_t i = 0; i < encrypted.data.size(); i++) cipher[i] = encrypted.data[i];
                
                // Derive key
                std::vector<unsigned char> key(32);
                PKCS5_PBKDF2_HMAC(
                    password.c_str(), password.length(),
                    salt.data(), salt.size(),
                    100000, EVP_sha256(),
                    32, key.data()
                );
                
                // Setup decryption
                EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
                if (!ctx) {
                    result.error = "Failed to create cipher context";
                    return result;
                }
                
                if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, key.data(), iv.data()) != 1) {
                    EVP_CIPHER_CTX_free(ctx);
                    result.error = "Failed to init decryption";
                    return result;
                }
                
                // Decrypt
                std::vector<unsigned char> plain(cipher.size());
                int len = 0, plainLen = 0;
                
                if (EVP_DecryptUpdate(ctx, plain.data(), &len, cipher.data(), cipher.size()) != 1) {
                    EVP_CIPHER_CTX_free(ctx);
                    result.error = "Decryption failed";
                    return result;
                }
                plainLen = len;
                
                // Set expected tag
                EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, tag.data());
                
                // Verify and finalize
                if (EVP_DecryptFinal_ex(ctx, plain.data() + len, &len) != 1) {
                    EVP_CIPHER_CTX_free(ctx);
                    result.error = "Authentication failed - wrong password or corrupted data";
                    return result;
                }
                plainLen += len;
                EVP_CIPHER_CTX_free(ctx);
                
                // Store results
                result.data.resize(plainLen);
                for (int i = 0; i < plainLen; i++) {
                    result.data[i] = plain[i];
                }
                
                result.success = true;
                return result;
    }

    inline CryptoResult encryptString(const std::string& text, const std::string& password) {
        std::vector<int64_t> bytes(text.size());
                for (size_t i = 0; i < text.size(); i++) {
                    bytes[i] = static_cast<unsigned char>(text[i]);
                }
                return encrypt(bytes, password);
    }

    inline std::string decryptToString(CryptoResult encrypted, const std::string& password) {
        CryptoResult decrypted;
                decrypted.success = false;
                decrypted.data = std::vector<int64_t>();
                decrypted.iv = std::vector<int64_t>();
                decrypted.tag = std::vector<int64_t>();
                decrypted.salt = std::vector<int64_t>();
                
                if (!encrypted.success || encrypted.data.empty()) {
                    return "";
                }
                
                // Convert arrays
                std::vector<unsigned char> salt(encrypted.salt.size());
                std::vector<unsigned char> iv(encrypted.iv.size());
                std::vector<unsigned char> tag(encrypted.tag.size());
                std::vector<unsigned char> cipher(encrypted.data.size());
                
                for (size_t i = 0; i < encrypted.salt.size(); i++) salt[i] = encrypted.salt[i];
                for (size_t i = 0; i < encrypted.iv.size(); i++) iv[i] = encrypted.iv[i];
                for (size_t i = 0; i < encrypted.tag.size(); i++) tag[i] = encrypted.tag[i];
                for (size_t i = 0; i < encrypted.data.size(); i++) cipher[i] = encrypted.data[i];
                
                // Derive key
                std::vector<unsigned char> key(32);
                PKCS5_PBKDF2_HMAC(
                    password.c_str(), password.length(),
                    salt.data(), salt.size(),
                    100000, EVP_sha256(),
                    32, key.data()
                );
                
                // Setup decryption
                EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
                if (!ctx) return "";
                
                if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, key.data(), iv.data()) != 1) {
                    EVP_CIPHER_CTX_free(ctx);
                    return "";
                }
                
                // Decrypt
                std::vector<unsigned char> plain(cipher.size());
                int len = 0, plainLen = 0;
                
                if (EVP_DecryptUpdate(ctx, plain.data(), &len, cipher.data(), cipher.size()) != 1) {
                    EVP_CIPHER_CTX_free(ctx);
                    return "";
                }
                plainLen = len;
                
                EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, tag.data());
                
                if (EVP_DecryptFinal_ex(ctx, plain.data() + len, &len) != 1) {
                    EVP_CIPHER_CTX_free(ctx);
                    return "";
                }
                plainLen += len;
                EVP_CIPHER_CTX_free(ctx);
                
                std::string text;
                text.reserve(plainLen);
                for (int i = 0; i < plainLen; i++) {
                    text += static_cast<char>(plain[i]);
                }
                return text;
    }

    inline std::string hashPassword(const std::string& password) {
        // Generate salt
                std::vector<unsigned char> salt(16);
                RAND_bytes(salt.data(), 16);
                
                // Derive hash
                std::vector<unsigned char> hash(32);
                PKCS5_PBKDF2_HMAC(
                    password.c_str(), password.length(),
                    salt.data(), salt.size(),
                    100000, EVP_sha256(),
                    32, hash.data()
                );
                
                // Format: salt$hash (hex encoded)
                std::stringstream ss;
                for (auto b : salt) ss << std::hex << std::setfill('0') << std::setw(2) << (int)b;
                ss << "$";
                for (auto b : hash) ss << std::hex << std::setfill('0') << std::setw(2) << (int)b;
                return ss.str();
    }

    inline bool verifyPassword(const std::string& password, const std::string& hash) {
        // Parse hash format: salt$hash
                size_t delim = hash.find('$');
                if (delim == std::string::npos || delim != 32) return false;
                
                std::string saltHex = hash.substr(0, 32);
                std::string hashHex = hash.substr(33);
                
                // Decode salt
                std::vector<unsigned char> salt(16);
                for (int i = 0; i < 16; i++) {
                    salt[i] = std::stoi(saltHex.substr(i*2, 2), nullptr, 16);
                }
                
                // Derive hash
                std::vector<unsigned char> computed(32);
                PKCS5_PBKDF2_HMAC(
                    password.c_str(), password.length(),
                    salt.data(), salt.size(),
                    100000, EVP_sha256(),
                    32, computed.data()
                );
                
                // Compare (constant time)
                std::stringstream ss;
                for (auto b : computed) ss << std::hex << std::setfill('0') << std::setw(2) << (int)b;
                
                return CRYPTO_memcmp(ss.str().c_str(), hashHex.c_str(), 64) == 0;
    }

} // namespace Crypto

// =======================================================================
// Std.Map (Auto-generated from stdlib)
// =======================================================================
namespace Map {

    inline int64_t sizeStrStr(const std::unordered_map<std::string, std::string>& map) {
        return static_cast<int64_t>(map.size());
    }

    inline bool isEmptyStrStr(const std::unordered_map<std::string, std::string>& map) {
        return map.empty();
    }

    inline void clearStrStr(const std::unordered_map<std::string, std::string>& map) {
        map.clear();
    }

    inline std::optional<std::string> getStrStr(const std::unordered_map<std::string, std::string>& map, const std::string& key) {
        auto it = map.find(key);
                if (it != map.end()) {
                    return std::make_optional(it->second);
                }
                return std::nullopt;
    }

    inline std::string getOrStrStr(const std::unordered_map<std::string, std::string>& map, const std::string& key, const std::string& defaultVal) {
        auto it = map.find(key);
                if (it != map.end()) {
                    return it->second;
                }
                return defaultVal;
    }

    inline bool containsStrStr(const std::unordered_map<std::string, std::string>& map, const std::string& key) {
        return map.find(key) != map.end();
    }

    inline void insertStrStr(const std::unordered_map<std::string, std::string>& map, const std::string& key, const std::string& value) {
        map[key] = value;
    }

    inline void setStrStr(const std::unordered_map<std::string, std::string>& map, const std::string& key, const std::string& value) {
        map[key] = value;
    }

    inline bool removeStrStr(const std::unordered_map<std::string, std::string>& map, const std::string& key) {
        auto it = map.find(key);
                if (it != map.end()) {
                    map.erase(it);
                    return true;
                }
                return false;
    }

    inline std::vector<std::string> keysStrStr(const std::unordered_map<std::string, std::string>& map) {
        std::vector<std::string> result;
                result.reserve(map.size());
                for (const auto& pair : map) {
                    result.push_back(pair.first);
                }
                return result;
    }

    inline std::vector<std::string> valuesStrStr(const std::unordered_map<std::string, std::string>& map) {
        std::vector<std::string> result;
                result.reserve(map.size());
                for (const auto& pair : map) {
                    result.push_back(pair.second);
                }
                return result;
    }

    inline int64_t sizeStrInt(const std::unordered_map<std::string, int64_t>& map) {
        return static_cast<int64_t>(map.size());
    }

    inline bool isEmptyStrInt(const std::unordered_map<std::string, int64_t>& map) {
        return map.empty();
    }

    inline void clearStrInt(const std::unordered_map<std::string, int64_t>& map) {
        map.clear();
    }

    inline std::optional<int64_t> getStrInt(const std::unordered_map<std::string, int64_t>& map, const std::string& key) {
        auto it = map.find(key);
                if (it != map.end()) {
                    return std::make_optional(it->second);
                }
                return std::nullopt;
    }

    inline int64_t getOrStrInt(const std::unordered_map<std::string, int64_t>& map, const std::string& key, int64_t defaultVal) {
        auto it = map.find(key);
                if (it != map.end()) {
                    return it->second;
                }
                return defaultVal;
    }

    inline bool containsStrInt(const std::unordered_map<std::string, int64_t>& map, const std::string& key) {
        return map.find(key) != map.end();
    }

    inline void insertStrInt(const std::unordered_map<std::string, int64_t>& map, const std::string& key, int64_t value) {
        map[key] = value;
    }

    inline void setStrInt(const std::unordered_map<std::string, int64_t>& map, const std::string& key, int64_t value) {
        map[key] = value;
    }

    inline bool removeStrInt(const std::unordered_map<std::string, int64_t>& map, const std::string& key) {
        auto it = map.find(key);
                if (it != map.end()) {
                    map.erase(it);
                    return true;
                }
                return false;
    }

    inline std::vector<std::string> keysStrInt(const std::unordered_map<std::string, int64_t>& map) {
        std::vector<std::string> result;
                result.reserve(map.size());
                for (const auto& pair : map) {
                    result.push_back(pair.first);
                }
                return result;
    }

    inline std::vector<int64_t> valuesStrInt(const std::unordered_map<std::string, int64_t>& map) {
        std::vector<int64_t> result;
                result.reserve(map.size());
                for (const auto& pair : map) {
                    result.push_back(pair.second);
                }
                return result;
    }

    inline void incrementStrInt(const std::unordered_map<std::string, int64_t>& map, const std::string& key) {
        map[key]++;
    }

    inline void decrementStrInt(const std::unordered_map<std::string, int64_t>& map, const std::string& key) {
        map[key]--;
    }

    inline int64_t sizeIntInt(const std::unordered_map<int64_t, int64_t>& map) {
        return static_cast<int64_t>(map.size());
    }

    inline bool isEmptyIntInt(const std::unordered_map<int64_t, int64_t>& map) {
        return map.empty();
    }

    inline std::optional<int64_t> getIntInt(const std::unordered_map<int64_t, int64_t>& map, int64_t key) {
        auto it = map.find(key);
                if (it != map.end()) {
                    return std::make_optional(it->second);
                }
                return std::nullopt;
    }

    inline int64_t getOrIntInt(const std::unordered_map<int64_t, int64_t>& map, int64_t key, int64_t defaultVal) {
        auto it = map.find(key);
                if (it != map.end()) {
                    return it->second;
                }
                return defaultVal;
    }

    inline bool containsIntInt(const std::unordered_map<int64_t, int64_t>& map, int64_t key) {
        return map.find(key) != map.end();
    }

    inline void insertIntInt(const std::unordered_map<int64_t, int64_t>& map, int64_t key, int64_t value) {
        map[key] = value;
    }

    inline bool removeIntInt(const std::unordered_map<int64_t, int64_t>& map, int64_t key) {
        auto it = map.find(key);
                if (it != map.end()) {
                    map.erase(it);
                    return true;
                }
                return false;
    }

    inline std::vector<int64_t> keysIntInt(const std::unordered_map<int64_t, int64_t>& map) {
        std::vector<int64_t> result;
                result.reserve(map.size());
                for (const auto& pair : map) {
                    result.push_back(pair.first);
                }
                return result;
    }

    inline std::vector<int64_t> valuesIntInt(const std::unordered_map<int64_t, int64_t>& map) {
        std::vector<int64_t> result;
                result.reserve(map.size());
                for (const auto& pair : map) {
                    result.push_back(pair.second);
                }
                return result;
    }

    inline int64_t sizeIntStr(const std::unordered_map<int64_t, std::string>& map) {
        return static_cast<int64_t>(map.size());
    }

    inline std::optional<std::string> getIntStr(const std::unordered_map<int64_t, std::string>& map, int64_t key) {
        auto it = map.find(key);
                if (it != map.end()) {
                    return std::make_optional(it->second);
                }
                return std::nullopt;
    }

    inline std::string getOrIntStr(const std::unordered_map<int64_t, std::string>& map, int64_t key, const std::string& defaultVal) {
        auto it = map.find(key);
                if (it != map.end()) {
                    return it->second;
                }
                return defaultVal;
    }

    inline bool containsIntStr(const std::unordered_map<int64_t, std::string>& map, int64_t key) {
        return map.find(key) != map.end();
    }

    inline void insertIntStr(const std::unordered_map<int64_t, std::string>& map, int64_t key, const std::string& value) {
        map[key] = value;
    }

    inline bool removeIntStr(const std::unordered_map<int64_t, std::string>& map, int64_t key) {
        auto it = map.find(key);
                if (it != map.end()) {
                    map.erase(it);
                    return true;
                }
                return false;
    }

} // namespace Map

// =======================================================================
// Std.Array (Auto-generated from stdlib)
// =======================================================================
namespace Array {

    inline int64_t lengthInt(const std::vector<int64_t>& arr) {
        return static_cast<int64_t>(arr.size());
    }

    inline int64_t lengthStr(const std::vector<std::string>& arr) {
        return static_cast<int64_t>(arr.size());
    }

    inline bool isEmptyInt(const std::vector<int64_t>& arr) {
        return arr.empty();
    }

    inline bool isEmptyStr(const std::vector<std::string>& arr) {
        return arr.empty();
    }

    inline void pushInt(const std::vector<int64_t>& arr, int64_t value) {
        arr.push_back(value);
    }

    inline void pushStr(const std::vector<std::string>& arr, const std::string& value) {
        arr.push_back(value);
    }

    inline int64_t popInt(const std::vector<int64_t>& arr) {
        auto v = arr.back();
                arr.pop_back();
                return v;
    }

    inline std::string popStr(const std::vector<std::string>& arr) {
        auto v = arr.back();
                arr.pop_back();
                return v;
    }

    inline std::optional<int64_t> firstInt(const std::vector<int64_t>& arr) {
        if (arr.empty()) return std::nullopt;
                return arr.front();
    }

    inline std::optional<std::string> firstStr(const std::vector<std::string>& arr) {
        if (arr.empty()) return std::nullopt;
                return arr.front();
    }

    inline std::optional<int64_t> lastInt(const std::vector<int64_t>& arr) {
        if (arr.empty()) return std::nullopt;
                return arr.back();
    }

    inline std::optional<std::string> lastStr(const std::vector<std::string>& arr) {
        if (arr.empty()) return std::nullopt;
                return arr.back();
    }

    inline std::optional<int64_t> getInt(const std::vector<int64_t>& arr, int64_t index) {
        if (index < 0 || static_cast<size_t>(index) >= arr.size()) return std::nullopt;
                return arr[static_cast<size_t>(index)];
    }

    inline std::optional<std::string> getStr(const std::vector<std::string>& arr, int64_t index) {
        if (index < 0 || static_cast<size_t>(index) >= arr.size()) return std::nullopt;
                return arr[static_cast<size_t>(index)];
    }

    inline bool setInt(const std::vector<int64_t>& arr, int64_t index, int64_t value) {
        if (index < 0 || static_cast<size_t>(index) >= arr.size()) return false;
                arr[static_cast<size_t>(index)] = value;
                return true;
    }

    inline bool setStr(const std::vector<std::string>& arr, int64_t index, const std::string& value) {
        if (index < 0 || static_cast<size_t>(index) >= arr.size()) return false;
                arr[static_cast<size_t>(index)] = value;
                return true;
    }

    inline std::vector<int64_t> sliceInt(const std::vector<int64_t>& arr, int64_t start, int64_t end) {
        std::vector<int64_t> result;
                int64_t s = start < 0 ? 0 : start;
                int64_t e = end > static_cast<int64_t>(arr.size()) ? arr.size() : end;
                for (int64_t i = s; i < e; i++) {
                    result.push_back(arr[i]);
                }
                return result;
    }

    inline std::vector<std::string> sliceStr(const std::vector<std::string>& arr, int64_t start, int64_t end) {
        std::vector<std::string> result;
                int64_t s = start < 0 ? 0 : start;
                int64_t e = end > static_cast<int64_t>(arr.size()) ? arr.size() : end;
                for (int64_t i = s; i < e; i++) {
                    result.push_back(arr[i]);
                }
                return result;
    }

    inline std::vector<int64_t> concatInt(const std::vector<int64_t>& a, const std::vector<int64_t>& b) {
        std::vector<int64_t> result = a;
                result.insert(result.end(), b.begin(), b.end());
                return result;
    }

    inline std::vector<std::string> concatStr(const std::vector<std::string>& a, const std::vector<std::string>& b) {
        std::vector<std::string> result = a;
                result.insert(result.end(), b.begin(), b.end());
                return result;
    }

    inline std::vector<int64_t> reverseInt(const std::vector<int64_t>& arr) {
        std::vector<int64_t> result = arr;
                std::reverse(result.begin(), result.end());
                return result;
    }

    inline std::vector<std::string> reverseStr(const std::vector<std::string>& arr) {
        std::vector<std::string> result = arr;
                std::reverse(result.begin(), result.end());
                return result;
    }

    inline bool containsInt(const std::vector<int64_t>& arr, int64_t value) {
        return std::find(arr.begin(), arr.end(), value) != arr.end();
    }

    inline bool containsStr(const std::vector<std::string>& arr, const std::string& value) {
        return std::find(arr.begin(), arr.end(), value) != arr.end();
    }

    inline std::optional<int64_t> indexOfInt(const std::vector<int64_t>& arr, int64_t value) {
        auto it = std::find(arr.begin(), arr.end(), value);
                if (it == arr.end()) return std::nullopt;
                return static_cast<int64_t>(std::distance(arr.begin(), it));
    }

    inline std::optional<int64_t> indexOfStr(const std::vector<std::string>& arr, const std::string& value) {
        auto it = std::find(arr.begin(), arr.end(), value);
                if (it == arr.end()) return std::nullopt;
                return static_cast<int64_t>(std::distance(arr.begin(), it));
    }

    inline void clearInt(const std::vector<int64_t>& arr) {
        arr.clear();
    }

    inline void clearStr(const std::vector<std::string>& arr) {
        arr.clear();
    }

    inline std::vector<int64_t> filled(int64_t size, int64_t value) {
        return std::vector<int64_t>(static_cast<size_t>(size), static_cast<int64_t>(value));
    }

    inline std::vector<int64_t> zeros(int64_t size) {
        return std::vector<int64_t>(static_cast<size_t>(size), 0);
    }

    inline std::vector<int64_t> ones(int64_t size) {
        return std::vector<int64_t>(static_cast<size_t>(size), 1);
    }

    inline std::vector<int64_t> range(int64_t start, int64_t end) {
        std::vector<int64_t> result;
                for (int64_t i = start; i < end; i++) {
                    result.push_back(i);
                }
                return result;
    }

    inline int64_t sum(const std::vector<int64_t>& arr) {
        int64_t total = 0;
                for (auto v : arr) total += v;
                return total;
    }

    inline std::optional<int64_t> min(const std::vector<int64_t>& arr) {
        if (arr.empty()) return std::nullopt;
                return *std::min_element(arr.begin(), arr.end());
    }

    inline std::optional<int64_t> max(const std::vector<int64_t>& arr) {
        if (arr.empty()) return std::nullopt;
                return *std::max_element(arr.begin(), arr.end());
    }

    inline std::vector<int64_t> sort(const std::vector<int64_t>& arr) {
        std::vector<int64_t> result = arr;
                std::sort(result.begin(), result.end());
                return result;
    }

    inline std::vector<int64_t> sortDesc(const std::vector<int64_t>& arr) {
        std::vector<int64_t> result = arr;
                std::sort(result.begin(), result.end(), std::greater<int64_t>());
                return result;
    }

    inline std::vector<int64_t> unique(const std::vector<int64_t>& arr) {
        std::vector<int64_t> result = arr;
                std::sort(result.begin(), result.end());
                result.erase(std::unique(result.begin(), result.end()), result.end());
                return result;
    }

    inline std::vector<std::string> sortStr(const std::vector<std::string>& arr) {
        std::vector<std::string> result = arr;
                std::sort(result.begin(), result.end());
                return result;
    }

    inline std::vector<std::string> uniqueStr(const std::vector<std::string>& arr) {
        std::vector<std::string> result = arr;
                std::sort(result.begin(), result.end());
                result.erase(std::unique(result.begin(), result.end()), result.end());
                return result;
    }

} // namespace Array

// =======================================================================
// Std.String (Auto-generated from stdlib)
// =======================================================================
namespace String {

    inline int64_t length(const std::string& s) {
        return static_cast<int64_t>(s.length());
    }

    inline bool isEmpty(const std::string& s) {
        return s.empty();
    }

    inline std::optional<std::string> charAt(const std::string& s, int64_t index) {
        if (index < 0 || static_cast<size_t>(index) >= s.length()) return std::nullopt;
                return std::string(1, s[static_cast<size_t>(index)]);
    }

    inline std::string substring(const std::string& s, int64_t start, int64_t end) {
        int64_t st = start;
                int64_t en = end;
                if (st < 0) st = 0;
                if (en > static_cast<int64_t>(s.length())) en = s.length();
                if (st >= en) return "";
                return s.substr(static_cast<size_t>(st), static_cast<size_t>(en - st));
    }

    inline std::string slice(const std::string& s, int64_t start, int64_t end) {
        int64_t st = start;
                int64_t en = end;
                if (st < 0) st = 0;
                if (en > static_cast<int64_t>(s.length())) en = s.length();
                if (st >= en) return "";
                return s.substr(static_cast<size_t>(st), static_cast<size_t>(en - st));
    }

    inline std::string toUpper(const std::string& s) {
        std::string result = s;
                std::transform(result.begin(), result.end(), result.begin(), ::toupper);
                return result;
    }

    inline std::string toLower(const std::string& s) {
        std::string result = s;
                std::transform(result.begin(), result.end(), result.begin(), ::tolower);
                return result;
    }

    inline std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\n\r");
                if (start == std::string::npos) return "";
                size_t end = s.find_last_not_of(" \t\n\r");
                return s.substr(start, end - start + 1);
    }

    inline std::string trimStart(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\n\r");
                if (start == std::string::npos) return "";
                return s.substr(start);
    }

    inline std::string trimEnd(const std::string& s) {
        size_t end = s.find_last_not_of(" \t\n\r");
                if (end == std::string::npos) return "";
                return s.substr(0, end + 1);
    }

    inline bool startsWith(const std::string& s, const std::string& prefix) {
        if (prefix.length() > s.length()) return false;
                return s.compare(0, prefix.length(), prefix) == 0;
    }

    inline bool endsWith(const std::string& s, const std::string& suffix) {
        if (suffix.length() > s.length()) return false;
                return s.compare(s.length() - suffix.length(), suffix.length(), suffix) == 0;
    }

    inline bool contains(const std::string& s, const std::string& substr) {
        return s.find(substr) != std::string::npos;
    }

    inline std::optional<int64_t> indexOf(const std::string& s, const std::string& substr) {
        size_t pos = s.find(substr);
                if (pos == std::string::npos) return std::nullopt;
                return static_cast<int64_t>(pos);
    }

    inline std::optional<int64_t> lastIndexOf(const std::string& s, const std::string& substr) {
        size_t pos = s.rfind(substr);
                if (pos == std::string::npos) return std::nullopt;
                return static_cast<int64_t>(pos);
    }

    inline std::string replace(const std::string& s, const std::string& from, const std::string& to) {
        std::string result = s;
                size_t pos = 0;
                while ((pos = result.find(from, pos)) != std::string::npos) {
                    result.replace(pos, from.length(), to);
                    pos += to.length();
                }
                return result;
    }

    inline std::string replaceFirst(const std::string& s, const std::string& from, const std::string& to) {
        std::string result = s;
                size_t pos = result.find(from);
                if (pos != std::string::npos) {
                    result.replace(pos, from.length(), to);
                }
                return result;
    }

    inline std::string remove(const std::string& s, const std::string& substr) {
        std::string result = s;
                size_t pos = 0;
                while ((pos = result.find(substr, pos)) != std::string::npos) {
                    result.erase(pos, substr.length());
                }
                return result;
    }

    inline std::vector<std::string> split(const std::string& s, const std::string& delim) {
        std::vector<std::string> result;
                if (delim.empty()) {
                    result.push_back(s);
                    return result;
                }
                size_t start = 0;
                size_t end;
                while ((end = s.find(delim, start)) != std::string::npos) {
                    result.push_back(s.substr(start, end - start));
                    start = end + delim.length();
                }
                result.push_back(s.substr(start));
                return result;
    }

    inline std::vector<std::string> splitChar(const std::string& s, const std::string& delim) {
        std::vector<std::string> result;
                if (delim.empty()) {
                    result.push_back(s);
                    return result;
                }
                char d = delim[0];
                size_t start = 0;
                for (size_t i = 0; i < s.length(); i++) {
                    if (s[i] == d) {
                        result.push_back(s.substr(start, i - start));
                        start = i + 1;
                    }
                }
                result.push_back(s.substr(start));
                return result;
    }

    inline std::vector<std::string> splitLines(const std::string& s) {
        std::vector<std::string> result;
                std::istringstream stream(s);
                std::string line;
                while (std::getline(stream, line)) {
                    result.push_back(line);
                }
                return result;
    }

    inline std::vector<std::string> splitWhitespace(const std::string& s) {
        std::vector<std::string> result;
                std::istringstream stream(s);
                std::string word;
                while (stream >> word) {
                    result.push_back(word);
                }
                return result;
    }

    inline std::string join(const std::vector<std::string>& parts, const std::string& sep) {
        std::string result;
                for (size_t i = 0; i < parts.size(); i++) {
                    if (i > 0) result += sep;
                    result += parts[i];
                }
                return result;
    }

    inline std::string repeat(const std::string& s, int64_t count) {
        std::string result;
                result.reserve(s.length() * count);
                for (int64_t i = 0; i < count; i++) {
                    result += s;
                }
                return result;
    }

    inline std::string reverse(const std::string& s) {
        std::string result = s;
                std::reverse(result.begin(), result.end());
                return result;
    }

    inline std::string padStart(const std::string& s, int64_t length, const std::string& pad) {
        if (static_cast<int64_t>(s.length()) >= length || pad.empty()) return s;
                std::string result;
                int64_t needed = length - s.length();
                while (static_cast<int64_t>(result.length()) < needed) {
                    result += pad;
                }
                return result.substr(0, needed) + s;
    }

    inline std::string padEnd(const std::string& s, int64_t length, const std::string& pad) {
        if (static_cast<int64_t>(s.length()) >= length || pad.empty()) return s;
                std::string result = s;
                while (static_cast<int64_t>(result.length()) < length) {
                    result += pad;
                }
                return result.substr(0, length);
    }

    inline std::string capitalize(const std::string& s) {
        if (s.empty()) return s;
                std::string result = s;
                result[0] = std::toupper(result[0]);
                return result;
    }

    inline std::string titleCase(const std::string& s) {
        std::string result = s;
                bool newWord = true;
                for (size_t i = 0; i < result.length(); i++) {
                    if (std::isspace(result[i])) {
                        newWord = true;
                    } else if (newWord) {
                        result[i] = std::toupper(result[i]);
                        newWord = false;
                    }
                }
                return result;
    }

    inline std::optional<int64_t> parseInt(const std::string& s) {
        try {
                    size_t pos;
                    int64_t val = std::stoll(s, &pos);
                    if (pos != s.length()) return std::nullopt;
                    return val;
                } catch (...) {
                    return std::nullopt;
                }
    }

    inline std::optional<double> parseFloat(const std::string& s) {
        try {
                    size_t pos;
                    double val = std::stod(s, &pos);
                    if (pos != s.length()) return std::nullopt;
                    return val;
                } catch (...) {
                    return std::nullopt;
                }
    }

    inline std::optional<bool> parseBool(const std::string& s) {
        std::string lower = s;
                std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
                if (lower == "true" || lower == "1" || lower == "yes") return true;
                if (lower == "false" || lower == "0" || lower == "no") return false;
                return std::nullopt;
    }

    inline std::string formatTemplate(const std::string& tmpl, const std::unordered_map<std::string, std::string>& args) {
        std::string result = tmpl;
                for (const auto& pair : args) {
                    std::string placeholder = "{" + pair.first + "}";
                    size_t pos = 0;
                    while ((pos = result.find(placeholder, pos)) != std::string::npos) {
                        result.replace(pos, placeholder.length(), pair.second);
                        pos += pair.second.length();
                    }
                }
                return result;
    }

    inline std::string escapeHtml(const std::string& s) {
        std::string result;
                result.reserve(s.length() * 2);
                for (char c : s) {
                    switch (c) {
                        case '&': result += "&amp;"; break;
                        case '<': result += "&lt;"; break;
                        case '>': result += "&gt;"; break;
                        case '"': result += "&quot;"; break;
                        case '\'': result += "&#39;"; break;
                        default: result += c; break;
                    }
                }
                return result;
            }
        }
        
        pub fn escapeJson(s: string) -> string {
            @cpp {
                std::string result;
                result.reserve(s.length() * 2);
                for (char c : s) {
                    switch (c) {
                        case '\\': result += "\\\\"; break;
                        case '"': result += "\\\""; break;
                        case '\n': result += "\\n"; break;
                        case '\r': result += "\\r"; break;
                        case '\t': result += "\\t"; break;
                        default: result += c; break;
                    }
                }
                return result;
    }

    inline std::string escapeJson(const std::string& s) {
        std::string result;
                result.reserve(s.length() * 2);
                for (char c : s) {
                    switch (c) {
                        case '\\': result += "\\\\"; break;
                        case '"': result += "\\\""; break;
                        case '\n': result += "\\n"; break;
                        case '\r': result += "\\r"; break;
                        case '\t': result += "\\t"; break;
                        default: result += c; break;
                    }
                }
                return result;
    }

    inline std::vector<std::string> toCharArray(const std::string& s) {
        std::vector<std::string> result;
                for (char c : s) {
                    result.push_back(std::string(1, c));
                }
                return result;
    }

    inline std::string fromCharCodes(const std::vector<int64_t>& codes) {
        std::string result;
                for (auto code : codes) {
                    result += static_cast<char>(code);
                }
                return result;
    }

} // namespace String

// =======================================================================
// Std.File (Auto-generated from stdlib)
// =======================================================================
namespace File {

    inline bool exists(const std::string& path) {
        return std::filesystem::exists(path);
    }

    inline bool isFile(const std::string& path) {
        return std::filesystem::is_regular_file(path);
    }

    inline bool isDirectory(const std::string& path) {
        return std::filesystem::is_directory(path);
    }

    inline int64_t size(const std::string& path) {
        if (!std::filesystem::exists(path)) return -1;
                return static_cast<int64_t>(std::filesystem::file_size(path));
    }

    inline std::optional<std::string> read(const std::string& path) {
        std::ifstream file(path);
                if (!file) return std::nullopt;
                std::stringstream buffer;
                buffer << file.rdbuf();
                return buffer.str();
    }

    inline bool write(const std::string& path, const std::string& content) {
        std::ofstream file(path);
                if (!file) return false;
                file << content;
                return true;
    }

    inline bool append(const std::string& path, const std::string& content) {
        std::ofstream file(path, std::ios::app);
                if (!file) return false;
                file << content;
                return true;
    }

    inline bool remove(const std::string& path) {
        return std::filesystem::remove(path);
    }

    inline bool createDir(const std::string& path) {
        return std::filesystem::create_directories(path);
    }

    inline bool copy(const std::string& src, const std::string& dst) {
        try {
                    std::filesystem::copy(src, dst, std::filesystem::copy_options::overwrite_existing);
                    return true;
                } catch (...) {
                    return false;
                }
    }

    inline bool move(const std::string& src, const std::string& dst) {
        try {
                    std::filesystem::rename(src, dst);
                    return true;
                } catch (...) {
                    return false;
                }
    }

    inline std::string absolutePath(const std::string& path) {
        return std::filesystem::absolute(path).string();
    }

    inline std::string parentDir(const std::string& path) {
        return std::filesystem::path(path).parent_path().string();
    }

    inline std::string fileName(const std::string& path) {
        return std::filesystem::path(path).filename().string();
    }

    inline std::string extension(const std::string& path) {
        return std::filesystem::path(path).extension().string();
    }

    inline std::string tempDir() {
        return std::filesystem::temp_directory_path().string();
    }

    inline std::optional<std::string> createTempFile(const std::string& prefix) {
        std::string dir = std::filesystem::temp_directory_path().string();
                std::string path = dir + "/" + prefix + "_XXXXXX";
                std::vector<char> buf(path.begin(), path.end());
                buf.push_back('\0');
                int fd = mkstemp(buf.data());
                if (fd == -1) return std::nullopt;
                close(fd);
                return std::string(buf.data());
    }

    inline std::string cwd() {
        return std::filesystem::current_path().string();
    }

    inline bool setCwd(const std::string& path) {
        try {
                    std::filesystem::current_path(path);
                    return true;
                } catch (...) {
                    return false;
                }
    }

    inline std::vector<std::string> listDir(const std::string& path) {
        std::vector<std::string> entries;
                if (!std::filesystem::is_directory(path)) return entries;
                for (const auto& entry : std::filesystem::directory_iterator(path)) {
                    entries.push_back(entry.path().filename().string());
                }
                return entries;
    }

    inline std::vector<int64_t> readBytes(const std::string& path) {
        std::vector<int64_t> result;
                std::ifstream file(path, std::ios::binary);
                if (!file) return result;
                char byte;
                while (file.get(byte)) {
                    result.push_back(static_cast<unsigned char>(byte));
                }
                return result;
    }

    inline bool writeBytes(const std::string& path, const std::vector<int64_t>& data) {
        std::ofstream file(path, std::ios::binary);
                if (!file) return false;
                for (auto b : data) {
                    file.put(static_cast<char>(b));
                }
                return true;
    }

} // namespace File

// =======================================================================
// Standard Library Helpers (generated once)
// =======================================================================
#ifndef MAGOLOR_STDLIB_HELPERS_H
#define MAGOLOR_STDLIB_HELPERS_H

// Template helpers for string conversion
template<typename T>
inline std::string mg_to_string(const T& val) { 
    std::ostringstream oss; 
    oss << val; 
    return oss.str(); 
}

template<>
inline std::string mg_to_string(const bool& val) {
    return val ? "true" : "false";
}

template<>
inline std::string mg_to_string(const std::string& val) {
    return val;
}

// Global Option helpers
template<typename T>
inline bool isSome(const std::optional<T>& opt) { return opt.has_value(); }

template<typename T>
inline bool isNone(const std::optional<T>& opt) { return !opt.has_value(); }

template<typename T>
inline T unwrap(const std::optional<T>& opt) {
    if (!opt.has_value()) {
        throw std::runtime_error("Called unwrap on None value");
    }
    return opt.value();
}

template<typename T>
inline T unwrapOr(const std::optional<T>& opt, const T& defaultValue) {
    return opt.value_or(defaultValue);
}

#endif // MAGOLOR_STDLIB_HELPERS_H

// Global I/O functions
inline void print(const std::string& s) { std::cout << s; }
inline void println(const std::string& s) { std::cout << s << std::endl; }
inline void println() { std::cout << std::endl; }
inline void eprint(const std::string& s) { std::cerr << s; }
inline void eprintln(const std::string& s) { std::cerr << s << std::endl; }
inline std::string readLine() { std::string line; std::getline(std::cin, line); return line; }

// Overloads for common types
template<typename T>
inline void print(const T& val) { std::cout << val; }
template<typename T>
inline void println(const T& val) { std::cout << val << std::endl; }

class ValueType;
class SlateSchema;
class SlateValue;
class SlateObject;
class SlateConfig;
class BinaryWriter;
class BinaryReader;
class Schema;

class ValueType {
public:
    static constexpr int64_t VALUE_NULL = 0;
    static constexpr int64_t INT = 1;
    static constexpr int64_t FLOAT = 2;
    static constexpr int64_t STRING = 3;
    static constexpr int64_t BOOL = 4;
    static constexpr int64_t OBJECT_REF = 5;
    static constexpr int64_t ARRAY = 6;
};

// Auto-generated print support
inline std::ostream& operator<<(std::ostream& os, const ValueType& obj) {
    os << "ValueType { ";
    os << " }";
    return os;
}

class SlateSchema {
public:
    std::string className;
    int64_t classId;
    std::vector<std::string> fieldNames;
    std::vector<int64_t> fieldTypes;
    void create() {
        this->className = std::string("");
        this->classId = 0;
        // Inline C++ code:

            this->fieldNames = std::vector<std::string>();
            this->fieldTypes = std::vector<int>();
        
    }
    void addField(std::string name, int64_t typeCode) {
        // Inline C++ code:

            this->fieldNames.push_back(name);
            this->fieldTypes.push_back(typeCode);
        
    }
};

// Auto-generated print support
inline std::ostream& operator<<(std::ostream& os, const SlateSchema& obj) {
    os << "SlateSchema { ";
    os << "className: " << obj.className;
    os << ", ";
    os << "classId: " << obj.classId;
    os << " }";
    return os;
}

class SlateValue {
public:
    int64_t valueType;
    int64_t intValue;
    double floatValue;
    std::string stringValue;
    bool boolValue;
    int64_t objectId;
    std::vector<SlateValue> arrayValue;
    void create() {
        this->valueType = 0;
        this->intValue = 0;
        this->floatValue = 0.000000;
        this->stringValue = std::string("");
        this->boolValue = false;
        this->objectId = (-1);
        // Inline C++ code:

            this->arrayValue = std::vector<SlateValue>();
        
    }
    static SlateValue makeNull() {
        auto v = SlateValue();
        v.valueType = 0;
        return v;
    }
    static SlateValue makeInt(int64_t value) {
        auto v = SlateValue();
        v.valueType = 1;
        v.intValue = value;
        return v;
    }
    static SlateValue makeFloat(double value) {
        auto v = SlateValue();
        v.valueType = 2;
        v.floatValue = value;
        return v;
    }
    static SlateValue makeString(std::string value) {
        auto v = SlateValue();
        v.valueType = 3;
        v.stringValue = value;
        return v;
    }
    static SlateValue makeBool(bool value) {
        auto v = SlateValue();
        v.valueType = 4;
        v.boolValue = value;
        return v;
    }
    static SlateValue makeRef(int64_t id) {
        auto v = SlateValue();
        v.valueType = 5;
        v.objectId = id;
        return v;
    }
    static SlateValue makeArray() {
        auto v = SlateValue();
        v.valueType = 6;
        // Inline C++ code:

            v.arrayValue = std::vector<SlateValue>();
        
        return v;
    }
    void push(SlateValue val) {
        // Inline C++ code:

            this->arrayValue.push_back(val);
        
    }
};

// Auto-generated print support
inline std::ostream& operator<<(std::ostream& os, const SlateValue& obj) {
    os << "SlateValue { ";
    os << "valueType: " << obj.valueType;
    os << ", ";
    os << "intValue: " << obj.intValue;
    os << ", ";
    os << "floatValue: " << obj.floatValue;
    os << ", ";
    os << "stringValue: " << obj.stringValue;
    os << ", ";
    os << "boolValue: " << obj.boolValue;
    os << ", ";
    os << "objectId: " << obj.objectId;
    os << " }";
    return os;
}

class SlateObject {
public:
    std::string className;
    int64_t objectId;
    std::unordered_map<std::string, SlateValue> fields;
    void create() {
        this->className = std::string("");
        this->objectId = 0;
        // Inline C++ code:

            this->fields = std::unordered_map<std::string, SlateValue>();
        
    }
    void setField(std::string name, SlateValue value) {
        this->fields[name] = value;
    }
    std::optional<SlateValue> getField(std::string name) {
        // Inline C++ code:

            auto it = this->fields.find(name);
            if (it != this->fields.end()) {
                return std::make_optional(it->second);
            }
            return std::nullopt;
        
    }
    int64_t getInt(std::string name) {
        auto opt = this->getField(name);
        if (isSome(opt)) {
            return unwrap(opt).intValue;
        }
        return 0;
    }
    std::string getString(std::string name) {
        auto opt = this->getField(name);
        if (isSome(opt)) {
            return unwrap(opt).stringValue;
        }
        return std::string("");
    }
    bool getBool(std::string name) {
        auto opt = this->getField(name);
        if (isSome(opt)) {
            return unwrap(opt).boolValue;
        }
        return false;
    }
};

// Auto-generated print support
inline std::ostream& operator<<(std::ostream& os, const SlateObject& obj) {
    os << "SlateObject { ";
    os << "className: " << obj.className;
    os << ", ";
    os << "objectId: " << obj.objectId;
    os << " }";
    return os;
}

class SlateConfig {
public:
    bool encrypted;
    std::string password;
    bool autoSave;
    bool compressData;
    void create() {
        this->encrypted = false;
        this->password = std::string("");
        this->autoSave = true;
        this->compressData = false;
    }
    SlateConfig openConfig() {
        return SlateConfig();
    }
    SlateConfig openEncryptedConfig(std::string password) {
        auto cfg = SlateConfig();
        cfg.encrypted = true;
        cfg.password = password;
        return cfg;
    }
    SlateConfig setAutoSave(bool enabled) {
        this->autoSave = enabled;
        return *this;
    }
};

// Auto-generated print support
inline std::ostream& operator<<(std::ostream& os, const SlateConfig& obj) {
    os << "SlateConfig { ";
    os << "encrypted: " << obj.encrypted;
    os << ", ";
    os << "password: " << obj.password;
    os << ", ";
    os << "autoSave: " << obj.autoSave;
    os << ", ";
    os << "compressData: " << obj.compressData;
    os << " }";
    return os;
}

class BinaryWriter {
public:
    std::vector<int64_t> data;
    void create() {
        // Inline C++ code:

            this->data = std::vector<int>();
        
    }
    void writeU8(int64_t val) {
        // Inline C++ code:
 this->data.push_back(val & 0xFF); 
    }
    void writeU32(int64_t val) {
        // Inline C++ code:

            this->writeU8(val & 0xFF);
            this->writeU8((val >> 8) & 0xFF);
            this->writeU8((val >> 16) & 0xFF);
            this->writeU8((val >> 24) & 0xFF);
        
    }
    void writeI64(int64_t val) {
        // Inline C++ code:

            for (int i = 0; i < 8; i++) {
                this->data.push_back((val >> (i * 8)) & 0xFF);
            }
        
    }
    void writeF64(double val) {
        // Inline C++ code:

            double d = val;
            uint64_t bits;
            memcpy(&bits, &d, sizeof(bits));
            for (int i = 0; i < 8; i++) {
                this->data.push_back((bits >> (i * 8)) & 0xFF);
            }
        
    }
    void writeString(std::string val) {
        auto len = val.length();
        this->writeU32(len);
        // Inline C++ code:

            for (char c : val) {
                this->data.push_back(static_cast<unsigned char>(c));
            }
        
    }
    void writeValue(SlateValue val) {
        this->writeU8(val.valueType);
        if ((val.valueType == 1)) {
            this->writeI64(val.intValue);
        }
        else {
            if ((val.valueType == 2)) {
                this->writeF64(val.floatValue);
            }
            else {
                if ((val.valueType == 3)) {
                    this->writeString(val.stringValue);
                }
                else {
                    if ((val.valueType == 4)) {
                        if (val.boolValue) {
                            this->writeU8(1);
                        }
                        else {
                            this->writeU8(0);
                        }
                    }
                    else {
                        if ((val.valueType == 5)) {
                            this->writeI64(val.objectId);
                        }
                        else {
                            if ((val.valueType == 6)) {
                                // Inline C++ code:

                int count = val.arrayValue.size();
                this->writeU32(count);
                for (int i = 0; i < count; i++) {
                    this->writeValue(val.arrayValue[i]);
                }
            
                            }
                        }
                    }
                }
            }
        }
    }
    std::vector<int64_t> toInt64Vector() {
        // Inline C++ code:

            std::vector<int64_t> result;
            result.reserve(this->data.size());
            for (int byte : this->data) {
                result.push_back(static_cast<int64_t>(byte));
            }
            return result;
        
    }
};

// Auto-generated print support
inline std::ostream& operator<<(std::ostream& os, const BinaryWriter& obj) {
    os << "BinaryWriter { ";
    os << " }";
    return os;
}

class BinaryReader {
public:
    std::vector<int64_t> data;
    int64_t pos;
    void create() {
        // Inline C++ code:

            this->data = std::vector<int>();
        
        this->pos = 0;
    }
    void setFromInt64Vector(std::vector<int64_t> int64Data) {
        // Inline C++ code:

            this->data.clear();
            for (int64_t byte : int64Data) {
                this->data.push_back(static_cast<int>(byte));
            }
            this->pos = 0;
        
    }
    int64_t readU8() {
        auto v = this->data[this->pos];
        this->pos = (this->pos + 1);
        return v;
    }
    int64_t readU32() {
        auto b0 = this->readU8();
        auto b1 = this->readU8();
        auto b2 = this->readU8();
        auto b3 = this->readU8();
        // Inline C++ code:
 return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24); 
    }
    int64_t readI64() {
        // Inline C++ code:

            int64_t v = 0;
            for (int i = 0; i < 8; i++) {
                v |= static_cast<int64_t>(this->data[this->pos++]) << (i * 8);
            }
            return v;
        
    }
    double readF64() {
        // Inline C++ code:

            uint64_t bits = 0;
            for (int i = 0; i < 8; i++) {
                bits |= static_cast<uint64_t>(this->data[this->pos++]) << (i * 8);
            }
            double d;
            memcpy(&d, &bits, sizeof(d));
            return d;
        
    }
    std::string readString() {
        auto len = this->readU32();
        // Inline C++ code:

            std::string s;
            s.reserve(len);
            for (int i = 0; i < len; i++) {
                s += static_cast<char>(this->data[this->pos++]);
            }
            return s;
        
    }
    SlateValue readValue() {
        auto typeCode = this->readU8();
        auto v = SlateValue();
        v.valueType = typeCode;
        if ((typeCode == 1)) {
            v.intValue = this->readI64();
        }
        else {
            if ((typeCode == 2)) {
                v.floatValue = this->readF64();
            }
            else {
                if ((typeCode == 3)) {
                    v.stringValue = this->readString();
                }
                else {
                    if ((typeCode == 4)) {
                        v.boolValue = (this->readU8() != 0);
                    }
                    else {
                        if ((typeCode == 5)) {
                            v.objectId = this->readI64();
                        }
                        else {
                            if ((typeCode == 6)) {
                                // Inline C++ code:

                int count = this->readU32();
                for (int i = 0; i < count; i++) {
                    v.arrayValue.push_back(this->readValue());
                }
            
                            }
                        }
                    }
                }
            }
        }
        return v;
    }
};

// Auto-generated print support
inline std::ostream& operator<<(std::ostream& os, const BinaryReader& obj) {
    os << "BinaryReader { ";
    os << "pos: " << obj.pos;
    os << " }";
    return os;
}

class Schema {
public:
    SlateSchema schema;
    void create() {
        this->schema = SlateSchema();
    }
    static Schema define(std::string name) {
        auto s = Schema();
        s.schema.className = name;
        return s;
    }
    Schema addInt(std::string name) {
        this->schema.addField(name, 1);
        return *this;
    }
    Schema addFloat(std::string name) {
        this->schema.addField(name, 2);
        return *this;
    }
    Schema addString(std::string name) {
        this->schema.addField(name, 3);
        return *this;
    }
    Schema addBool(std::string name) {
        this->schema.addField(name, 4);
        return *this;
    }
    Schema addRef(std::string name) {
        this->schema.addField(name, 5);
        return *this;
    }
    Schema addArray(std::string name) {
        this->schema.addField(name, 6);
        return *this;
    }
    SlateSchema build() {
        return this->schema;
    }
};

// Auto-generated print support
inline std::ostream& operator<<(std::ostream& os, const Schema& obj) {
    os << "Schema { ";
    os << " }";
    return os;
}

void exportObject(SlateObject obj, std::string filename);
SlateObject importObject(std::string filename);
void exportObjectEncrypted(SlateObject obj, std::string filename, std::string password);
SlateObject importObjectEncrypted(std::string filename, std::string password);

void exportObject(SlateObject obj, std::string filename) {
    auto writer = BinaryWriter();
    writer.writeString(obj.className);
    writer.writeI64(obj.objectId);
    // Inline C++ code:

        writer.writeU32(obj.fields.size());
        
        // Write each field
        for (const auto& [key, value] : obj.fields) {
            writer.writeString(key);
            writer.writeValue(value);
        }
    
    // Inline C++ code:

        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return;
        }
        
        for (int byte : writer.data) {
            file.put(static_cast<char>(byte));
        }
        
        file.close();
        std::cout << "Exported to " << filename << " (" << writer.data.size() << " bytes)" << std::endl;
    
}

SlateObject importObject(std::string filename) {
    auto reader = BinaryReader();
    // Inline C++ code:

        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return SlateObject();
        }
        
        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        file.seekg(0, std::ios::beg);
        
        reader.data.clear();
        for (size_t i = 0; i < size; i++) {
            reader.data.push_back(static_cast<unsigned char>(file.get()));
        }
        
        file.close();
        std::cout << "Loaded " << size << " bytes from " << filename << std::endl;
    
    reader.pos = 0;
    auto obj = SlateObject();
    obj.className = reader.readString();
    obj.objectId = reader.readI64();
    auto fieldCount = reader.readU32();
    // Inline C++ code:

        for (int i = 0; i < fieldCount; i++) {
            std::string key = reader.readString();
            SlateValue value = reader.readValue();
            obj.fields[key] = value;
        }
    
    return obj;
}

void exportObjectEncrypted(SlateObject obj, std::string filename, std::string password) {
    auto writer = BinaryWriter();
    writer.writeString(obj.className);
    writer.writeI64(obj.objectId);
    // Inline C++ code:

        writer.writeU32(obj.fields.size());
        
        // Write each field
        for (const auto& [key, value] : obj.fields) {
            writer.writeString(key);
            writer.writeValue(value);
        }
    
    // Inline C++ code:

        // Convert int to int64_t for Crypto::encrypt
        std::vector<int64_t> plaintextData;
        plaintextData.reserve(writer.data.size());
        for (int byte : writer.data) {
            plaintextData.push_back(static_cast<int64_t>(byte));
        }
        
        // Import Crypto functions directly
        using Crypto::CryptoResult;
        using Crypto::encrypt;
        
        // Encrypt data
        CryptoResult encrypted = encrypt(plaintextData, password);
        
        if (!encrypted.success) {
            std::cerr << "Encryption failed: " << encrypted.error << std::endl;
            return;
        }
        
        // Write encrypted file with metadata
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return;
        }
        
        // Write magic header
        file.write("SLATEDB", 7);
        
        // Write salt (16 bytes)
        for (int64_t byte : encrypted.salt) {
            file.put(static_cast<char>(byte));
        }
        
        // Write IV (12 bytes)
        for (int64_t byte : encrypted.iv) {
            file.put(static_cast<char>(byte));
        }
        
        // Write tag (16 bytes)
        for (int64_t byte : encrypted.tag) {
            file.put(static_cast<char>(byte));
        }
        
        // Write data length (4 bytes)
        uint32_t len = encrypted.data.size();
        file.put((len >> 0) & 0xFF);
        file.put((len >> 8) & 0xFF);
        file.put((len >> 16) & 0xFF);
        file.put((len >> 24) & 0xFF);
        
        // Write encrypted data
        for (int64_t byte : encrypted.data) {
            file.put(static_cast<char>(byte));
        }
        
        file.close();
        std::cout << "Exported encrypted to " << filename << " (" 
                  << (7 + 16 + 12 + 16 + 4 + encrypted.data.size()) 
                  << " bytes)" << std::endl;
    
}

SlateObject importObjectEncrypted(std::string filename, std::string password) {
    // Inline C++ code:

        // Import Crypto functions directly
        using Crypto::CryptoResult;
        using Crypto::decrypt;
        
        CryptoResult encrypted;
        
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return SlateObject();
        }
        
        // Read and verify magic header
        char magic[8];
        file.read(magic, 7);
        magic[7] = '\0';
        if (std::string(magic) != "SLATEDB") {
            std::cerr << "Invalid file format" << std::endl;
            return SlateObject();
        }
        
        // Read salt (16 bytes)
        encrypted.salt.resize(16);
        for (int i = 0; i < 16; i++) {
            encrypted.salt[i] = static_cast<int64_t>(static_cast<unsigned char>(file.get()));
        }
        
        // Read IV (12 bytes)
        encrypted.iv.resize(12);
        for (int i = 0; i < 12; i++) {
            encrypted.iv[i] = static_cast<int64_t>(static_cast<unsigned char>(file.get()));
        }
        
        // Read tag (16 bytes)
        encrypted.tag.resize(16);
        for (int i = 0; i < 16; i++) {
            encrypted.tag[i] = static_cast<int64_t>(static_cast<unsigned char>(file.get()));
        }
        
        // Read data length
        uint32_t len = 0;
        len |= static_cast<unsigned char>(file.get()) << 0;
        len |= static_cast<unsigned char>(file.get()) << 8;
        len |= static_cast<unsigned char>(file.get()) << 16;
        len |= static_cast<unsigned char>(file.get()) << 24;
        
        // Read encrypted data
        encrypted.data.resize(len);
        for (uint32_t i = 0; i < len; i++) {
            encrypted.data[i] = static_cast<int64_t>(static_cast<unsigned char>(file.get()));
        }
        
        file.close();
        encrypted.success = true;
        
        std::cout << "Loaded " << (7 + 16 + 12 + 16 + 4 + len) 
                  << " bytes from " << filename << std::endl;
        
        // Decrypt
        CryptoResult decrypted = decrypt(encrypted, password);
        
        if (!decrypted.success) {
            std::cerr << "Decryption failed: " << decrypted.error << std::endl;
            return SlateObject();
        }
        
        // Parse decrypted data - convert int64_t back to int
        BinaryReader reader;
        reader.data.clear();
        for (int64_t byte : decrypted.data) {
            reader.data.push_back(static_cast<int>(byte));
        }
        reader.pos = 0;
        
        // Read object metadata
        SlateObject obj;
        obj.className = reader.readString();
        obj.objectId = reader.readI64();
        
        // Read field count
        int fieldCount = reader.readU32();
        
        // Read each field
        for (int i = 0; i < fieldCount; i++) {
            std::string key = reader.readString();
            SlateValue value = reader.readValue();
            obj.fields[key] = value;
        }
        
        return obj;
    
}

int main() {
    println(std::string("Testing SlateDB with AES-256-GCM Encryption..."));
    println(std::string(""));
    auto user = SlateObject();
    user.className = std::string("User");
    user.objectId = 1;
    user.setField(std::string("id"), SlateValue::makeInt(1));
    user.setField(std::string("name"), SlateValue::makeString(std::string("Alice")));
    user.setField(std::string("email"), SlateValue::makeString(std::string("alice@example.com")));
    user.setField(std::string("active"), SlateValue::makeBool(true));
    user.setField(std::string("balance"), SlateValue::makeFloat(1234.560000));
    println(std::string("=== Original Data ==="));
    println((std::string("User ID: ") + mg_to_string(user.getInt("id"))));
    println((std::string("User Name: ") + mg_to_string(user.getString("name"))));
    println((std::string("User Email: ") + mg_to_string(user.getString("email"))));
    println((std::string("User Active: ") + mg_to_string(user.getBool("active"))));
    println(std::string(""));
    println(std::string("=== Unencrypted Export ==="));
    exportObject(user, std::string("user_plain.db"));
    println(std::string("(You can read strings in this file)"));
    println(std::string(""));
    println(std::string("=== Encrypted Export (AES-256-GCM) ==="));
    exportObjectEncrypted(user, std::string("user_secure.db"), std::string("my_secret_password"));
    println(std::string("(This file is completely unreadable)"));
    println(std::string(""));
    println(std::string("=== Importing Encrypted Data ==="));
    auto loadedUser = importObjectEncrypted(std::string("user_secure.db"), std::string("my_secret_password"));
    println(std::string("=== Decrypted Data ==="));
    println((std::string("User ID: ") + mg_to_string(loadedUser.getInt("id"))));
    println((std::string("User Name: ") + mg_to_string(loadedUser.getString("name"))));
    println((std::string("User Email: ") + mg_to_string(loadedUser.getString("email"))));
    println((std::string("User Active: ") + mg_to_string(loadedUser.getBool("active"))));
    println(std::string(""));
    println(std::string("=== Testing Wrong Password ==="));
    auto badUser = importObjectEncrypted(std::string("user_secure.db"), std::string("wrong_password"));
    println(std::string("(Should fail with authentication error)"));
    println(std::string(""));
    println(std::string("SlateDB encryption test completed!"));
    return 0;
}

