using Std.IO;
using Std.File;
using Std.String;
using Std.Array;
using Std.Map;
using Std.Crypto;

// ============================
// Value type codes
// ============================

pub const TYPE_NULL = 0;
pub const TYPE_INT = 1;
pub const TYPE_FLOAT = 2;
pub const TYPE_STRING = 3;
pub const TYPE_BOOL = 4;
pub const TYPE_OBJECT_REF = 5;
pub const TYPE_ARRAY = 6;

// ============================
// SlateSchema
// ============================

pub class SlateSchema {
    pub className;
    pub classId;
    pub fieldNames;
    pub fieldTypes;

    pub fn create() {
        this.className = "";
        this.classId = 0;
        this.fieldNames = new Array();
        this.fieldTypes = new Array();
    }

    pub fn addField(name, typeCode) {
        this.fieldNames.push_back(name);
        this.fieldTypes.push_back(typeCode);
    }
}

// ============================
// SlateValue
// ============================

pub class SlateValue {
    pub valueType;
    pub intValue;
    pub floatValue;
    pub stringValue;
    pub boolValue;
    pub objectId;
    pub arrayValue;

    pub fn create() {
        this.valueType = TYPE_NULL;
        this.intValue = 0;
        this.floatValue = 0.0;
        this.stringValue = "";
        this.boolValue = false;
        this.objectId = -1;
        this.arrayValue = new Array();
    }

    pub fn makeNull() {
        let v = new SlateValue();
        v.valueType = TYPE_NULL;
        return v;
    }

    pub fn makeInt(value) {
        let v = new SlateValue();
        v.valueType = TYPE_INT;
        v.intValue = value;
        return v;
    }

    pub fn makeFloat(value) {
        let v = new SlateValue();
        v.valueType = TYPE_FLOAT;
        v.floatValue = value;
        return v;
    }

    pub fn makeString(value) {
        let v = new SlateValue();
        v.valueType = TYPE_STRING;
        v.stringValue = value;
        return v;
    }

    pub fn makeBool(value) {
        let v = new SlateValue();
        v.valueType = TYPE_BOOL;
        v.boolValue = value;
        return v;
    }

    pub fn makeRef(id) {
        let v = new SlateValue();
        v.valueType = TYPE_OBJECT_REF;
        v.objectId = id;
        return v;
    }

    pub fn makeArray() {
        let v = new SlateValue();
        v.valueType = TYPE_ARRAY;
        v.arrayValue = new Array();
        return v;
    }

    pub fn push(val) {
        this.arrayValue.push_back(val);
    }
}

// ============================
// SlateObject
// ============================

pub class SlateObject {
    pub className;
    pub objectId;
    pub fields;

    pub fn create() {
        this.className = "";
        this.objectId = 0;
        this.fields = new Map();
    }

    pub fn setField(name, value) {
        this.fields[name] = value;
    }

    pub fn getField(name) {
        @cpp {
            auto it = this->fields.find(name);
            if (it != this->fields.end()) {
                return std::make_optional(it->second);
            }
            return std::nullopt;
        }
    }

    pub fn getInt(name) {
        let opt = this.getField(name);
        if (isSome(opt)) {
            return unwrap(opt).intValue;
        }
        return 0;
    }

    pub fn getString(name) {
        let opt = this.getField(name);
        if (isSome(opt)) {
            return unwrap(opt).stringValue;
        }
        return "";
    }

    pub fn getBool(name) {
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
    pub encrypted;
    pub password;
    pub autoSave;
    pub compressData;

    pub fn create() {
        this.encrypted = false;
        this.password = "";
        this.autoSave = true;
        this.compressData = false;
    }

    pub fn openConfig() {
        return new SlateConfig();
    }

    pub fn openEncryptedConfig(password) {
        let cfg = new SlateConfig();
        cfg.encrypted = true;
        cfg.password = password;
        return cfg;
    }

    pub fn setAutoSave(enabled) {
        this.autoSave = enabled;
        return this;
    }
}

// ============================
// BinaryWriter
// ============================

class BinaryWriter {
    pub data;

    pub fn create() {
        this.data = new Array();
    }

    pub fn writeU8(val) {
        @cpp { this->data.push_back(val & 0xFF); }
    }

