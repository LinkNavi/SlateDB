// src/slate/db.mg - FIXED VERSION
// Changed all field accesses to use "this." syntax for C++ compatibility

using Std.IO;
using Std.File;
using Std.String;
using Std.Array;
using Std.Map;

class SlateSchema {
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

class SlateValue {
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
    
    pub static fn createNull() -> SlateValue {
        let v = new SlateValue();
        v.valueType = 0;
        return v;
    }
    
    pub static fn createInt(value: int) -> SlateValue {
        let v = new SlateValue();
        v.valueType = 1;
        v.intValue = value;
        return v;
    }
    
    pub static fn createString(value: string) -> SlateValue {
        let v = new SlateValue();
        v.valueType = 3;
        v.stringValue = value;
        return v;
    }
    
    pub static fn createBool(value: bool) -> SlateValue {
        let v = new SlateValue();
        v.valueType = 4;
        v.boolValue = value;
        return v;
    }
    
    pub static fn createObjectRef(id: int) -> SlateValue {
        let v = new SlateValue();
        v.valueType = 5;
        v.objectId = id;
        return v;
    }
    
    pub static fn createArray() -> SlateValue {
        let v = new SlateValue();
        v.valueType = 6;
        return v;
    }
    
    pub fn pushToArray(val: SlateValue) {
        this.arrayValue.push_back(val);
    }
}

class SlateObject {
    pub className: string;
    pub objectId: int;
    pub fields: Map<string, SlateValue>;
    
    pub fn create() {
        this.className = "";
        this.objectId = 0;
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
}

class SlateConfig {
    pub pageSize: int;
    pub encrypted: bool;
    pub password: string;
    pub autoFlush: bool;
    
    pub fn create() {
        this.pageSize = 4096;
        this.encrypted = false;
        this.password = "";
        this.autoFlush = true;
    }
    
    pub static fn createDefault() -> SlateConfig {
        return new SlateConfig();
    }
    
    pub static fn createEncrypted(pwd: string) -> SlateConfig {
        let cfg = new SlateConfig();
        cfg.encrypted = true;
        cfg.password = pwd;
        return cfg;
    }
}

pub class SlateDB {
    pub isOpen: bool;
    pub filepath: string;
    pub nextObjectId: int;
    pub schemas: Map<string, SlateSchema>;
    pub objectCache: Map<int, SlateObject>;
    
    pub fn create() {
        this.isOpen = false;
        this.filepath = "";
        this.nextObjectId = 1;
    }
    
    pub fn open(path: string, cfg: SlateConfig) -> bool {
        this.filepath = path;
        
        let exists = File.exists(path);
        if (exists) {
            println("Opening existing database");
        } else {
            println("Creating new database");
        }
        this.isOpen = true;
        return true;
    }
    
    pub fn close() {
        if (this.isOpen) {
            println("Closing database");
            this.isOpen = false;
        }
    }
    
    pub fn registerSchema(schema: SlateSchema) {
        println($"Registering schema: {schema.className}");
        this.schemas[schema.className] = schema;

    }
    
    pub fn getSchema(className: string) -> Option<SlateSchema> {
        println($"Looking for schema: {className}");

        @cpp {
            auto it = this->schemas.find(className);
            if (it != this->schemas.end()) {
                std::cout << "Found schema!" << std::endl;
                return std::make_optional(it->second);
            }
            std::cout << "Schema not found!" << std::endl;
            return std::nullopt;
        }
    }
    
    pub fn createObject(className: string) -> Option<SlateObject> {
        let schemaOpt = this.getSchema(className);
        let hasSchema = isSome(schemaOpt);
        
        if (hasSchema) {
            let schema = unwrap(schemaOpt);
            let obj = new SlateObject();
            obj.className = className;
            obj.objectId = this.nextObjectId;
            this.nextObjectId = this.nextObjectId + 1;
            
            let i = 0;
            let fieldCount: int = schema.fieldNames.size();
            while (i < fieldCount) {
                let fieldName = schema.fieldNames[i];
                obj.setField(fieldName, SlateValue.createNull());
                i = i + 1;
            }
            
            return Some(obj);
        }
        
        return None;
    }
    
    pub fn save(obj: SlateObject) {
        this.objectCache[obj.objectId] = obj;
        println("Saved object");
    }
    
    pub fn load(objectId: int) -> Option<SlateObject> {
        @cpp {
            auto it = this->objectCache.find(objectId);
            if (it != this->objectCache.end()) {
                return std::make_optional(it->second);
            }
            return std::nullopt;
        }
    }
    
    pub fn query(className: string) -> Array<SlateObject> {
        @cpp {
            std::vector<SlateObject> results;
            for (auto& pair : this->objectCache) {
                if (pair.second.className == className) {
                    results.push_back(pair.second);
                }
            }
            return results;
        }
    }
}

class SchemaBuilder {
    pub schema: SlateSchema;
    pub tableName: string;
    
    pub fn create() {
        this.schema = new SlateSchema();
        this.tableName = "";
    }
    
    pub static fn forTable(name: string) -> SchemaBuilder {
        let builder = new SchemaBuilder();
        builder.tableName = name;
        builder.schema.className = name;
        return builder;
    }
    
    pub fn addInt(name: string) -> SchemaBuilder {
        this.schema.addField(name, 1);
        return this;
    }
    
    pub fn addString(name: string) -> SchemaBuilder {
        this.schema.addField(name, 3);
        return this;
    }
    
    pub fn addBool(name: string) -> SchemaBuilder {
        this.schema.addField(name, 4);
        return this;
    }
    
    pub fn addObject(name: string) -> SchemaBuilder {
        this.schema.addField(name, 5);
        return this;
    }
    
    pub fn addArray(name: string) -> SchemaBuilder {
        this.schema.addField(name, 6);
        return this;
    }
    
    pub fn build() -> SlateSchema {
        return this.schema;
    }
}
