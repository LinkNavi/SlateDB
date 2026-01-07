@cpp_header {
    #include <cstring>
}

using Std.IO;
using Std.File;
using Std.String;
using Std.Array;
using Std.Map;
using Std.Crypto;

// ============================
// ValueType holder (no top-level vars allowed)
// ============================

pub class ValueType {
    pub static VALUE_NULL: int = 0;
    pub static INT: int = 1;
    pub static FLOAT: int = 2;
    pub static STRING: int = 3;
    pub static BOOL: int = 4;
    pub static OBJECT_REF: int = 5;
    pub static ARRAY: int = 6;
}

// ============================
// SlateSchema
// ============================

pub class SlateSchema {
    pub className: string;
    pub classId: int;
    pub fieldNames: Array<string>;
    pub fieldTypes: Array<int>;

    pub fn create() {
        this.className = "";
        this.classId = 0;
        @cpp {
            this->fieldNames = std::vector<std::string>();
            this->fieldTypes = std::vector<int>();
        }
    }

    pub fn addField(name: string, typeCode: int) {
        @cpp {
            this->fieldNames.push_back(name);
            this->fieldTypes.push_back(typeCode);
        }
    }
}

// ============================
// SlateValue
// ============================

pub class SlateValue {
    pub valueType: int;
    pub intValue: int;
    pub floatValue: float;
    pub stringValue: string;
    pub boolValue: bool;
    pub objectId: int;
    pub arrayValue: Array<SlateValue>;

    pub fn create() {
        this.valueType = 0;  // VALUE_NULL
        this.intValue = 0;
        this.floatValue = 0.0;
        this.stringValue = "";
        this.boolValue = false;
        this.objectId = -1;
        @cpp {
            this->arrayValue = std::vector<SlateValue>();
        }
    }

    // Static factory methods
    pub static fn makeNull() -> SlateValue {
        let v = new SlateValue();
        v.valueType = 0;  // VALUE_NULL
        return v;
    }

    pub static fn makeInt(value: int) -> SlateValue {
        let v = new SlateValue();
        v.valueType = 1;  // INT
        v.intValue = value;
        return v;
    }

    pub static fn makeFloat(value: float) -> SlateValue {
        let v = new SlateValue();
        v.valueType = 2;  // FLOAT
        v.floatValue = value;
        return v;
    }

    pub static fn makeString(value: string) -> SlateValue {
        let v = new SlateValue();
        v.valueType = 3;  // STRING
        v.stringValue = value;
        return v;
    }

    pub static fn makeBool(value: bool) -> SlateValue {
        let v = new SlateValue();
        v.valueType = 4;  // BOOL
        v.boolValue = value;
        return v;
    }

    pub static fn makeRef(id: int) -> SlateValue {
        let v = new SlateValue();
        v.valueType = 5;  // OBJECT_REF
        v.objectId = id;
        return v;
    }

    pub static fn makeArray() -> SlateValue {
        let v = new SlateValue();
        v.valueType = 6;  // ARRAY
        @cpp {
            v.arrayValue = std::vector<SlateValue>();
        }
        return v;
    }

    pub fn push(val: SlateValue) {
        @cpp {
            this->arrayValue.push_back(val);
        }
    }
}

// ============================
// SlateObject
// ============================

pub class SlateObject {
    pub className: string;
    pub objectId: int;
    pub fields: Map<string, SlateValue>;

    pub fn create() {
        this.className = "";
        this.objectId = 0;
        @cpp {
            this->fields = std::unordered_map<std::string, SlateValue>();
        }
    }

    pub fn setField(name: string, value: SlateValue) {
        this.fields[name] = value;
    }

    pub fn getField(name: string) -> Option<SlateValue> {
        @cpp {
            auto it = this->fields.find(name);
            if (it != this->fields.end()) {
                return std::make_optional(it->second);
            }
            return std::nullopt;
        }
    }

    pub fn getInt(name: string) -> int {
        let opt = this.getField(name);
        if (isSome(opt)) {
            return unwrap(opt).intValue;
        }
        return 0;
    }

    pub fn getString(name: string) -> string {
        let opt = this.getField(name);
        if (isSome(opt)) {
            return unwrap(opt).stringValue;
        }
        return "";
    }

    pub fn getBool(name: string) -> bool {
        let opt = this.getField(name);
        if (isSome(opt)) {
            return unwrap(opt).boolValue;
        }
        return false;
    }
}

