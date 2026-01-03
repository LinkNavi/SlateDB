// SlateDB - A lightweight object-oriented database for Magolor
using Std.IO;
using Std.File;
using Std.String;
using Std.Array;
using Std.Map;

class SlateValue {
    pub valueType: int;
    pub intValue: int;
    pub floatValue: float;
    pub stringValue: string;
    pub boolValue: bool;
    pub objectId: int;  // Store object ID instead of object directly
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
        v.arrayValue = Array.create<SlateValue>();
        return v;
    }
    
    pub fn pushToArray(val: SlateValue) {
        Array.push<SlateValue>(arrayValue, val);
    }
}

class SlateObject {
    pub className: string;
    pub objectId: int;
    pub fields: Map<string, SlateValue>;
    
    pub fn create() {
        className = "";
        objectId = 0;
        fields = Map.create<string, SlateValue>();
    }
    
    pub fn setField(name: string, value: SlateValue) {
        Map.insert<string, SlateValue>(fields, name, value);
    }
    
    pub fn getField(name: string) -> Option<SlateValue> {
        return Map.get<string, SlateValue>(fields, name);
    }
}

class SlateSchema {
    pub className: string;
    pub classId: int;
    pub fieldNames: Array<string>;
    pub fieldTypes: Array<int>;
    
    pub fn create() {
        className = "";
        classId = 0;
        fieldNames = Array.create<string>();
        fieldTypes = Array.create<int>();
    }
    
    pub fn addField(name: string, typeCode: int) {
        Array.push<string>(fieldNames, name);
        Array.push<int>(fieldTypes, typeCode);
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
        schemas = Map.create<string, SlateSchema>();
        objectCache = Map.create<int, SlateObject>();
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
        Map.insert<string, SlateSchema>(schemas, schema.className, schema);
    }
    
    pub fn getSchema(className: string) -> Option<SlateSchema> {
        return Map.get<string, SlateSchema>(schemas, className);
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
            let fieldCount = Array.length<string>(schema.fieldNames);
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
        Map.insert<int, SlateObject>(objectCache, obj.objectId, obj);
        println("Saved object");
    }
    
    pub fn load(objectId: int) -> Option<SlateObject> {
        return Map.get<int, SlateObject>(objectCache, objectId);
    }
    
    pub fn query(className: string) -> Array<SlateObject> {
        let results = Array.create<SlateObject>();
        let allObjects = Map.values<int, SlateObject>(objectCache);
        
        let i = 0;
        let total = Array.length<SlateObject>(allObjects);
        while (i < total) {
            let obj = allObjects[i];
            if (obj.className == className) {
                Array.push<SlateObject>(results, obj);
            }
            i = i + 1;
        }
        
        return results;
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
