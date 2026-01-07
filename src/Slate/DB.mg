// src/slate/db.mg - SlateDB with AES-256-GCM encryption
// Binary serialization with secure password-based encryption

using Std.IO;
using Std.File;
using Std.String;
using Std.Array;
using Std.Map;
using Std.Crypto;

// Value type codes
pub const TYPE_NULL: int = 0;
pub const TYPE_INT: int = 1;
pub const TYPE_FLOAT: int = 2;
pub const TYPE_STRING: int = 3;
pub const TYPE_BOOL: int = 4;
pub const TYPE_OBJECT_REF: int = 5;
pub const TYPE_ARRAY: int = 6;

pub class SlateSchema {
    pub className: string;
    pub classId: int;
    pub fieldNames: Array<string>;
    pub fieldTypes: Array<int>;
    
    pub fn create() {
        this.className = "";
        this.classId = 0;
    }
    
    pub fn addField(name: string, typeCode: int) {
        this.fieldNames.push_back(name);
        this.fieldTypes.push_back(typeCode);
    }
}

pub class SlateValue {
    pub valueType: int;
    pub intValue: int;
    pub floatValue: float;
    pub stringValue: string;
    pub boolValue: bool;
    pub objectId: int;
    pub arrayValue: Array<SlateValue>;
    
    pub fn create() {
        this.valueType = 0;
        this.intValue = 0;
        this.floatValue = 0.0;
        this.stringValue = "";
        this.boolValue = false;
        this.objectId = -1;
    }
    
    pub static fn null() -> SlateValue {
        let v = new SlateValue();
        v.valueType = TYPE_NULL;
        return v;
    }
    
    pub static fn int(value: int) -> SlateValue {
        let v = new SlateValue();
        v.valueType = TYPE_INT;
        v.intValue = value;
        return v;
    }
    
    pub static fn float(value: float) -> SlateValue {
        let v = new SlateValue();
        v.valueType = TYPE_FLOAT;
        v.floatValue = value;
        return v;
    }
    
    pub static fn string(value: string) -> SlateValue {
        let v = new SlateValue();
        v.valueType = TYPE_STRING;
        v.stringValue = value;
        return v;
    }
    
    pub static fn bool(value: bool) -> SlateValue {
        let v = new SlateValue();
        v.valueType = TYPE_BOOL;
        v.boolValue = value;
        return v;
    }
    
    pub static fn ref(id: int) -> SlateValue {
        let v = new SlateValue();
        v.valueType = TYPE_OBJECT_REF;
        v.objectId = id;
        return v;
    }
    
    pub static fn array() -> SlateValue {
        let v = new SlateValue();
        v.valueType = TYPE_ARRAY;
        return v;
    }
    
    pub fn push(val: SlateValue) {
        this.arrayValue.push_back(val);
    }
}

pub class SlateObject {
    pub className: string;
    pub objectId: int;
    pub fields: Map<string, SlateValue>;
    
    pub fn create() {
        this.className = "";
        this.objectId = 0;
    }
    
    pub fn set(name: string, value: SlateValue) {
        this.fields[name] = value;
    }
    
    pub fn get(name: string) -> Option<SlateValue> {
        @cpp {
            auto it = this->fields.find(name);
            if (it != this->fields.end()) {
                return std::make_optional(it->second);
            }
            return std::nullopt;
        }
    }
    
    pub fn getInt(name: string) -> int {
        let opt = this.get(name);
        if (isSome(opt)) {
            return unwrap(opt).intValue;
        }
        return 0;
    }
    
    pub fn getString(name: string) -> string {
        let opt = this.get(name);
        if (isSome(opt)) {
            return unwrap(opt).stringValue;
        }
        return "";
    }
    
    pub fn getBool(name: string) -> bool {
        let opt = this.get(name);
        if (isSome(opt)) {
            return unwrap(opt).boolValue;
        }
        return false;
    }
}

// Database configuration
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
    
    pub static fn open() -> SlateConfig {
        return new SlateConfig();
    }
    
    pub static fn encrypted(password: string) -> SlateConfig {
        let cfg = new SlateConfig();
        cfg.encrypted = true;
        cfg.password = password;
        return cfg;
    }
    
    pub fn withAutoSave(enabled: bool) -> SlateConfig {
        this.autoSave = enabled;
        return this;
    }
}

// Binary serializer
class BinaryWriter {
    pub data: Array<int>;
    
    pub fn create() {}
    
    pub fn writeU8(val: int) {
        this.data.push_back(val & 0xFF);
    }
    
