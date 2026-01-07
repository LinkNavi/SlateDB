// Auto-generated C++ code from Magolor
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

class SlateSchema;
class SlateValue;
class SlateObject;
class SlateConfig;
class SlateDB;
class SchemaBuilder;

class SlateSchema {
public:
    std::string className;
    int classId;
    std::vector<std::string> fieldNames;
    std::vector<int> fieldTypes;
    void create() {
        this->className = std::string("");
        this->classId = 0;
    }
    void addField(std::string name, int typeCode) {
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
    }
    static     SlateValue createNull() {
        auto v = SlateValue();
        v.valueType = 0;
        return v;
    }
    static     SlateValue createInt(int value) {
        auto v = SlateValue();
        v.valueType = 1;
        v.intValue = value;
        return v;
    }
    static     SlateValue createString(std::string value) {
        auto v = SlateValue();
        v.valueType = 3;
        v.stringValue = value;
        return v;
    }
    static     SlateValue createBool(bool value) {
        auto v = SlateValue();
        v.valueType = 4;
        v.boolValue = value;
        return v;
    }
    static     SlateValue createObjectRef(int id) {
        auto v = SlateValue();
        v.valueType = 5;
        v.objectId = id;
        return v;
    }
    static     SlateValue createArray() {
        auto v = SlateValue();
        v.valueType = 6;
        return v;
    }
    void pushToArray(SlateValue val) {
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
    int pageSize;
    bool encrypted;
    std::string password;
    bool autoFlush;
    void create() {
        this->pageSize = 4096;
        this->encrypted = false;
        this->password = std::string("");
        this->autoFlush = true;
    }
    static     SlateConfig createDefault() {
        return SlateConfig();
    }
    static     SlateConfig createEncrypted(std::string pwd) {
        auto cfg = SlateConfig();
        cfg.encrypted = true;
        cfg.password = pwd;
        return cfg;
    }
};

// Auto-generated print support
inline std::ostream& operator<<(std::ostream& os, const SlateConfig& obj) {
    os << "SlateConfig { ";
    os << "pageSize: " << obj.pageSize;
    os << ", ";
    os << "encrypted: " << obj.encrypted;
    os << ", ";
    os << "password: " << obj.password;
    os << ", ";
    os << "autoFlush: " << obj.autoFlush;
    os << " }";
    return os;
}

class SlateDB {
public:
    bool isOpen;
    std::string filepath;
    int nextObjectId;
    std::unordered_map<std::string, SlateSchema> schemas;
    std::unordered_map<int, SlateObject> objectCache;
    void create() {
        this->isOpen = false;
        this->filepath = std::string("");
        this->nextObjectId = 1;
    }
    bool open(std::string path, SlateConfig cfg) {
        this->filepath = path;
        auto exists = File::exists(path);
        if (exists) {
            println(std::string("Opening existing database"));
        }
        else {
            println(std::string("Creating new database"));
        }
        this->isOpen = true;
        return true;
    }
    void close() {
        if (this->isOpen) {
            println(std::string("Closing database"));
            this->isOpen = false;
        }
    }
    void registerSchema(SlateSchema schema) {
        println((std::string("Registering schema: ") + mg_to_string(schema.className)));
        this->schemas[schema.className] = schema;
    }
    std::optional<SlateSchema> getSchema(std::string className) {
        println((std::string("Looking for schema: ") + mg_to_string(className)));
        // Inline C++ code:

            auto it = this->schemas.find(className);
            if (it != this->schemas.end()) {
                std::cout << "Found schema!" << std::endl;
                return std::make_optional(it->second);
            }
            std::cout << "Schema not found!" << std::endl;
            return std::nullopt;
        
    }
    std::optional<SlateObject> createObject(std::string className) {
        auto schemaOpt = this->getSchema(className);
        auto hasSchema = isSome(schemaOpt);
        if (hasSchema) {
            auto schema = unwrap(schemaOpt);
            auto obj = SlateObject();
            obj.className = className;
            obj.objectId = this->nextObjectId;
            this->nextObjectId = (this->nextObjectId + 1);
            auto i = 0;
            int fieldCount = schema.fieldNames.size();
            while ((i < fieldCount)) {
                auto fieldName = schema.fieldNames[i];
                obj.setField(fieldName, SlateValue::createNull());
                i = (i + 1);
            }
            return std::make_optional(obj);
        }
        return std::nullopt;
    }
    void save(SlateObject obj) {
        this->objectCache[obj.objectId] = obj;
        println(std::string("Saved object"));
    }
    std::optional<SlateObject> load(int objectId) {
        // Inline C++ code:

            auto it = this->objectCache.find(objectId);
            if (it != this->objectCache.end()) {
                return std::make_optional(it->second);
            }
            return std::nullopt;
        
    }
    std::vector<SlateObject> query(std::string className) {
        // Inline C++ code:

            std::vector<SlateObject> results;
            for (auto& pair : this->objectCache) {
                if (pair.second.className == className) {
                    results.push_back(pair.second);
                }
            }
            return results;
        
    }
};

// Auto-generated print support
inline std::ostream& operator<<(std::ostream& os, const SlateDB& obj) {
    os << "SlateDB { ";
    os << "isOpen: " << obj.isOpen;
    os << ", ";
    os << "filepath: " << obj.filepath;
    os << ", ";
    os << "nextObjectId: " << obj.nextObjectId;
    os << " }";
    return os;
}

class SchemaBuilder {
public:
    SlateSchema schema;
    std::string tableName;
    void create() {
        this->schema = SlateSchema();
        this->tableName = std::string("");
    }
    static     SchemaBuilder forTable(std::string name) {
        auto builder = SchemaBuilder();
        builder.tableName = name;
        builder.schema.className = name;
        return builder;
    }
    SchemaBuilder addInt(std::string name) {
        this->schema.addField(name, 1);
        return *this;
    }
    SchemaBuilder addString(std::string name) {
        this->schema.addField(name, 3);
        return *this;
    }
    SchemaBuilder addBool(std::string name) {
        this->schema.addField(name, 4);
        return *this;
    }
    SchemaBuilder addObject(std::string name) {
        this->schema.addField(name, 5);
        return *this;
    }
    SchemaBuilder addArray(std::string name) {
        this->schema.addField(name, 6);
        return *this;
    }
    SlateSchema build() {
        return this->schema;
    }
};

// Auto-generated print support
inline std::ostream& operator<<(std::ostream& os, const SchemaBuilder& obj) {
    os << "SchemaBuilder { ";
    os << "tableName: " << obj.tableName;
    os << " }";
    return os;
}

SlateDB setupSchemas(SlateDB db);
SlateDB createSampleData(SlateDB db);
SlateDB displayAllPosts(SlateDB db);
SlateDB testEmbeddedObjects(SlateDB db);

SlateDB setupSchemas(SlateDB db) {
    println(std::string("Setting up schemas..."));
    auto userSchema = SchemaBuilder::forTable(std::string("User")).addInt(std::string("id")).addString(std::string("username")).addString(std::string("email")).addInt(std::string("posts_count")).build();
    db.registerSchema(userSchema);
    println(std::string("  ✓ User schema registered"));
    auto postSchema = SchemaBuilder::forTable(std::string("Post")).addInt(std::string("id")).addString(std::string("title")).addString(std::string("content")).addObject(std::string("author")).addArray(std::string("tags")).addInt(std::string("likes")).build();
    db.registerSchema(postSchema);
    println(std::string("  ✓ Post schema registered"));
    auto commentSchema = SchemaBuilder::forTable(std::string("Comment")).addInt(std::string("id")).addString(std::string("text")).addObject(std::string("author")).addInt(std::string("post_id")).build();
    db.registerSchema(commentSchema);
    println(std::string("  ✓ Comment schema registered\n"));
    return db;
}

SlateDB createSampleData(SlateDB db) {
    println(std::string("Creating sample data..."));
    auto userOpt = db.createObject(std::string("User"));
    if (isNone(userOpt)) {
        println(std::string("Failed to create user object"));
        return db;
    }
    auto user = unwrap(userOpt);
    user.setField(std::string("id"), SlateValue::createInt(1));
    user.setField(std::string("username"), SlateValue::createString(std::string("alice")));
    user.setField(std::string("email"), SlateValue::createString(std::string("alice@example.com")));
    user.setField(std::string("posts_count"), SlateValue::createInt(0));
    db.save(user);
    println(std::string("  ✓ Created user: alice"));
    auto user2Opt = db.createObject(std::string("User"));
    auto user2 = unwrap(user2Opt);
    user2.setField(std::string("id"), SlateValue::createInt(2));
    user2.setField(std::string("username"), SlateValue::createString(std::string("bob")));
    user2.setField(std::string("email"), SlateValue::createString(std::string("bob@example.com")));
    user2.setField(std::string("posts_count"), SlateValue::createInt(0));
    db.save(user2);
    println(std::string("  ✓ Created user: bob"));
    auto postOpt = db.createObject(std::string("Post"));
    auto post = unwrap(postOpt);
    post.setField(std::string("id"), SlateValue::createInt(1));
    post.setField(std::string("title"), SlateValue::createString(std::string("Getting Started with SlateDB")));
    post.setField(std::string("content"), SlateValue::createString(std::string("SlateDB is an amazing object database...")));
    post.setField(std::string("author"), SlateValue::createObjectRef(user.objectId));
    auto tags = SlateValue::createArray();
    tags.pushToArray(SlateValue::createString(std::string("database")));
    tags.pushToArray(SlateValue::createString(std::string("tutorial")));
    tags.pushToArray(SlateValue::createString(std::string("magolor")));
    post.setField(std::string("tags"), tags);
    post.setField(std::string("likes"), SlateValue::createInt(42));
    db.save(post);
    println(std::string("  ✓ Created post with embedded author"));
    auto post2Opt = db.createObject(std::string("Post"));
    auto post2 = unwrap(post2Opt);
    post2.setField(std::string("id"), SlateValue::createInt(2));
    post2.setField(std::string("title"), SlateValue::createString(std::string("Advanced SlateDB Patterns")));
    post2.setField(std::string("content"), SlateValue::createString(std::string("Let's explore advanced features...")));
    post2.setField(std::string("author"), SlateValue::createObjectRef(user2.objectId));
    auto tags2 = SlateValue::createArray();
    tags2.pushToArray(SlateValue::createString(std::string("database")));
    tags2.pushToArray(SlateValue::createString(std::string("advanced")));
    post2.setField(std::string("tags"), tags2);
    post2.setField(std::string("likes"), SlateValue::createInt(128));
    db.save(post2);
    println(std::string("  ✓ Created second post\n"));
    return db;
}

SlateDB displayAllPosts(SlateDB db) {
    println(std::string("=== All Blog Posts ===\n"));
    auto posts = db.query(std::string("Post"));
    int count = posts.size();
    println((std::string("Found ") + mg_to_string(count) + std::string(" posts:\n")));
    auto i = 0;
    while ((i < count)) {
        auto post = posts[i];
        auto titleOpt = post.getField(std::string("title"));
        auto title = std::string("Untitled");
        if (isSome(titleOpt)) {
            auto titleVal = unwrap(titleOpt);
            title = titleVal.stringValue;
        }
        auto likesOpt = post.getField(std::string("likes"));
        auto likes = 0;
        if (isSome(likesOpt)) {
            auto likesVal = unwrap(likesOpt);
            likes = likesVal.intValue;
        }
        auto authorOpt = post.getField(std::string("author"));
        auto authorName = std::string("Unknown");
        if (isSome(authorOpt)) {
            auto authorVal = unwrap(authorOpt);
            auto authorId = authorVal.objectId;
            authorName = (std::string("User #") + mg_to_string(authorId));
        }
        println((std::string("📝 ") + mg_to_string(title)));
        println((std::string("   Author: ") + mg_to_string(authorName)));
        println((std::string("   Likes: ") + mg_to_string(likes)));
        auto tagsOpt = post.getField(std::string("tags"));
        if (isSome(tagsOpt)) {
            auto tagsVal = unwrap(tagsOpt);
            auto tagArray = tagsVal.arrayValue;
            int tagCount = tagArray.size();
            print(std::string("   Tags: "));
            auto j = 0;
            while ((j < tagCount)) {
                auto tag = tagArray[j];
                print(tag.stringValue);
                if ((j < (tagCount - 1))) {
                    print(std::string(", "));
                }
                j = (j + 1);
            }
            println(std::string(""));
        }
        println(std::string(""));
        i = (i + 1);
    }
    return db;
}

SlateDB testEmbeddedObjects(SlateDB db) {
    println(std::string("=== Testing Nested Objects ===\n"));
    auto innerUser = db.createObject(std::string("User"));
    auto user = unwrap(innerUser);
    user.setField(std::string("username"), SlateValue::createString(std::string("nested_user")));
    auto middlePost = db.createObject(std::string("Post"));
    auto post = unwrap(middlePost);
    post.setField(std::string("title"), SlateValue::createString(std::string("Nested Post")));
    post.setField(std::string("author"), SlateValue::createObjectRef(user.objectId));
    auto outerComment = db.createObject(std::string("Comment"));
    auto comment = unwrap(outerComment);
    comment.setField(std::string("text"), SlateValue::createString(std::string("Great post!")));
    comment.setField(std::string("author"), SlateValue::createObjectRef(user.objectId));
    comment.setField(std::string("post_id"), SlateValue::createInt(1));
    db.save(comment);
    println(std::string("✓ Created nested object structure:"));
    println(std::string("  Comment -> Author (User)"));
    println(std::string("  Comment -> Post -> Author (User)"));
    println(std::string("\nNested objects allow you to create rich, interconnected data!"));
    return db;
}

int main() {
    println(std::string("=== SlateDB Blog Example ===\n"));
    auto config = SlateConfig::createEncrypted(std::string("my-secret-password-123"));
    config.pageSize = 8192;
    auto db = SlateDB();
    auto opened = db.open(std::string("blog.slatedb"), config);
    if ((!opened)) {
        println(std::string("Failed to open database!"));
        return 1;
    }
    println(std::string("✓ Database opened successfully\n"));
    db = setupSchemas(db);
    db = createSampleData(db);
    db = displayAllPosts(db);
    db = testEmbeddedObjects(db);
    db.close();
    println(std::string("\n✓ Database closed"));
    return 0;
}