// ============================
// SlateConfig
// ============================

pub class SlateConfig {
    pub encrypted: bool;
    pub password: string;
    pub autoSave: bool;
    pub compressData: bool;

    pub fn create() {
        this.encrypted = false;
        this.password = "";
        this.autoSave = true;
        this.compressData = false;
    }

    pub fn openConfig() -> SlateConfig {
        return new SlateConfig();
    }

    pub fn openEncryptedConfig(password: string) -> SlateConfig {
        let cfg = new SlateConfig();
        cfg.encrypted = true;
        cfg.password = password;
        return cfg;
    }

    pub fn setAutoSave(enabled: bool) -> SlateConfig {
        this.autoSave = enabled;
        return this;
    }
}

// ============================
// BinaryWriter - FIX: Use int64_t for Crypto compatibility
// ============================

class BinaryWriter {
    // FIX: Changed from Array<int> to work with int64_t for Crypto
    pub data: Array<int>;

    pub fn create() {
        @cpp {
            this->data = std::vector<int>();
        }
    }

    pub fn writeU8(val: int) {
        @cpp { this->data.push_back(val & 0xFF); }
    }

    pub fn writeU32(val: int) {
        @cpp {
            this->writeU8(val & 0xFF);
            this->writeU8((val >> 8) & 0xFF);
            this->writeU8((val >> 16) & 0xFF);
            this->writeU8((val >> 24) & 0xFF);
        }
    }

    pub fn writeI64(val: int) {
        @cpp {
            for (int i = 0; i < 8; i++) {
                this->data.push_back((val >> (i * 8)) & 0xFF);
            }
        }
    }

    pub fn writeF64(val: float) {
        @cpp {
            double d = val;
            uint64_t bits;
            memcpy(&bits, &d, sizeof(bits));
            for (int i = 0; i < 8; i++) {
                this->data.push_back((bits >> (i * 8)) & 0xFF);
            }
        }
    }

    pub fn writeString(val: string) {
        let len = val.length();
        this.writeU32(len);
        @cpp {
            for (char c : val) {
                this->data.push_back(static_cast<unsigned char>(c));
            }
        }
    }

    pub fn writeValue(val: SlateValue) {
        this.writeU8(val.valueType);

        if (val.valueType == 1) {  // INT
            this.writeI64(val.intValue);
        } else if (val.valueType == 2) {  // FLOAT
            this.writeF64(val.floatValue);
        } else if (val.valueType == 3) {  // STRING
            this.writeString(val.stringValue);
        } else if (val.valueType == 4) {  // BOOL
            if (val.boolValue) {
                this.writeU8(1);
            } else {
                this.writeU8(0);
            }
        } else if (val.valueType == 5) {  // OBJECT_REF
            this.writeI64(val.objectId);
        } else if (val.valueType == 6) {  // ARRAY
            @cpp {
                int count = val.arrayValue.size();
                this->writeU32(count);
                for (int i = 0; i < count; i++) {
                    this->writeValue(val.arrayValue[i]);
                }
            }
        }
    }
    
    // FIX: Convert to int64_t vector for Crypto
    pub fn toInt64Vector() -> Array<int> {
        @cpp {
            std::vector<int64_t> result;
            result.reserve(this->data.size());
            for (int byte : this->data) {
                result.push_back(static_cast<int64_t>(byte));
            }
            return result;
        }
    }
}

// ============================
// BinaryReader
// ============================

class BinaryReader {
    pub data: Array<int>;
    pub pos: int;

    pub fn create() {
        @cpp {
            this->data = std::vector<int>();
        }
        this.pos = 0;
    }
    
    // FIX: Set data from int64_t vector
    pub fn setFromInt64Vector(int64Data: Array<int>) {
        @cpp {
            this->data.clear();
            for (int64_t byte : int64Data) {
                this->data.push_back(static_cast<int>(byte));
            }
            this->pos = 0;
        }
    }

    pub fn readU8() -> int {
        let v = this.data[this.pos];
        this.pos = this.pos + 1;
        return v;
    }