    pub fn writeU32(val: int) {
        this.writeU8(val & 0xFF);
        this.writeU8((val >> 8) & 0xFF);
        this.writeU8((val >> 16) & 0xFF);
        this.writeU8((val >> 24) & 0xFF);
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
        let len: int = val.length();
        this.writeU32(len);
        @cpp {
            for (char c : val) {
                this->data.push_back(static_cast<unsigned char>(c));
            }
        }
    }
    
    pub fn writeValue(val: SlateValue) {
        this.writeU8(val.valueType);
        
        if (val.valueType == TYPE_INT) {
            this.writeI64(val.intValue);
        } else if (val.valueType == TYPE_FLOAT) {
            this.writeF64(val.floatValue);
        } else if (val.valueType == TYPE_STRING) {
            this.writeString(val.stringValue);
        } else if (val.valueType == TYPE_BOOL) {
            this.writeU8(val.boolValue ? 1 : 0);
        } else if (val.valueType == TYPE_OBJECT_REF) {
            this.writeI64(val.objectId);
        } else if (val.valueType == TYPE_ARRAY) {
            let arrLen: int = val.arrayValue.size();
            this.writeU32(arrLen);
            let i = 0;
            while (i < arrLen) {
                this.writeValue(val.arrayValue[i]);
                i = i + 1;
            }
        }
    }
}

// Binary deserializer
class BinaryReader {
    pub data: Array<int>;
    pub pos: int;
    
    pub fn create() {
        this.pos = 0;
    }
    
    pub fn readU8() -> int {
        let val = this.data[this.pos];
        this.pos = this.pos + 1;
        return val;
    }
    
    pub fn readU32() -> int {
        let b0 = this.readU8();
        let b1 = this.readU8();
        let b2 = this.readU8();
        let b3 = this.readU8();
        return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
    }
    
    pub fn readI64() -> int {
        @cpp {
            int64_t val = 0;
            for (int i = 0; i < 8; i++) {
                val |= static_cast<int64_t>(this->data[this->pos++]) << (i * 8);
            }
            return val;
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
        let val = new SlateValue();
        val.valueType = typeCode;
        
        if (typeCode == TYPE_INT) {
            val.intValue = this.readI64();
        } else if (typeCode == TYPE_FLOAT) {
            val.floatValue = this.readF64();
        } else if (typeCode == TYPE_STRING) {
            val.stringValue = this.readString();
        } else if (typeCode == TYPE_BOOL) {
            val.boolValue = this.readU8() != 0;
        } else if (typeCode == TYPE_OBJECT_REF) {
            val.objectId = this.readI64();
        } else if (typeCode == TYPE_ARRAY) {
            let arrLen = this.readU32();
            let i = 0;
            while (i < arrLen) {
                val.arrayValue.push_back(this.readValue());
                i = i + 1;
            }
        }
        
        return val;
    }
}

// Main database class
pub class SlateDB {
    pub isOpen: bool;
    pub filepath: string;
    pub nextObjectId: int;
    pub schemas: Map<string, SlateSchema>;
    pub objects: Map<int, SlateObject>;
    pub config: SlateConfig;
    pub dirty: bool;
    
    pub fn create() {
        this.isOpen = false;
        this.filepath = "";
        this.nextObjectId = 1;
        this.config = new SlateConfig();
        this.dirty = false;
    }
    
    // Open or create database
    pub static fn open(path: string) -> SlateDB {
        let db = new SlateDB();
        db.filepath = path;
        db.config = SlateConfig.open();
        db.loadFromFile();
        db.isOpen = true;
        return db;
    }
    
    // Open encrypted database
    pub static fn openEncrypted(path: string, password: string) -> SlateDB {
        let db = new SlateDB();
        db.filepath = path;
        db.config = SlateConfig.encrypted(password);
        db.loadFromFile();
        db.isOpen = true;
        return db;
    }
    
    // Create new encrypted database
    pub static fn createEncrypted(path: string, password: string) -> SlateDB {
        let db = new SlateDB();
        db.filepath = path;
        db.config = SlateConfig.encrypted(password);
        db.isOpen = true;
        db.dirty = true;
        return db;
    }
    
    pub fn close() {
        if (this.isOpen) {
            if (this.dirty) {
                this.flush();
            }
            this.isOpen = false;
        }
    }
    
    // Register a schema
    pub fn schema(s: SlateSchema) {
        this.schemas[s.className] = s;
        this.dirty = true;
        if (this.config.autoSave) {
            this.flush();
        }
    }
    
