// Example: Using SlateDB with encryption
using Std.IO;
using Slate.DB;

fn main() {
    // Create encrypted database
    let db = SlateDB.createEncrypted("users.slate", "mySecretPassword123!");
    
    // Define schema using fluent API
    db.schema(Schema.define("User")
        .int("id")
        .string("name")
        .string("email")
        .bool("active")
        .build());
    
    db.schema(Schema.define("Post")
        .int("id")
        .string("title")
        .string("content")
        .ref("author")
        .build());
    
    // Create objects
    let userOpt = db.new("User");
    if (isSome(userOpt)) {
        let user = unwrap(userOpt);
        user.set("id", SlateValue.int(1));
        user.set("name", SlateValue.string("Alice"));
        user.set("email", SlateValue.string("alice@example.com"));
        user.set("active", SlateValue.bool(true));
        db.save(user);
        
        // Create a post
        let postOpt = db.new("Post");
        if (isSome(postOpt)) {
            let post = unwrap(postOpt);
            post.set("id", SlateValue.int(1));
            post.set("title", SlateValue.string("Hello World"));
            post.set("content", SlateValue.string("My first encrypted post!"));
            post.set("author", SlateValue.ref(user.objectId));
            db.save(post);
        }
    }
    
    // Flush to disk (encrypted with AES-256-GCM)
    db.flush();
    db.close();
    
    println("Database created and encrypted!");
    
    // Later: Open encrypted database
    let db2 = SlateDB.openEncrypted("users.slate", "mySecretPassword123!");
    
    // Query users
    let users = db2.find("User");
    println($"Found {users.size()} users");
    
    let i = 0;
    while (i < users.size()) {
        let u = users[i];
        println($"User: {u.getString(\"name\")} - {u.getString(\"email\")}");
        i = i + 1;
    }
    
    db2.close();
    
    // Wrong password will fail
    println("Trying wrong password...");
    let db3 = SlateDB.openEncrypted("users.slate", "wrongPassword");
    let badUsers = db3.find("User");
    println($"Users with wrong password: {badUsers.size()}"); // Will be 0
}
