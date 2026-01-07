// Auto-generated C++ code from Magolor

// User-provided C++ headers

    #include <cstring>

#include <vector>
#include <unordered_map>
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

// ============================================================================
// Auto-generated Standard Library Implementations
// ============================================================================

// ============================================================================
// Std.Crypto (Auto-generated from stdlib)
// ============================================================================
namespace Crypto {

    class CryptoResult {
    public:
        bool success = false;
        std::vector<int64_t> data = 0;
        std::string error = "";
        std::vector<int64_t> iv = 0;
        std::vector<int64_t> tag = 0;
        std::vector<int64_t> salt = 0;
    };

    class KeyConfig {
    public:
        int64_t iterations = 0;
        int64_t keyLength = 0;
    };

    inline void create() {
        // Implementation not found
    }

    inline void create() {
        // Implementation not found
    }

    inline KeyConfig standard() {
        // Implementation not found
    }

    inline KeyConfig fast() {
        // Implementation not found
    }

    inline KeyConfig paranoid() {
        // Implementation not found
    }

    inline std::vector<int64_t> deriveKey(const std::string& password, const std::vector<int64_t>& salt, KeyConfig config) {
        #include <openssl/evp.h>
                #include <openssl/rand.h>
                
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
                
                for (int i = 0; i < config.keyLength; i++) {
                    key[i] = keyBytes[i];
                }
                return key;
    }