    // Create new object
    pub fn new(className: string) -> Option<SlateObject> {
        @cpp {
            auto it = this->schemas.find(className);
            if (it == this->schemas.end()) {
                return std::nullopt;
            }
            
            auto& schema = it->second;
            auto obj = std::make_shared<SlateObject>();
            obj->className = className;
            obj->objectId = this->nextObjectId++;
            
            // Initialize fields with null
            for (size_t i = 0; i < schema.fieldNames.size(); i++) {
                SlateValue nullVal;
                nullVal.valueType = 0;
                obj->fields[schema.fieldNames[i]] = nullVal;
            }
            
            this->objects[obj->objectId] = *obj;
            this->dirty = true;
            
            if (this->config.autoSave) {
                // Will be flushed later
            }
            
            return std::make_optional(*obj);
        }
    }
    
    // Save object
    pub fn save(obj: SlateObject) {
        this.objects[obj.objectId] = obj;
        this.dirty = true;
        if (this.config.autoSave) {
            this.flush();
        }
    }
    
    // Load object by ID
    pub fn get(objectId: int) -> Option<SlateObject> {
        @cpp {
            auto it = this->objects.find(objectId);
            if (it != this->objects.end()) {
                return std::make_optional(it->second);
            }
            return std::nullopt;
        }
    }
    
    // Query all objects of a class
    pub fn find(className: string) -> Array<SlateObject> {
        @cpp {
            std::vector<SlateObject> results;
            for (auto& pair : this->objects) {
                if (pair.second.className == className) {
                    results.push_back(pair.second);
                }
            }
            return results;
        }
    }
    
    // Delete object
    pub fn delete(objectId: int) -> bool {
        @cpp {
            auto it = this->objects.find(objectId);
            if (it != this->objects.end()) {
                this->objects.erase(it);
                this->dirty = true;
                return true;
            }
            return false;
        }
    }
    
    // Flush to disk
    pub fn flush() {
        if (!this.dirty) {
            return;
        }
        
        let writer = new BinaryWriter();
        
        // Magic + version
        writer.writeU32(0x534C4154);  // "SLAT"
        writer.writeU32(1);            // version
        
        // Write schemas
        @cpp {
            writer.writeU32(this->schemas.size());
            for (auto& pair : this->schemas) {
                writer.writeString(pair.first);
                writer.writeU32(pair.second.fieldNames.size());
                for (size_t i = 0; i < pair.second.fieldNames.size(); i++) {
                    writer.writeString(pair.second.fieldNames[i]);
                    writer.writeU8(pair.second.fieldTypes[i]);
                }
            }
        }
        
        // Write objects
        @cpp {
            writer.writeU32(this->objects.size());
            for (auto& pair : this->objects) {
                writer.writeI64(pair.first);
                writer.writeString(pair.second.className);
                writer.writeU32(pair.second.fields.size());
                for (auto& field : pair.second.fields) {
                    writer.writeString(field.first);
                    writer.writeValue(field.second);
                }
            }
        }
        
        // Write next ID
        writer.writeI64(this.nextObjectId);
        
        // Encrypt if needed
        if (this.config.encrypted) {
            let encrypted = Crypto.encrypt(writer.data, this.config.password);
            if (encrypted.success) {
                this.writeEncrypted(encrypted);
            } else {
                println($"Encryption failed: {encrypted.error}");
            }
        } else {
            this.writeRaw(writer.data);
        }
        
        this.dirty = false;
    }
    
    fn writeRaw(data: Array<int>) {
        @cpp {
            std::ofstream file(this->filepath, std::ios::binary);
            if (file) {
                for (auto b : data) {
                    file.put(static_cast<char>(b));
                }
            }
        }
    }
    
    fn writeEncrypted(result: CryptoResult) {
        @cpp {
            std::ofstream file(this->filepath, std::ios::binary);
            if (!file) return;
            
            // Write header: SLTX (encrypted slate)
            file.put('S'); file.put('L'); file.put('T'); file.put('X');
            
            // Write salt (16 bytes)
            for (auto b : result.salt) file.put(static_cast<char>(b));
            
            // Write IV (12 bytes)
            for (auto b : result.iv) file.put(static_cast<char>(b));
            
            // Write tag (16 bytes)
            for (auto b : result.tag) file.put(static_cast<char>(b));
            
            // Write data length
            uint32_t len = result.data.size();
            file.put(len & 0xFF);
            file.put((len >> 8) & 0xFF);
            file.put((len >> 16) & 0xFF);
            file.put((len >> 24) & 0xFF);
            
            // Write encrypted data
            for (auto b : result.data) file.put(static_cast<char>(b));
        }
    }
    
