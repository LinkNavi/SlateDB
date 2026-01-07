using Std.IO;
using SlateDB.Slate.DB;

fn main() {
    println("Testing SlateDB with AES-256-GCM Encryption...");
    println("");
    
    // Create a user object
    let user = new SlateObject();
    user.className = "User";
    user.objectId = 1;
    
    user.setField("id", SlateValue.makeInt(1));
    user.setField("name", SlateValue.makeString("Alice"));
    user.setField("email", SlateValue.makeString("alice@example.com"));
    user.setField("active", SlateValue.makeBool(true));
    user.setField("balance", SlateValue.makeFloat(1234.56));
    
    // Display original data
    println("=== Original Data ===");
    println($"User ID: {user.getInt(\"id\")}");
    println($"User Name: {user.getString(\"name\")}");
    println($"User Email: {user.getString(\"email\")}");
    println($"User Active: {user.getBool(\"active\")}");
    println("");

    
    // Export unencrypted
    println("=== Unencrypted Export ===");
    exportObject(user, "user_plain.db");
    println("(You can read strings in this file)");
    println("");
    
    // Export encrypted with AES-256-GCM
    println("=== Encrypted Export (AES-256-GCM) ===");
    exportObjectEncrypted(user, "user_secure.db", "my_secret_password");
    println("(This file is completely unreadable)");
    println("");
    
    // Import encrypted
    println("=== Importing Encrypted Data ===");
    let loadedUser = importObjectEncrypted("user_secure.db", "my_secret_password");
    
    // Display loaded data
    println("=== Decrypted Data ===");
    println($"User ID: {loadedUser.getInt(\"id\")}");
    println($"User Name: {loadedUser.getString(\"name\")}");
    println($"User Email: {loadedUser.getString(\"email\")}");
    println($"User Active: {loadedUser.getBool(\"active\")}");
    println("");
    
    // Try wrong password
    println("=== Testing Wrong Password ===");
    let badUser = importObjectEncrypted("user_secure.db", "wrong_password");
    println("(Should fail with authentication error)");
    println("");
    
    println("SlateDB encryption test completed!");
}