    inline std::vector<int64_t> randomBytes(int64_t count) {
        #include <openssl/rand.h>
                
                std::vector<unsigned char> bytes(count);
                RAND_bytes(bytes.data(), count);
                
                std::vector<int64_t> result(count);
                for (int i = 0; i < count; i++) {
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
        #include <openssl/evp.h>
                #include <openssl/rand.h>
                
                CryptoResult result;
                result.success = false;
                
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
        #include <openssl/evp.h>
                
                CryptoResult result;
                result.success = false;
                
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
    }

    inline std::string decryptToString(CryptoResult encrypted, const std::string& password) {
        std::string text;
                text.reserve(result.data.size());
                for (auto b : result.data) {
                    text += static_cast<char>(b);
                }
                return text;
    }

    inline std::string hashPassword(const std::string& password) {
        #include <openssl/evp.h>
                #include <openssl/rand.h>
                #include <sstream>
                #include <iomanip>
                
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
        #include <openssl/evp.h>
                
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

// ============================================================================
// Std.Map (Auto-generated from stdlib)
// ============================================================================
namespace Map {

    inline int64_t size(Map<any map) {
        return map.size();
    }

    inline bool isEmpty(Map<any map) {
        return map.empty();
    }

    inline void clear(Map<any map) {
        map.clear();
    }

    inline Option<any> get(Map<any map, any key) {
        auto it = map.find(key);
                if (it != map.end()) {
                    return std::make_optional(it->second);
                }
                return std::nullopt;
    }

    inline any getOr(Map<any map, any key, any defaultVal) {
        auto it = map.find(key);
                if (it != map.end()) {
                    return it->second;
                }
                return defaultVal;
    }

    inline bool contains(Map<any map, any key) {
        return map.find(key) != map.end();
    }

    inline void insert(Map<any map, any key, any value) {
        map[key] = value;
    }

    inline void set(Map<any map, any key, any value) {
        map[key] = value;
    }

    inline bool remove(Map<any map, any key) {
        auto it = map.find(key);
                if (it != map.end()) {
                    map.erase(it);
                    return true;
                }
                return false;
    }

    inline any) update(Map<any map, any key, fn(any f) {
        auto it = map.find(key);
                if (it != map.end()) {
                    it->second = f(it->second);
                }
    }

    inline bool insertIfAbsent(Map<any map, any key, any value) {
        if (map.find(key) == map.end()) {
                    map[key] = value;
                    return true;
                }
                return false;
    }

    inline Array<any> keys(Map<any map) {
        std::vector<decltype(map)::key_type> result;
                result.reserve(map.size());
                for (const auto& pair : map) {
                    result.push_back(pair.first);
                }
                return result;
    }

    inline Array<any> values(Map<any map) {
        std::vector<decltype(map)::mapped_type> result;
                result.reserve(map.size());
                for (const auto& pair : map) {
                    result.push_back(pair.second);
                }
                return result;
    }

    inline Array<Array<any>> entries(Map<any map) {
        std::vector<std::pair<decltype(map)::key_type, decltype(map)::mapped_type>> result;
                result.reserve(map.size());
                for (const auto& pair : map) {
                    result.push_back(pair);
                }
                return result;
    }

    inline any) -> Map<any, any> mapValues(Map<any map, fn(any f) {
        std::unordered_map<decltype(map)::key_type, decltype(f(map.begin()->second))> result;
                for (const auto& pair : map) {
                    result[pair.first] = f(pair.second);
                }
                return result;
    }

    inline bool) -> Map<any, any> filterMap(Map<any map, fn(any predicate) {
        decltype(map) result;
                for (const auto& pair : map) {
                    if (predicate(pair.first, pair.second)) {
                        result[pair.first] = pair.second;
                    }
                }
                return result;
    }

    inline Map<any, any> merge(Map<any a, Map<any b) {
        auto result = a;
                for (const auto& pair : b) {
                    result[pair.first] = pair.second;
                }
                return result;
    }

    inline any) -> Map<any, any> mergeWith(Map<any a, Map<any b, fn(any f) {
        auto result = a;
                for (const auto& pair : b) {
                    auto it = result.find(pair.first);
                    if (it != result.end()) {
                        it->second = f(it->second, pair.second);
                    } else {
                        result[pair.first] = pair.second;
                    }
                }
                return result;
    }

    inline Map<any, any> fromEntries(Array<Array<any>> entries) {
        std::unordered_map<decltype(entries[0][0]), decltype(entries[0][1])> result;
                for (const auto& entry : entries) {
                    if (entry.size() >= 2) {
                        result[entry[0]] = entry[1];
                    }
                }
                return result;
    }

    inline Map<any, any> invert(Map<any map) {
        std::unordered_map<decltype(map)::mapped_type, decltype(map)::key_type> result;
                for (const auto& pair : map) {
                    result[pair.second] = pair.first;
                }
                return result;
    }

    inline any) -> Map<any, int> countBy(Array<any> arr, fn(any keyFn) {
        std::unordered_map<decltype(keyFn(arr[0])), int> result;
                for (const auto& item : arr) {
                    result[keyFn(item)]++;
                }
                return result;
    }

    inline any) -> Map<any, Array<any>> groupBy(Array<any> arr, fn(any keyFn) {
        std::unordered_map<decltype(keyFn(arr[0])), std::vector<decltype(arr)::value_type>> result;
                for (const auto& item : arr) {
                    result[keyFn(item)].push_back(item);
                }
                return result;
    }

} // namespace Map

// ============================================================================
// Std.Array (Auto-generated from stdlib)
// ============================================================================
namespace Array {

    inline int64_t length(Array<any> arr) {
        return arr.size();
    }

    inline bool isEmpty(Array<any> arr) {
        return arr.empty();
    }

    inline int64_t capacity(Array<any> arr) {
        return arr.capacity();
    }

    inline Option<any> get(Array<any> arr, int64_t index) {
        if (index >= 0 && index < static_cast<int>(arr.size())) {
                    return std::make_optional(arr[index]);
                }
                return std::nullopt;
    }

    inline Option<any> first(Array<any> arr) {
        // Implementation not found
    }

    inline Option<any> last(Array<any> arr) {
        // Implementation not found
    }

    inline void push(Array<any> arr, any item) {
        arr.push_back(item);
    }

    inline Option<any> pop(Array<any> arr) {
        if (arr.empty()) return std::nullopt;
                auto item = arr.back();
                arr.pop_back();
                return std::make_optional(item);
    }

    inline void insert(Array<any> arr, int64_t index, any item) {
        arr.insert(arr.begin() + index, item);
    }

    inline Option<any> removeAt(Array<any> arr, int64_t index) {
        if (index < 0 || index >= static_cast<int>(arr.size())) {
                    return std::nullopt;
                }
                auto item = arr[index];
                arr.erase(arr.begin() + index);
                return std::make_optional(item);
    }

    inline void clear(Array<any> arr) {
        arr.clear();
    }

    inline void resize(Array<any> arr, int64_t newSize) {
        arr.resize(newSize);
    }

    inline void reserve(Array<any> arr, int64_t capacity) {
        arr.reserve(capacity);
    }

    inline bool contains(Array<any> arr, any item) {
        return std::find(arr.begin(), arr.end(), item) != arr.end();
    }

    inline Option<int> indexOf(Array<any> arr, any item) {
        auto it = std::find(arr.begin(), arr.end(), item);
                if (it != arr.end()) {
                    return std::make_optional(static_cast<int>(std::distance(arr.begin(), it)));
                }
                return std::nullopt;
    }

    inline Option<int> lastIndexOf(Array<any> arr, any item) {
        for (int i = arr.size() - 1; i >= 0; i--) {
                    if (arr[i] == item) {
                        return std::make_optional(i);
                    }
                }
                return std::nullopt;
    }

    inline int64_t count(Array<any> arr, any item) {
        return std::count(arr.begin(), arr.end(), item);
    }

    inline void reverse(Array<any> arr) {
        std::reverse(arr.begin(), arr.end());
    }

    inline void sort(Array<any> arr) {
        std::sort(arr.begin(), arr.end());
    }

    inline void sortDesc(Array<any> arr) {
        std::sort(arr.begin(), arr.end(), std::greater<>());
    }

    inline void shuffle(Array<any> arr) {
        static std::random_device rd;
                static std::mt19937 g(rd());
                std::shuffle(arr.begin(), arr.end(), g);
    }

    inline Array<any> slice(Array<any> arr, int64_t start, int64_t endIdx) {
        if (start < 0) start = 0;
                if (endIdx > static_cast<int>(arr.size())) endIdx = arr.size();
                return std::vector<decltype(arr)::value_type>(arr.begin() + start, arr.begin() + endIdx);
    }

    inline Array<any> concat(Array<any> a, Array<any> b) {
        auto result = a;
                result.insert(result.end(), b.begin(), b.end());
                return result;
    }

    inline Array<any> flatten(Array<Array<any>> arr) {
        std::vector<typename decltype(arr)::value_type::value_type> result;
                for (const auto& inner : arr) {
                    result.insert(result.end(), inner.begin(), inner.end());
                }
                return result;
    }

    inline Array<any> dedupe(Array<any> arr) {
        auto result = arr;
                std::sort(result.begin(), result.end());
                result.erase(std::unique(result.begin(), result.end()), result.end());
                return result;
    }

    inline any) -> Array<any> map(Array<any> arr, fn(any f) {
        std::vector<decltype(f(arr[0]))> result;
                result.reserve(arr.size());
                for (const auto& item : arr) {
                    result.push_back(f(item));
                }
                return result;
    }

    inline bool) -> Array<any> filter(Array<any> arr, fn(any predicate) {
        std::vector<decltype(arr)::value_type> result;
                for (const auto& item : arr) {
                    if (predicate(item)) {
                        result.push_back(item);
                    }
                }
                return result;
    }

    inline any) -> any reduce(Array<any> arr, any initial, fn(any f) {
        auto acc = initial;
                for (const auto& item : arr) {
                    acc = f(acc, item);
                }
                return acc;
    }

    inline bool) -> Option<any> find(Array<any> arr, fn(any predicate) {
        for (const auto& item : arr) {
                    if (predicate(item)) {
                        return std::make_optional(item);
                    }
                }
                return std::nullopt;
    }

    inline bool) -> Option<int> findIndex(Array<any> arr, fn(any predicate) {
        for (size_t i = 0; i < arr.size(); i++) {
                    if (predicate(arr[i])) {
                        return std::make_optional(static_cast<int>(i));
                    }
                }
                return std::nullopt;
    }

    inline bool) -> bool any(Array<any> arr, fn(any predicate) {
        return std::any_of(arr.begin(), arr.end(), predicate);
    }

    inline bool) -> bool all(Array<any> arr, fn(any predicate) {
        return std::all_of(arr.begin(), arr.end(), predicate);
    }

    inline bool) -> bool none(Array<any> arr, fn(any predicate) {
        return std::none_of(arr.begin(), arr.end(), predicate);
    }

    inline int64_t sum(const std::vector<int64_t>& arr) {
        return std::accumulate(arr.begin(), arr.end(), 0);
    }

    inline double sumf(Array<float> arr) {
        return std::accumulate(arr.begin(), arr.end(), 0.0);
    }

    inline Option<int> minVal(const std::vector<int64_t>& arr) {
        if (arr.empty()) return std::nullopt;
                return std::make_optional(*std::min_element(arr.begin(), arr.end()));
    }

    inline Option<int> maxVal(const std::vector<int64_t>& arr) {
        if (arr.empty()) return std::nullopt;
                return std::make_optional(*std::max_element(arr.begin(), arr.end()));
    }

    inline std::vector<int64_t> range(int64_t start, int64_t endVal) {
        std::vector<int> result;
                for (int i = start; i < endVal; i++) {
                    result.push_back(i);
                }
                return result;
    }

    inline std::vector<int64_t> rangeStep(int64_t start, int64_t endVal, int64_t step) {
        std::vector<int> result;
                for (int i = start; i < endVal; i += step) {
                    result.push_back(i);
                }
                return result;
    }

    inline Array<any> filled(int64_t size, any value) {
        return std::vector<decltype(value)>(size, value);
    }

    inline std::vector<int64_t> zeros(int64_t size) {
        return std::vector<int>(size, 0);
    }

    inline std::vector<int64_t> ones(int64_t size) {
        return std::vector<int>(size, 1);
    }

    inline Array<Array<any>> zip(Array<any> a, Array<any> b) {
        std::vector<std::vector<decltype(a)::value_type>> result;
                size_t len = std::min(a.size(), b.size());
                for (size_t i = 0; i < len; i++) {
                    result.push_back({a[i], b[i]});
                }
                return result;
    }

    inline Array<Array<any>> enumerate(Array<any> arr) {
        std::vector<std::pair<int, decltype(arr)::value_type>> result;
                for (size_t i = 0; i < arr.size(); i++) {
                    result.push_back({static_cast<int>(i), arr[i]});
                }
                return result;
    }

} // namespace Array

// ============================================================================
// Std.String (Auto-generated from stdlib)
// ============================================================================
namespace String {

    inline int64_t length(const std::string& s) {
        return s.length();
    }

    inline bool isEmpty(const std::string& s) {
        return s.empty();
    }

    inline std::string charAt(const std::string& s, int64_t index) {
        return std::string(1, s[index]);
    }

    inline int64_t charCodeAt(const std::string& s, int64_t index) {
        return static_cast<int>(s[index]);
    }

    inline std::string fromCharCode(int64_t code) {
        return std::string(1, static_cast<char>(code));
    }

    inline std::string toLower(const std::string& s) {
        std::string result = s;
                std::transform(result.begin(), result.end(), result.begin(), ::tolower);
                return result;
    }

    inline std::string toUpper(const std::string& s) {
        std::string result = s;
                std::transform(result.begin(), result.end(), result.begin(), ::toupper);
                return result;
    }

    inline std::string capitalize(const std::string& s) {
        // Implementation not found
    }

    inline std::string titleCase(const std::string& s) {
        // Implementation not found
    }

    inline std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\n\r\f\v");
                if (start == std::string::npos) return "";
                size_t end = s.find_last_not_of(" \t\n\r\f\v");
                return s.substr(start, end - start + 1);
    }

    inline std::string trimStart(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\n\r\f\v");
                if (start == std::string::npos) return "";
                return s.substr(start);
    }

    inline std::string trimEnd(const std::string& s) {
        size_t end = s.find_last_not_of(" \t\n\r\f\v");
                if (end == std::string::npos) return "";
                return s.substr(0, end + 1);
    }

    inline bool contains(const std::string& s, const std::string& substr) {
        return s.find(substr) != std::string::npos;
    }

    inline bool startsWith(const std::string& s, const std::string& prefix) {
        return s.size() >= prefix.size() && 
                       s.compare(0, prefix.size(), prefix) == 0;
    }

    inline bool endsWith(const std::string& s, const std::string& suffix) {
        return s.size() >= suffix.size() && 
                       s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
    }

    inline Option<int> indexOf(const std::string& s, const std::string& substr) {
        size_t pos = s.find(substr);
                if (pos != std::string::npos) {
                    return std::make_optional(static_cast<int>(pos));
                }
                return std::nullopt;
    }

    inline Option<int> lastIndexOf(const std::string& s, const std::string& substr) {
        size_t pos = s.rfind(substr);
                if (pos != std::string::npos) {
                    return std::make_optional(static_cast<int>(pos));
                }
                return std::nullopt;
    }

    inline int64_t count(const std::string& s, const std::string& substr) {
        int count = 0;
                size_t pos = 0;
                while ((pos = s.find(substr, pos)) != std::string::npos) {
                    count++;
                    pos += substr.length();
                }
                return count;
    }

    inline std::string substring(const std::string& s, int64_t start) {
        return s.substr(start);
    }

    inline std::string substringLen(const std::string& s, int64_t start, int64_t len) {
        return s.substr(start, len);
    }

    inline std::string slice(const std::string& s, int64_t start, int64_t endIdx) {
        return s.substr(start, endIdx - start);
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
        // Implementation not found
    }

    inline Array<string> split(const std::string& s, const std::string& delim) {
        std::vector<std::string> result;
                size_t start = 0;
                size_t end;
                while ((end = s.find(delim, start)) != std::string::npos) {
                    result.push_back(s.substr(start, end - start));
                    start = end + delim.length();
                }
                result.push_back(s.substr(start));
                return result;
    }

    inline Array<string> splitChar(const std::string& s, const std::string& delim) {
        std::vector<std::string> result;
                std::stringstream ss(s);
                std::string token;
                char d = delim[0];
                while (std::getline(ss, token, d)) {
                    result.push_back(token);
                }
                return result;
    }

    inline Array<string> splitLines(const std::string& s) {
        // Implementation not found
    }

    inline Array<string> splitWhitespace(const std::string& s) {
        std::vector<std::string> result;
                std::stringstream ss(s);
                std::string word;
                while (ss >> word) {
                    result.push_back(word);
                }
                return result;
    }

    inline std::string join(Array<string> parts, const std::string& sep) {
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
                for (int i = 0; i < count; i++) {
                    result += s;
                }
                return result;
    }

    inline std::string padStart(const std::string& s, int64_t targetLen, const std::string& padStr) {
        // Implementation not found
    }

    inline std::string padEnd(const std::string& s, int64_t targetLen, const std::string& padStr) {
        // Implementation not found
    }

    inline std::string reverse(const std::string& s) {
        std::string result = s;
                std::reverse(result.begin(), result.end());
                return result;
    }

    inline bool isDigit(const std::string& c) {
        return !c.empty() && std::isdigit(c[0]);
    }

    inline bool isAlpha(const std::string& c) {
        return !c.empty() && std::isalpha(c[0]);
    }

    inline bool isAlphaNum(const std::string& c) {
        return !c.empty() && std::isalnum(c[0]);
    }

    inline bool isWhitespace(const std::string& c) {
        return !c.empty() && std::isspace(c[0]);
    }

    inline bool isUpper(const std::string& c) {
        return !c.empty() && std::isupper(c[0]);
    }

    inline bool isLower(const std::string& c) {
        return !c.empty() && std::islower(c[0]);
    }

    inline Option<int> parseInt(const std::string& s) {
        try {
                    size_t pos;
                    int val = std::stoi(s, &pos);
                    if (pos == s.length()) return std::make_optional(val);
                    return std::nullopt;
                } catch (...) {
                    return std::nullopt;
                }
    }

    inline Option<float> parseFloat(const std::string& s) {
        try {
                    size_t pos;
                    double val = std::stod(s, &pos);
                    if (pos == s.length()) return std::make_optional(val);
                    return std::nullopt;
                } catch (...) {
                    return std::nullopt;
                }
    }

    inline Option<bool> parseBool(const std::string& s) {
        // Implementation not found
    }

    inline std::string format(const std::string& template, Map<string args) {
        // Implementation not found
    }

    inline std::string escapeHtml(const std::string& s) {
        // Implementation not found
    }

    inline std::string escapeJson(const std::string& s) {
        // Implementation not found
    }

} // namespace String

// ============================================================================
// Std.File (Auto-generated from stdlib)
// ============================================================================
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

    inline bool isSymlink(const std::string& path) {
        return std::filesystem::is_symlink(path);
    }

    inline bool isReadable(const std::string& path) {
        auto perms = std::filesystem::status(path).permissions();
                return (perms & std::filesystem::perms::owner_read) != std::filesystem::perms::none;
    }

    inline bool isWritable(const std::string& path) {
        auto perms = std::filesystem::status(path).permissions();
                return (perms & std::filesystem::perms::owner_write) != std::filesystem::perms::none;
    }

    inline Option<string> readFile(const std::string& path) {
        std::ifstream file(path);
                if (!file) return std::nullopt;
                std::stringstream buffer;
                buffer << file.rdbuf();
                return buffer.str();
    }

    inline bool writeFile(const std::string& path, const std::string& content) {
        std::ofstream file(path);
                if (!file) return false;
                file << content;
                return true;
    }

    inline bool appendFile(const std::string& path, const std::string& content) {
        std::ofstream file(path, std::ios::app);
                if (!file) return false;
                file << content;
                return true;
    }

    inline Option<Array<string>> readLines(const std::string& path) {
        std::ifstream file(path);
                if (!file) return std::nullopt;
                std::vector<std::string> lines;
                std::string line;
                while (std::getline(file, line)) {
                    lines.push_back(line);
                }
                return lines;
    }

    inline bool writeLines(const std::string& path, Array<string> lines) {
        std::ofstream file(path);
                if (!file) return false;
                for (const auto& line : lines) {
                    file << line << "\n";
                }
                return true;
    }

    inline Option<Array<int>> readBytes(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
                if (!file) return std::nullopt;
                std::vector<int> bytes;
                char byte;
                while (file.get(byte)) {
                    bytes.push_back(static_cast<unsigned char>(byte));
                }
                return bytes;
    }

    inline bool writeBytes(const std::string& path, const std::vector<int64_t>& bytes) {
        std::ofstream file(path, std::ios::binary);
                if (!file) return false;
                for (int b : bytes) {
                    file.put(static_cast<char>(b));
                }
                return true;
    }

    inline Option<int> size(const std::string& path) {
        try {
                    return std::make_optional(static_cast<int>(std::filesystem::file_size(path)));
                } catch (...) {
                    return std::nullopt;
                }
    }

    inline Option<int> modifiedTime(const std::string& path) {
        try {
                    auto ftime = std::filesystem::last_write_time(path);
                    auto sctp = std::chrono::time_point_cast<std::chrono::seconds>(
                        std::chrono::file_clock::to_sys(ftime));
                    return std::make_optional(static_cast<int>(sctp.time_since_epoch().count()));
                } catch (...) {
                    return std::nullopt;
                }
    }

    inline bool copy(const std::string& from, const std::string& to) {
        try {
                    std::filesystem::copy(from, to, std::filesystem::copy_options::overwrite_existing);
                    return true;
                } catch (...) {
                    return false;
                }
    }

    inline bool move(const std::string& from, const std::string& to) {
        try {
                    std::filesystem::rename(from, to);
                    return true;
                } catch (...) {
                    return false;
                }
    }

    inline bool rename(const std::string& from, const std::string& to) {
        // Implementation not found
    }

    inline bool remove(const std::string& path) {
        try {
                    return std::filesystem::remove(path);
                } catch (...) {
                    return false;
                }
    }

    inline int64_t removeAll(const std::string& path) {
        try {
                    return static_cast<int>(std::filesystem::remove_all(path));
                } catch (...) {
                    return 0;
                }
    }

    inline bool createDir(const std::string& path) {
        try {
                    return std::filesystem::create_directory(path);
                } catch (...) {
                    return false;
                }
    }

    inline bool createDirs(const std::string& path) {
        try {
                    return std::filesystem::create_directories(path);
                } catch (...) {
                    return false;
                }
    }

    inline Option<Array<string>> listDir(const std::string& path) {
        try {
                    std::vector<std::string> entries;
                    for (const auto& entry : std::filesystem::directory_iterator(path)) {
                        entries.push_back(entry.path().filename().addString());
                    }
                    return entries;
                } catch (...) {
                    return std::nullopt;
                }
    }

    inline Option<Array<string>> listDirRecursive(const std::string& path) {
        try {
                    std::vector<std::string> entries;
                    for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
                        entries.push_back(entry.path().addString());
                    }
                    return entries;
                } catch (...) {
                    return std::nullopt;
                }
    }

