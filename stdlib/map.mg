// Std.Map - HashMap/Dictionary operations
// Key-value storage with O(1) average access
// NOTE: Map operations are template-based and handled by codegen
// These are convenience wrappers - actual Map<K,V> usage in user code
// generates proper std::unordered_map<K,V> types

// ============================================================================
// String-String Map operations (most common use case)
// ============================================================================

pub fn sizeStrStr(map: Map<string, string>) -> int {
    @cpp { return static_cast<int64_t>(map.size()); }
}

pub fn isEmptyStrStr(map: Map<string, string>) -> bool {
    @cpp { return map.empty(); }
}

pub fn clearStrStr(map: Map<string, string>) {
    @cpp { map.clear(); }
}

pub fn getStrStr(map: Map<string, string>, key: string) -> Option<string> {
    @cpp {
        auto it = map.find(key);
        if (it != map.end()) {
            return std::make_optional(it->second);
        }
        return std::nullopt;
    }
}

pub fn getOrStrStr(map: Map<string, string>, key: string, defaultVal: string) -> string {
    @cpp {
        auto it = map.find(key);
        if (it != map.end()) {
            return it->second;
        }
        return defaultVal;
    }
}

pub fn containsStrStr(map: Map<string, string>, key: string) -> bool {
    @cpp { return map.find(key) != map.end(); }
}

pub fn insertStrStr(map: Map<string, string>, key: string, value: string) {
    @cpp { map[key] = value; }
}

pub fn setStrStr(map: Map<string, string>, key: string, value: string) {
    @cpp { map[key] = value; }
}

pub fn removeStrStr(map: Map<string, string>, key: string) -> bool {
    @cpp {
        auto it = map.find(key);
        if (it != map.end()) {
            map.erase(it);
            return true;
        }
        return false;
    }
}

pub fn keysStrStr(map: Map<string, string>) -> Array<string> {
    @cpp {
        std::vector<std::string> result;
        result.reserve(map.size());
        for (const auto& pair : map) {
            result.push_back(pair.first);
        }
        return result;
    }
}

pub fn valuesStrStr(map: Map<string, string>) -> Array<string> {
    @cpp {
        std::vector<std::string> result;
        result.reserve(map.size());
        for (const auto& pair : map) {
            result.push_back(pair.second);
        }
        return result;
    }
}

// ============================================================================
// String-Int Map operations
// ============================================================================

pub fn sizeStrInt(map: Map<string, int>) -> int {
    @cpp { return static_cast<int64_t>(map.size()); }
}

pub fn isEmptyStrInt(map: Map<string, int>) -> bool {
    @cpp { return map.empty(); }
}

pub fn clearStrInt(map: Map<string, int>) {
    @cpp { map.clear(); }
}

pub fn getStrInt(map: Map<string, int>, key: string) -> Option<int> {
    @cpp {
        auto it = map.find(key);
        if (it != map.end()) {
            return std::make_optional(it->second);
        }
        return std::nullopt;
    }
}

pub fn getOrStrInt(map: Map<string, int>, key: string, defaultVal: int) -> int {
    @cpp {
        auto it = map.find(key);
        if (it != map.end()) {
            return it->second;
        }
        return defaultVal;
    }
}

pub fn containsStrInt(map: Map<string, int>, key: string) -> bool {
    @cpp { return map.find(key) != map.end(); }
}

pub fn insertStrInt(map: Map<string, int>, key: string, value: int) {
    @cpp { map[key] = value; }
}

pub fn setStrInt(map: Map<string, int>, key: string, value: int) {
    @cpp { map[key] = value; }
}

pub fn removeStrInt(map: Map<string, int>, key: string) -> bool {
    @cpp {
        auto it = map.find(key);
        if (it != map.end()) {
            map.erase(it);
            return true;
        }
        return false;
    }
}

pub fn keysStrInt(map: Map<string, int>) -> Array<string> {
    @cpp {
        std::vector<std::string> result;
        result.reserve(map.size());
        for (const auto& pair : map) {
            result.push_back(pair.first);
        }
        return result;
    }
}

pub fn valuesStrInt(map: Map<string, int>) -> Array<int> {
    @cpp {
        std::vector<int64_t> result;
        result.reserve(map.size());
        for (const auto& pair : map) {
            result.push_back(pair.second);
        }
        return result;
    }
}

pub fn incrementStrInt(map: Map<string, int>, key: string) {
    @cpp {
        map[key]++;
    }
}

pub fn decrementStrInt(map: Map<string, int>, key: string) {
    @cpp {
        map[key]--;
    }
}

// ============================================================================
// Int-Int Map operations
// ============================================================================

pub fn sizeIntInt(map: Map<int, int>) -> int {
    @cpp { return static_cast<int64_t>(map.size()); }
}

pub fn isEmptyIntInt(map: Map<int, int>) -> bool {
    @cpp { return map.empty(); }
}

pub fn getIntInt(map: Map<int, int>, key: int) -> Option<int> {
    @cpp {
        auto it = map.find(key);
        if (it != map.end()) {
            return std::make_optional(it->second);
        }
        return std::nullopt;
    }
}

pub fn getOrIntInt(map: Map<int, int>, key: int, defaultVal: int) -> int {
    @cpp {
        auto it = map.find(key);
        if (it != map.end()) {
            return it->second;
        }
        return defaultVal;
    }
}

pub fn containsIntInt(map: Map<int, int>, key: int) -> bool {
    @cpp { return map.find(key) != map.end(); }
}

pub fn insertIntInt(map: Map<int, int>, key: int, value: int) {
    @cpp { map[key] = value; }
}

pub fn removeIntInt(map: Map<int, int>, key: int) -> bool {
    @cpp {
        auto it = map.find(key);
        if (it != map.end()) {
            map.erase(it);
            return true;
        }
        return false;
    }
}

pub fn keysIntInt(map: Map<int, int>) -> Array<int> {
    @cpp {
        std::vector<int64_t> result;
        result.reserve(map.size());
        for (const auto& pair : map) {
            result.push_back(pair.first);
        }
        return result;
    }
}

pub fn valuesIntInt(map: Map<int, int>) -> Array<int> {
    @cpp {
        std::vector<int64_t> result;
        result.reserve(map.size());
        for (const auto& pair : map) {
            result.push_back(pair.second);
        }
        return result;
    }
}

// ============================================================================
// Int-String Map operations  
// ============================================================================

pub fn sizeIntStr(map: Map<int, string>) -> int {
    @cpp { return static_cast<int64_t>(map.size()); }
}

pub fn getIntStr(map: Map<int, string>, key: int) -> Option<string> {
    @cpp {
        auto it = map.find(key);
        if (it != map.end()) {
            return std::make_optional(it->second);
        }
        return std::nullopt;
    }
}

pub fn getOrIntStr(map: Map<int, string>, key: int, defaultVal: string) -> string {
    @cpp {
        auto it = map.find(key);
        if (it != map.end()) {
            return it->second;
        }
        return defaultVal;
    }
}

pub fn containsIntStr(map: Map<int, string>, key: int) -> bool {
    @cpp { return map.find(key) != map.end(); }
}

pub fn insertIntStr(map: Map<int, string>, key: int, value: string) {
    @cpp { map[key] = value; }
}

pub fn removeIntStr(map: Map<int, string>, key: int) -> bool {
    @cpp {
        auto it = map.find(key);
        if (it != map.end()) {
            map.erase(it);
            return true;
        }
        return false;
    }
}
