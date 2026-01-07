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
    
    auto plainData = writer.data;
    auto encrypted = Crypto::encrypt(plainData, password);
    if ((!encrypted.success)) {
        // Inline C++ code:

            std::cerr << "Encryption failed: " << encrypted.error << std::endl;
            return;
        
    }
    // Inline C++ code:

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