    inline Array<string> glob(const std::string& path, const std::string& pattern) {
        std::vector<std::string> matches;
                try {
                    for (const auto& entry : std::filesystem::directory_iterator(path)) {
                        std::string name = entry.path().filename().addString();
                        // Simple wildcard matching
                        if (pattern == "*" || name.find(pattern.substr(1)) != std::string::npos) {
                            matches.push_back(entry.path().addString());
                        }
                    }
                } catch (...) {}
                return matches;
    }

    inline std::string absolutePath(const std::string& path) {
        return std::filesystem::absolute(path).addString();
    }

    inline Option<string> canonicalPath(const std::string& path) {
        try {
                    return std::filesystem::canonical(path).addString();
                } catch (...) {
                    return std::nullopt;
                }
    }

    inline std::string relativePath(const std::string& path, const std::string& base) {
        return std::filesystem::relative(path, base).addString();
    }

    inline std::string parentPath(const std::string& path) {
        return std::filesystem::path(path).parent_path().addString();
    }

    inline std::string fileName(const std::string& path) {
        return std::filesystem::path(path).filename().addString();
    }

    inline std::string stem(const std::string& path) {
        return std::filesystem::path(path).stem().addString();
    }

    inline std::string extension(const std::string& path) {
        return std::filesystem::path(path).extension().addString();
    }

