using Std.IO;
using Std.File;




class SlateDB {

    // ------------------------------------------------------------------------
    // Internal storage
    // ------------------------------------------------------------------------
    priv file: File.Handle
    priv pageSize: int
    priv classIdCounter: int = 1
    priv classes: Map<string, ClassMeta> = Map.create()

    // ------------------------------------------------------------------------
    // Class metadata structure
    // ------------------------------------------------------------------------
    class ClassMeta {
        pub id: int
        pub name: string
        pub fields: Array<FieldMeta> = Array.create()
    }

    class FieldMeta {
        pub name: string
        pub typeName: string
        pub nestedClassId: int = 0
    }

    // ------------------------------------------------------------------------
    // Open or create a DB
    // ------------------------------------------------------------------------
    pub fn open(path: string, pageSize_: int = 4096) -> SlateDB {
        let db = SlateDB.new()
        db.pageSize = pageSize_

        if !File.exists(path) {
            db.file = File.open(path, File.Mode.Write).unwrap()
            db.writeHeader()
        } else {
            db.file = File.open(path, File.Mode.ReadWrite).unwrap()
            db.loadHeader()
            db.loadMeta()
        }

        return db
    }

    // ------------------------------------------------------------------------
    // Create class schema (register for DB)
    // ------------------------------------------------------------------------
    pub fn registerClass(name: string, fields: Array<FieldMeta>) -> int {
        let meta = ClassMeta.new()
        meta.id = classIdCounter
        meta.name = name
        meta.fields = fields
        classes.insert(name, meta)
        classIdCounter += 1
        return meta.id
    }

    // ------------------------------------------------------------------------
    // Create new object (returns in-memory instance)
    // ------------------------------------------------------------------------
    pub fn create<T>() -> T {
        let obj = T.new()
        // Allocate a page and assign Class ID internally
        // Will serialize on save
        return obj
    }

    // ------------------------------------------------------------------------
    // Save object to DB
    // ------------------------------------------------------------------------
    pub fn save<T>(obj: T) {
        let meta = classes.get(typeof(T).name)
        // Serialize obj fields into bytes according to meta
        let bytes = Array.create<uint8>()
        for f in meta.fields {
            let value = obj.getField(f.name)
            bytes.extend(serializeField(value, f.typeName, f.nestedClassId))
        }
        // Write bytes to a new page
        writePage(bytes, meta.id)
    }

    // ------------------------------------------------------------------------
    // Load object by page ID
    // ------------------------------------------------------------------------
    pub fn get<T>(pageId: int) -> T {
        let meta = classes.get(typeof(T).name)
        let bytes = readPage(pageId)
        let obj = T.new()
        deserializeFields(obj, bytes, meta.fields)
        return obj
    }

    // ------------------------------------------------------------------------
    // Internal helpers (high-level interface)
    // ------------------------------------------------------------------------
    priv fn writeHeader() {
        // Write DB file header (magic, pageSize, etc.)
    }

    priv fn loadHeader() {
        // Load header (pageSize, etc.)
    }

    priv fn writePage(data: Array<uint8>, classId: int) {
        // Allocate page, write PageHeader + ClassID + data
    }

    priv fn readPage(pageId: int) -> Array<uint8> {
        // Read page data as bytes
        return Array.create<uint8>()
    }

    priv fn loadMeta() {
        // Read META pages and rebuild class map
    }

    priv fn serializeField(value: any, typeName: string, nestedClassId: int) -> Array<uint8> {
        // Convert field to bytes based on type
        return Array.create<uint8>()
    }

    priv fn deserializeFields(obj: any, bytes: Array<uint8>, fields: Array<FieldMeta>) {
        // Convert bytes back into object fields
    }
}
class User{
	pub name: string;
	
	pub id: int;
}
fn main() {

}
