// Example: Using SlateDB with encryption
using Std.IO;
using SlateDB.Slate.DB;

fn main() {
    // Create encrypted database config
    let cfg = SlateConfig.createEncrypted("mySecretPassword123!");
    let db = new SlateDB();
    db.open("users.slate", cfg);
    
    // Define User schema using builder
    let userSchema = SchemaBuilder.forTable("User");
    userSchema.addInt("id");
    userSchema.addString("name");
    userSchema.addString("email");
    userSchema.addBool("active");
    db.registerSchema(userSchema.build());
    
    // Define Post schema
    let postSchema = SchemaBuilder.forTable("Post");
    postSchema.addInt("id");
    postSchema.addString("title");
    postSchema.addString("content");
    postSchema.addObject("author");
    db.registerSchema(postSchema.build());
    
    // Create user object
    let userOpt = db.createObject("User");
    if (isSome(userOpt)) {
        let user = unwrap(userOpt);
        user.setField("id", SlateValue.createInt(1));
        user.setField("name", SlateValue.createString("Alice"));
        user.setField("email", SlateValue.createString("alice@example.com"));
        user.setField("active", SlateValue.createBool(true));
        db.save(user);
        
        // Create post object
        let postOpt = db.createObject("Post");
        if (isSome(postOpt)) {
            let post = unwrap(postOpt);
            post.setField("id", SlateValue.createInt(1));
            post.setField("title", SlateValue.createString("Hello World"));
            post.setField("content", SlateValue.createString("My first encrypted post!"));
            post.setField("author", SlateValue.createObjectRef(user.objectId));
            db.save(post);
        }
    }
    
    db.close();
    println("Database created and encrypted!");
    
    // Later: Open and query
    let db2 = new SlateDB();
    db2.open("users.slate", cfg);
    
    let users = db2.query("User");
    println($"Found {users.size()} users");
    
    let i = 0;
    while (i < users.size()) {
        let u = users[i];
        let nameOpt = u.getField("name");
        let emailOpt = u.getField("email");
        
        if (isSome(nameOpt) && isSome(emailOpt)) {
            let name = unwrap(nameOpt);
            let email = unwrap(emailOpt);
            println($"User: {name.stringValue} - {email.stringValue}");
        }
        i = i + 1;
    }
    
    db2.close();
}