    inline std::string joinPath(Array<string> parts) {
        if (parts.empty()) return "";
                std::filesystem::path result = parts[0];
                for (size_t i = 1; i < parts.size(); i++) {
                    result /= parts[i];
                }
                return result.addString();
    }

    inline std::string normalizePath(const std::string& path) {
        return std::filesystem::path(path).lexically_normal().addString();
    }

    inline std::string tempDir() {
        return std::filesystem::temp_directory_path().addString();
    }

    inline Option<string> createTempFile(const std::string& prefix) {
        try {
                    auto temp = std::filesystem::temp_directory_path() / (prefix + "XXXXXX");
                    std::string path = temp.addString();
                    int fd = mkstemp(&path[0]);
                    if (fd == -1) return std::nullopt;
                    close(fd);
                    return path;
                } catch (...) {
                    return std::nullopt;
                }
    }

    inline std::string cwd() {
        return std::filesystem::current_path().addString();
    }

    inline bool chdir(const std::string& path) {
        try {
                    std::filesystem::current_path(path);
                    return true;
                } catch (...) {
                    return false;
                }
    }

} // namespace File

// ============================================================================
// Standard Library Helpers (generated once)
// ============================================================================
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

// Array helper wrappers
template<typename T> int length(const ::std::vector<T>& arr) { return arr.size(); }
template<typename T> void push(::std::vector<T>& arr, const T& val) { arr.push_back(val); }
template<typename T> T pop(::std::vector<T>& arr) { auto v = arr.back(); arr.pop_back(); return v; }

