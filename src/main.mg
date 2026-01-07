using Std.IO;
using Slate.DB;

fn main() {
    println("Testing SlateDB classes...");
    
    // Create a schema using static method
    let userSchema = Schema.define("User")
        .addInt("id")
        .addString("name")
        .addString("email")
        .addBool("active");
    let built = userSchema.build();
    
    println($"Created schema: {built.className}");
    
    // Create a SlateObject
    let user = new SlateObject();
    user.className = "User";
    user.objectId = 1;
    
    // Set fields using SlateValue static methods
    user.setField("id", SlateValue.makeInt(1));
    user.setField("name", SlateValue.makeString("Alice"));
    user.setField("email", SlateValue.makeString("alice@example.com"));
    user.setField("active", SlateValue.makeBool(true));
    
    // Get fields back
    println($"User ID: {user.getInt(\"id\")}");
    println($"User Name: {user.getString(\"name\")}");
    println($"User Email: {user.getString(\"email\")}");
    println($"User Active: {user.getBool(\"active\")}");
    
    println("SlateDB test completed!");
}