    pub fn readU32() -> int {
        let b0 = this.readU8();
        let b1 = this.readU8();
        let b2 = this.readU8();
        let b3 = this.readU8();
        @cpp { return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24); }
    }

    pub fn readI64() -> int {
        @cpp {
            int64_t v = 0;
            for (int i = 0; i < 8; i++) {
                v |= static_cast<int64_t>(this->data[this->pos++]) << (i * 8);
            }
            return v;
        }
    }

    pub fn readF64() -> float {
        @cpp {
            uint64_t bits = 0;
            for (int i = 0; i < 8; i++) {
                bits |= static_cast<uint64_t>(this->data[this->pos++]) << (i * 8);
            }
            double d;
            memcpy(&d, &bits, sizeof(d));
            return d;
        }
    }

    pub fn readString() -> string {
        let len = this.readU32();
        @cpp {
            std::string s;
            s.reserve(len);
            for (int i = 0; i < len; i++) {
                s += static_cast<char>(this->data[this->pos++]);
            }
            return s;
        }
    }

    pub fn readValue() -> SlateValue {
        let typeCode = this.readU8();
        let v = new SlateValue();
        v.valueType = typeCode;

        if (typeCode == 1) {  // INT
            v.intValue = this.readI64();
        } else if (typeCode == 2) {  // FLOAT
            v.floatValue = this.readF64();
        } else if (typeCode == 3) {  // STRING
            v.stringValue = this.readString();
        } else if (typeCode == 4) {  // BOOL
            v.boolValue = this.readU8() != 0;
        } else if (typeCode == 5) {  // OBJECT_REF
            v.objectId = this.readI64();
        } else if (typeCode == 6) {  // ARRAY
            @cpp {
                int count = this->readU32();
                for (int i = 0; i < count; i++) {
                    v.arrayValue.push_back(this->readValue());
                }
            }
        }

        return v;
    }
}

// ============================
// Export/Import Functions
// ============================

pub fn exportObject(obj: SlateObject, filename: string) {
    let writer = new BinaryWriter();
    
    // Write object metadata
    writer.writeString(obj.className);
    writer.writeI64(obj.objectId);
    
    // Write field count
    @cpp {
        writer.writeU32(obj.fields.size());
        
        // Write each field
        for (const auto& [key, value] : obj.fields) {
            writer.writeString(key);
            writer.writeValue(value);
        }
    }
    
    // Write to file
    @cpp {
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
}

pub fn importObject(filename: string) -> SlateObject {
    let reader = new BinaryReader();
    
    // Read file into reader
    @cpp {
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
    }
    
    reader.pos = 0;
    
    // Read object metadata
    let obj = new SlateObject();
    obj.className = reader.readString();
    obj.objectId = reader.readI64();
    
    // Read field count
    let fieldCount = reader.readU32();
    
    // Read each field
    @cpp {
        for (int i = 0; i < fieldCount; i++) {
            std::string key = reader.readString();
            SlateValue value = reader.readValue();
            obj.fields[key] = value;
        }
    }
    
    return obj;
}

pub fn exportObjectEncrypted(obj: SlateObject, filename: string, password: string) {
    let writer = new BinaryWriter();
    
    // Write object metadata
    writer.writeString(obj.className);
    writer.writeI64(obj.objectId);
    
    // Write field count
    @cpp {
        writer.writeU32(obj.fields.size());
        
        // Write each field
        for (const auto& [key, value] : obj.fields) {
            writer.writeString(key);
            writer.writeValue(value);
        }
    }
    
    // FIX: Convert to int64_t vector for Crypto compatibility
    @cpp {
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
}

pub fn importObjectEncrypted(filename: string, password: string) -> SlateObject {
    @cpp {
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
}


// ============================
// Schema Builder
// ============================

pub class Schema {
    pub schema: SlateSchema;

    pub fn create() {
        this.schema = new SlateSchema();
    }

    pub static fn define(name: string) -> Schema {
        let s = new Schema();
        s.schema.className = name;
        return s;
    }

    pub fn addInt(name: string) -> Schema {
        this.schema.addField(name, 1);  // INT = 1
        return this;
    }

    pub fn addFloat(name: string) -> Schema {
        this.schema.addField(name, 2);  // FLOAT = 2
        return this;
    }

    pub fn addString(name: string) -> Schema {
        this.schema.addField(name, 3);  // STRING = 3
        return this;
    }

    pub fn addBool(name: string) -> Schema {
        this.schema.addField(name, 4);  // BOOL = 4
        return this;
    }

    pub fn addRef(name: string) -> Schema {
        this.schema.addField(name, 5);  // OBJECT_REF = 5
        return this;
    }

    pub fn addArray(name: string) -> Schema {
        this.schema.addField(name, 6);  // ARRAY = 6
        return this;
    }

    pub fn build() -> SlateSchema {
        return this.schema;
    }
}