    pub fn writeU32(val) {
        @cpp {
            this->writeU8(val & 0xFF);
            this->writeU8((val >> 8) & 0xFF);
            this->writeU8((val >> 16) & 0xFF);
            this->writeU8((val >> 24) & 0xFF);
        }
    }

    pub fn writeI64(val) {
        @cpp {
            for (int i = 0; i < 8; i++) {
                this->data.push_back((val >> (i * 8)) & 0xFF);
            }
        }
    }

    pub fn writeF64(val) {
        @cpp {
            double d = val;
            uint64_t bits;
            memcpy(&bits, &d, sizeof(bits));
            for (int i = 0; i < 8; i++) {
                this->data.push_back((bits >> (i * 8)) & 0xFF);
            }
        }
    }

    pub fn writeString(val) {
        let len = val.length();
        this.writeU32(len);
        @cpp {
            for (char c : val) {
                this->data.push_back(static_cast<unsigned char>(c));
            }
        }
    }

    pub fn writeValue(val) {
        this.writeU8(val.valueType);

        if (val.valueType == TYPE_INT) {
            this.writeI64(val.intValue);
        } else if (val.valueType == TYPE_FLOAT) {
            this.writeF64(val.floatValue);
        } else if (val.valueType == TYPE_STRING) {
            this.writeString(val.stringValue);
        } else if (val.valueType == TYPE_BOOL) {
           
if (val.boolValue) {
    this.writeU8(1);
} else {
    this.writeU8(0);
}
        } else if (val.valueType == TYPE_OBJECT_REF) {
            this.writeI64(val.objectId);
        } else if (val.valueType == TYPE_ARRAY) {
            let count = val.arrayValue.size();
            this.writeU32(count);
            let i = 0;
            while (i < count) {
                this.writeValue(val.arrayValue[i]);
                i = i + 1;
            }
        }
    }
}

// ============================
// BinaryReader
// ============================

class BinaryReader {
    pub data;
    pub pos;

    pub fn create() {
        this.data = new Array();
        this.pos = 0;
    }

    pub fn readU8() {
        let v = this.data[this.pos];
        this.pos = this.pos + 1;
        return v;
    }

    pub fn readU32() {
        let b0 = this.readU8();
        let b1 = this.readU8();
        let b2 = this.readU8();
        let b3 = this.readU8();
        @cpp { return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24); }
    }

    pub fn readI64() {
        @cpp {
            int64_t v = 0;
            for (int i = 0; i < 8; i++) {
                v |= static_cast<int64_t>(this->data[this->pos++]) << (i * 8);
            }
            return v;
        }
    }

    pub fn readF64() {
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

    pub fn readString() {
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

    pub fn readValue() {
        let typeCode = this.readU8();
        let v = new SlateValue();
        v.valueType = typeCode;

        if (typeCode == TYPE_INT) {
            v.intValue = this.readI64();
        } else if (typeCode == TYPE_FLOAT) {
            v.floatValue = this.readF64();
        } else if (typeCode == TYPE_STRING) {
            v.stringValue = this.readString();
        } else if (typeCode == TYPE_BOOL) {
            v.boolValue = this.readU8() != 0;
        } else if (typeCode == TYPE_OBJECT_REF) {
            v.objectId = this.readI64();
        } else if (typeCode == TYPE_ARRAY) {
            let count = this.readU32();
            let i = 0;
            while (i < count) {
                v.arrayValue.push_back(this.readValue());
                i = i + 1;
            }
        }

        return v;
    }
}

// ============================
// Schema Builder
// ============================

pub class Schema {
    pub schema;

    pub fn create() {
        this.schema = new SlateSchema();
    }

    pub fn define(name) {
        let s = new Schema();
        s.schema.className = name;
        return s;
    }

    pub fn addInt(name) {
        this.schema.addField(name, TYPE_INT);
        return this;
    }

    pub fn addFloat(name) {
        this.schema.addField(name, TYPE_FLOAT);
        return this;
    }

    pub fn addString(name) {
        this.schema.addField(name, TYPE_STRING);
        return this;
    }

    pub fn addBool(name) {
        this.schema.addField(name, TYPE_BOOL);
        return this;
    }

    pub fn addRef(name) {
        this.schema.addField(name, TYPE_OBJECT_REF);
        return this;
    }

    pub fn addArray(name) {
        this.schema.addField(name, TYPE_ARRAY);
        return this;
    }

    pub fn build() {
        return this.schema;
    }
}
