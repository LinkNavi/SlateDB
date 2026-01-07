using Std.IO;
using Std.File;
using Std.String;
using Std.Array;
using Std.Map;
using Std.crypto;

// ============================
// ValueType holder (no top-level vars allowed)
// ============================

pub class ValueType {
    pub static NULL: int = 0;
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
    pub fieldNames: Array;
    pub fieldTypes: Array;

    pub fn create() {
        this.className = "";
        this.classId = 0;
        this.fieldNames = new Array();
        this.fieldTypes = new Array();
    }

    pub fn addField(name: string, typeCode: int) {
        this.fieldNames.push_back(name);
        this.fieldTypes.push_back(typeCode);
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
    pub arrayValue: Array;

    pub fn create() {
        this.valueType = ValueType.NULL;
        this.intValue = 0;
        this.floatValue = 0.0;
        this.stringValue = "";
        this.boolValue = false;
        this.objectId = -1;
        this.arrayValue = new Array();
    }

    pub fn makeNull() -> SlateValue {
        let v = new SlateValue();
        v.valueType = ValueType.NULL;
        return v;
    }

    pub fn makeInt(value: int) -> SlateValue {
        let v = new SlateValue();
        v.valueType = ValueType.INT;
        v.intValue = value;
        return v;
    }

    pub fn makeFloat(value: float) -> SlateValue {
        let v = new SlateValue();
        v.valueType = ValueType.FLOAT;
        v.floatValue = value;
        return v;
    }

    pub fn makeString(value: string) -> SlateValue {
        let v = new SlateValue();
        v.valueType = ValueType.STRING;
        v.stringValue = value;
        return v;
    }

    pub fn makeBool(value: bool) -> SlateValue {
        let v = new SlateValue();
        v.valueType = ValueType.BOOL;
        v.boolValue = value;
        return v;
    }

    pub fn makeRef(id: int) -> SlateValue {
        let v = new SlateValue();
        v.valueType = ValueType.OBJECT_REF;
        v.objectId = id;
        return v;
    }

    pub fn makeArray() -> SlateValue {
        let v = new SlateValue();
        v.valueType = ValueType.ARRAY;
        v.arrayValue = new Array();
        return v;
    }

    pub fn push(val: SlateValue) {
        this.arrayValue.push_back(val);
    }
}

// ============================
// SlateObject
// ============================

pub class SlateObject {
    pub className: string;
    pub objectId: int;
    pub fields: Map;

    pub fn create() {
        this.className = "";
        this.objectId = 0;
        this.fields = new Map();
    }

    pub fn setField(name: string, value: SlateValue) {
        this.fields[name] = value;
    }

    // returns Option<SlateValue> via C++ bridge (keeps original style)
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
    pub data: Array;

    pub fn create() {
        this.data = new Array();
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

        if (val.valueType == ValueType.INT) {
            this.writeI64(val.intValue);
        } else if (val.valueType == ValueType.FLOAT) {
            this.writeF64(val.floatValue);
        } else if (val.valueType == ValueType.STRING) {
            this.writeString(val.stringValue);
        } else if (val.valueType == ValueType.BOOL) {
            if (val.boolValue) {
                this.writeU8(1);
            } else {
                this.writeU8(0);
            }
        } else if (val.valueType == ValueType.OBJECT_REF) {
            this.writeI64(val.objectId);
        } else if (val.valueType == ValueType.ARRAY) {
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
    pub data: Array;
    pub pos: int;

    pub fn create() {
        this.data = new Array();
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

        if (typeCode == ValueType.INT) {
            v.intValue = this.readI64();
        } else if (typeCode == ValueType.FLOAT) {
            v.floatValue = this.readF64();
        } else if (typeCode == ValueType.STRING) {
            v.stringValue = this.readString();
        } else if (typeCode == ValueType.BOOL) {
            v.boolValue = this.readU8() != 0;
        } else if (typeCode == ValueType.OBJECT_REF) {
            v.objectId = this.readI64();
        } else if (typeCode == ValueType.ARRAY) {
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
    pub schema: SlateSchema;

    pub fn create() {
        this.schema = new SlateSchema();
    }

    pub fn define(name: string) -> Schema {
        let s = new Schema();
        s.schema.className = name;
        return s;
    }

    pub fn addInt(name: string) -> Schema {
        this.schema.addField(name, ValueType.INT);
        return this;
    }

    pub fn addFloat(name: string) -> Schema {
        this.schema.addField(name, ValueType.FLOAT);
        return this;
    }

    pub fn addString(name: string) -> Schema {
        this.schema.addField(name, ValueType.STRING);
        return this;
    }

    pub fn addBool(name: string) -> Schema {
        this.schema.addField(name, ValueType.BOOL);
        return this;
    }

    pub fn addRef(name: string) -> Schema {
        this.schema.addField(name, ValueType.OBJECT_REF);
        return this;
    }

    pub fn addArray(name: string) -> Schema {
        this.schema.addField(name, ValueType.ARRAY);
        return this;
    }

    pub fn build() -> SlateSchema {
        return this.schema;
    }
}
