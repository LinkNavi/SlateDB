// SlateDB - A lightweight object-oriented database for Magolor
using Std.IO;
using Std.File;
using Std.String;
using Std.Array;
using Std.Map;

// Using @cpp blocks for things that need precise C++ control

class SlateSchema {
    pub className: string;
    pub classId: int;
    pub fieldNames: Array<string>;
    pub fieldTypes: Array<int>;
    
    pub fn create() {
        className = "";
        classId = 0;
    }
    
    pub fn addField(name: string, typeCode: int) {
        fieldNames.push_back(name);
        fieldTypes.push_back(typeCode);
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
        valueType = 0;
        intValue = 0;
        floatValue = 0.0;
        stringValue = "";
        boolValue = false;
        objectId = -1;
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
        arrayValue.push_back(val);
    }
}

class SlateObject {
    pub className: string;
    pub objectId: int;
    pub fields: Map<string, SlateValue>;
    
    pub fn create() {
        className = "";
        objectId = 0;
    }
    
    pub fn setField(name: string, value: SlateValue) {
        fields[name] = value;
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
        pageSize = 4096;
        encrypted = false;
        password = "";
        autoFlush = true;
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

class SlateDB {
    pub isOpen: bool;
    pub filepath: string;
    pub nextObjectId: int;
    pub schemas: Map<string, SlateSchema>;
    pub objectCache: Map<int, SlateObject>;
    
    pub fn create() {
        isOpen = false;
        filepath = "";
        nextObjectId = 1;
    }
    
    pub fn open(path: string, cfg: SlateConfig) -> bool {
        filepath = path;
        
        let exists = File.exists(path);
        if (exists) {
            println("Opening existing database");
        } else {
            println("Creating new database");
        }
        isOpen = true;
        return true;
    }
    
    pub fn close() {
        if (isOpen) {
            println("Closing database");
            isOpen = false;
        }
    }
    
    pub fn registerSchema(schema: SlateSchema) {
        println($"Registering schema: {schema.className}");
        schemas[schema.className] = schema;
        println($"Schema count after register: {schemas.size()}");
    }
    
    pub fn getSchema(className: string) -> Option<SlateSchema> {
        println($"Looking for schema: {className}");
        println($"Current schema count: {schemas.size()}");
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
        let schemaOpt = getSchema(className);
        let hasSchema = isSome(schemaOpt);
        
        if (hasSchema) {
            let schema = unwrap(schemaOpt);
            let obj = new SlateObject();
            obj.className = className;
            obj.objectId = nextObjectId;
            nextObjectId = nextObjectId + 1;
            
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
        objectCache[obj.objectId] = obj;
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
        schema = new SlateSchema();
        tableName = "";
    }
    
    pub static fn forTable(name: string) -> SchemaBuilder {
        let builder = new SchemaBuilder();
        builder.tableName = name;
        builder.schema.className = name;
        return builder;
    }
    
    pub fn addInt(name: string) -> SchemaBuilder {
        schema.addField(name, 1);
        return this;
    }
    
    pub fn addString(name: string) -> SchemaBuilder {
        schema.addField(name, 3);
        return this;
    }
    
    pub fn addBool(name: string) -> SchemaBuilder {
        schema.addField(name, 4);
        return this;
    }
    
    pub fn addObject(name: string) -> SchemaBuilder {
        schema.addField(name, 5);
        return this;
    }
    
    pub fn addArray(name: string) -> SchemaBuilder {
        schema.addField(name, 6);
        return this;
    }
    
    pub fn build() -> SlateSchema {
        return schema;
    }
}
