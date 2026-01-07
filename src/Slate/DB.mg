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
// BinaryWriter
// ============================

class BinaryWriter {
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