// Map helper wrappers
namespace Map {
  template<typename K, typename V> ::std::unordered_map<K,V> create() { return {}; }
  template<typename K, typename V> void insert(::std::unordered_map<K,V>& m, const K& k, const V& v) { m[k] = v; }
  template<typename K, typename V> ::std::optional<V> get(const ::std::unordered_map<K,V>& m, const K& k) {
    auto it = m.find(k); return it != m.end() ? ::std::optional<V>(it->second) : ::std::nullopt;
  }
  template<typename K, typename V> ::std::vector<V> values(const ::std::unordered_map<K,V>& m) {
    ::std::vector<V> r; for(auto& p : m) r.push_back(p.second); return r;
  }
}

// File helper
namespace File {
  inline bool exists(const ::std::string& path) {
    ::std::ifstream f(path); return f.good();
  }
}

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
    static constexpr int VALUE_NULL = 0;
    static constexpr int INT = 1;
    static constexpr int FLOAT = 2;
    static constexpr int STRING = 3;
    static constexpr int BOOL = 4;
    static constexpr int OBJECT_REF = 5;
    static constexpr int ARRAY = 6;
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
    int classId;
    std::vector<std::string> fieldNames;
    std::vector<int> fieldTypes;
    void create() {
        this->className = std::string("");
        this->classId = 0;
        // Inline C++ code:

            this->fieldNames = std::vector<std::string>();
            this->fieldTypes = std::vector<int>();
        
    }
    void addField(std::string name, int typeCode) {
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
    int valueType;
    int intValue;
    double floatValue;
    std::string stringValue;
    bool boolValue;
    int objectId;
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
    static SlateValue makeInt(int value) {
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
    static SlateValue makeRef(int id) {
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
    int objectId;
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
    int getInt(std::string name) {
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
    std::vector<int> data;
    void create() {
        // Inline C++ code:

            this->data = std::vector<int>();
        
    }
    void writeU8(int val) {
        // Inline C++ code:
 this->data.push_back(val & 0xFF); 
    }
    void writeU32(int val) {
        // Inline C++ code:

            this->writeU8(val & 0xFF);
            this->writeU8((val >> 8) & 0xFF);
            this->writeU8((val >> 16) & 0xFF);
            this->writeU8((val >> 24) & 0xFF);
        
    }
    void writeI64(int val) {
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
};

// Auto-generated print support
inline std::ostream& operator<<(std::ostream& os, const BinaryWriter& obj) {
    os << "BinaryWriter { ";
    os << " }";
    return os;
}

class BinaryReader {
public:
    std::vector<int> data;
    int pos;
    void create() {
        // Inline C++ code:

            this->data = std::vector<int>();
        
        this->pos = 0;
    }
    int readU8() {
        auto v = this->data[this->pos];
        this->pos = (this->pos + 1);
        return v;
    }
    int readU32() {
        auto b0 = this->readU8();
        auto b1 = this->readU8();
        auto b2 = this->readU8();
        auto b3 = this->readU8();
        // Inline C++ code:
 return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24); 
    }
    int readI64() {
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

        // Import Crypto functions directly
        using Crypto::CryptoResult;
        using Crypto::encrypt;
        
        // Encrypt data
        CryptoResult encrypted = encrypt(writer.data, password);
        
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
        for (int byte : encrypted.salt) {
            file.put(static_cast<char>(byte));
        }
        
        // Write IV (12 bytes)
        for (int byte : encrypted.iv) {
            file.put(static_cast<char>(byte));
        }
        
        // Write tag (16 bytes)
        for (int byte : encrypted.tag) {
            file.put(static_cast<char>(byte));
        }
        
        // Write data length (4 bytes)
        uint32_t len = encrypted.data.size();
        file.put((len >> 0) & 0xFF);
        file.put((len >> 8) & 0xFF);
        file.put((len >> 16) & 0xFF);
        file.put((len >> 24) & 0xFF);
        
        // Write encrypted data
        for (int byte : encrypted.data) {
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
            encrypted.salt[i] = static_cast<unsigned char>(file.get());
        }
        
        // Read IV (12 bytes)
        encrypted.iv.resize(12);
        for (int i = 0; i < 12; i++) {
            encrypted.iv[i] = static_cast<unsigned char>(file.get());
        }
        
        // Read tag (16 bytes)
        encrypted.tag.resize(16);
        for (int i = 0; i < 16; i++) {
            encrypted.tag[i] = static_cast<unsigned char>(file.get());
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
            encrypted.data[i] = static_cast<unsigned char>(file.get());
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
        
        // Parse decrypted data
        BinaryReader reader;
        reader.data = decrypted.data;
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