    fn loadFromFile() {
        if (!File.exists(this.filepath)) {
            return;
        }
        
        @cpp {
            std::ifstream file(this->filepath, std::ios::binary);
            if (!file) return;
            
            // Read magic
            char magic[4];
            file.read(magic, 4);
            
            std::vector<int64_t> data;
            
            if (magic[0] == 'S' && magic[1] == 'L' && magic[2] == 'T' && magic[3] == 'X') {
                // Encrypted file
                if (!this->config.encrypted) {
                    std::cerr << "Database is encrypted but no password provided" << std::endl;
                    return;
                }
                
                CryptoResult encrypted;
                encrypted.success = true;
                
                // Read salt
                encrypted.salt.resize(16);
                for (int i = 0; i < 16; i++) {
                    encrypted.salt[i] = static_cast<unsigned char>(file.get());
                }
                
                // Read IV
                encrypted.iv.resize(12);
                for (int i = 0; i < 12; i++) {
                    encrypted.iv[i] = static_cast<unsigned char>(file.get());
                }
                
                // Read tag
                encrypted.tag.resize(16);
                for (int i = 0; i < 16; i++) {
                    encrypted.tag[i] = static_cast<unsigned char>(file.get());
                }
                
                // Read data length
                uint32_t len = 0;
                len |= static_cast<unsigned char>(file.get());
                len |= static_cast<unsigned char>(file.get()) << 8;
                len |= static_cast<unsigned char>(file.get()) << 16;
                len |= static_cast<unsigned char>(file.get()) << 24;
                
                // Read encrypted data
                encrypted.data.resize(len);
                for (uint32_t i = 0; i < len; i++) {
                    encrypted.data[i] = static_cast<unsigned char>(file.get());
                }
                
                // Decrypt
                auto decrypted = Crypto_decrypt(encrypted, this->config.password);
                if (!decrypted.success) {
                    std::cerr << "Decryption failed: " << decrypted.error << std::endl;
                    return;
                }
                
                data = decrypted.data;
            } else {
                // Unencrypted - magic is first 4 bytes of data
                data.push_back(static_cast<unsigned char>(magic[0]));
                data.push_back(static_cast<unsigned char>(magic[1]));
                data.push_back(static_cast<unsigned char>(magic[2]));
                data.push_back(static_cast<unsigned char>(magic[3]));
                
                char c;
                while (file.get(c)) {
                    data.push_back(static_cast<unsigned char>(c));
                }
            }
            
            // Parse data
            BinaryReader reader;
            reader.data = data;
            reader.pos = 0;
            
            // Verify magic
            uint32_t fileMagic = reader.readU32();
            if (fileMagic != 0x534C4154) {
                std::cerr << "Invalid database file" << std::endl;
                return;
            }
            
            uint32_t version = reader.readU32();
            
            // Read schemas
            uint32_t schemaCount = reader.readU32();
            for (uint32_t i = 0; i < schemaCount; i++) {
                SlateSchema schema;
                schema.className = reader.readString();
                uint32_t fieldCount = reader.readU32();
                for (uint32_t j = 0; j < fieldCount; j++) {
                    std::string fieldName = reader.readString();
                    int fieldType = reader.readU8();
                    schema.fieldNames.push_back(fieldName);
                    schema.fieldTypes.push_back(fieldType);
                }
                this->schemas[schema.className] = schema;
            }
            
            // Read objects
            uint32_t objectCount = reader.readU32();
            for (uint32_t i = 0; i < objectCount; i++) {
                int64_t objId = reader.readI64();
                SlateObject obj;
                obj.objectId = objId;
                obj.className = reader.readString();
                uint32_t fieldCount = reader.readU32();
                for (uint32_t j = 0; j < fieldCount; j++) {
                    std::string fieldName = reader.readString();
                    SlateValue val = reader.readValue();
                    obj.fields[fieldName] = val;
                }
                this->objects[objId] = obj;
            }
            
            // Read next ID
            this->nextObjectId = reader.readI64();
        }
    }
}

// Schema builder for fluent API
pub class Schema {
    pub schema: SlateSchema;
    
    pub fn create() {
        this.schema = new SlateSchema();
    }
    
    pub static fn define(name: string) -> Schema {
        let builder = new Schema();
        builder.schema.className = name;
        return builder;
    }
    
    pub fn int(name: string) -> Schema {
        this.schema.addField(name, TYPE_INT);
        return this;
    }
    
    pub fn float(name: string) -> Schema {
        this.schema.addField(name, TYPE_FLOAT);
        return this;
    }
    
    pub fn string(name: string) -> Schema {
        this.schema.addField(name, TYPE_STRING);
        return this;
    }
    
    pub fn bool(name: string) -> Schema {
        this.schema.addField(name, TYPE_BOOL);
        return this;
    }
    
    pub fn ref(name: string) -> Schema {
        this.schema.addField(name, TYPE_OBJECT_REF);
        return this;
    }
    
    pub fn array(name: string) -> Schema {
        this.schema.addField(name, TYPE_ARRAY);
        return this;
    }
    
    pub fn build() -> SlateSchema {
        return this.schema;
    }
}
