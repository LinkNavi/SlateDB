// Example: Using SlateDB for a blog application
using Std.IO;
using Std.String;
using Std.Array;

fn setupSchemas(db: SlateDB) -> SlateDB {
    println("Setting up schemas...");
    
    let userSchema = SchemaBuilder.forTable("User")
        .addInt("id")
        .addString("username")
        .addString("email")
        .addInt("posts_count")
        .build();
    
    db.registerSchema(userSchema);
    println("  ✓ User schema registered");
    
    let postSchema = SchemaBuilder.forTable("Post")
        .addInt("id")
        .addString("title")
        .addString("content")
        .addObject("author")
        .addArray("tags")
        .addInt("likes")
        .build();
    
    db.registerSchema(postSchema);
    println("  ✓ Post schema registered");
    
    let commentSchema = SchemaBuilder.forTable("Comment")
        .addInt("id")
        .addString("text")
        .addObject("author")
        .addInt("post_id")
        .build();
    
    db.registerSchema(commentSchema);
    println("  ✓ Comment schema registered\n");
    return db;
}

fn createSampleData(db: SlateDB) -> SlateDB {
    println("Creating sample data...");
    
    let userOpt = db.createObject("User");
    if (isNone(userOpt)) {
        println("Failed to create user object");
        return db;
    }
    
    let user = unwrap(userOpt);
    user.setField("id", SlateValue.createInt(1));
    user.setField("username", SlateValue.createString("alice"));
    user.setField("email", SlateValue.createString("alice@example.com"));
    user.setField("posts_count", SlateValue.createInt(0));
    
    db.save(user);
    println("  ✓ Created user: alice");
    
    let user2Opt = db.createObject("User");
    let user2 = unwrap(user2Opt);
    user2.setField("id", SlateValue.createInt(2));
    user2.setField("username", SlateValue.createString("bob"));
    user2.setField("email", SlateValue.createString("bob@example.com"));
    user2.setField("posts_count", SlateValue.createInt(0));
    
    db.save(user2);
    println("  ✓ Created user: bob");
    
    let postOpt = db.createObject("Post");
    let post = unwrap(postOpt);
    post.setField("id", SlateValue.createInt(1));
    post.setField("title", SlateValue.createString("Getting Started with SlateDB"));
    post.setField("content", SlateValue.createString("SlateDB is an amazing object database..."));
    post.setField("author", SlateValue.createObjectRef(user.objectId));
    
    let tags = SlateValue.createArray();
    tags.pushToArray(SlateValue.createString("database"));
    tags.pushToArray(SlateValue.createString("tutorial"));
    tags.pushToArray(SlateValue.createString("magolor"));
    post.setField("tags", tags);
    
    post.setField("likes", SlateValue.createInt(42));
    
    db.save(post);
    println("  ✓ Created post with embedded author");
    
    let post2Opt = db.createObject("Post");
    let post2 = unwrap(post2Opt);
    post2.setField("id", SlateValue.createInt(2));
    post2.setField("title", SlateValue.createString("Advanced SlateDB Patterns"));
    post2.setField("content", SlateValue.createString("Let's explore advanced features..."));
    post2.setField("author", SlateValue.createObjectRef(user2.objectId));
    
    let tags2 = SlateValue.createArray();
    tags2.pushToArray(SlateValue.createString("database"));
    tags2.pushToArray(SlateValue.createString("advanced"));
    post2.setField("tags", tags2);
    
    post2.setField("likes", SlateValue.createInt(128));
    
    db.save(post2);
    println("  ✓ Created second post\n");
    return db;
}

fn displayAllPosts(db: SlateDB) -> SlateDB {
    println("=== All Blog Posts ===\n");
    
    let posts = db.query("Post");
    let count: int = posts.size();
    
    println($"Found {count} posts:\n");
    
    let i = 0;
    while (i < count) {
        let post = posts[i];
        
        let titleOpt = post.getField("title");
        let title = "Untitled";
        if (isSome(titleOpt)) {
            let titleVal = unwrap(titleOpt);
            title = titleVal.stringValue;
        }
        
        let likesOpt = post.getField("likes");
        let likes = 0;
        if (isSome(likesOpt)) {
            let likesVal = unwrap(likesOpt);
            likes = likesVal.intValue;
        }
        
        let authorOpt = post.getField("author");
        let authorName = "Unknown";
        if (isSome(authorOpt)) {
            let authorVal = unwrap(authorOpt);
            // For now just show the author ID since we store refs
            let authorId = authorVal.objectId;
            authorName = $"User #{authorId}";
        }
        
        println($"📝 {title}");
        println($"   Author: {authorName}");
        println($"   Likes: {likes}");
        
        let tagsOpt = post.getField("tags");
        if (isSome(tagsOpt)) {
            let tagsVal = unwrap(tagsOpt);
            let tagArray = tagsVal.arrayValue;
            let tagCount: int = tagArray.size();
            
            print("   Tags: ");
            let j = 0;
            while (j < tagCount) {
                let tag = tagArray[j];
                print(tag.stringValue);
                if (j < tagCount - 1) {
                    print(", ");
                }
                j = j + 1;
            }
            println("");
        }
        
        println("");
        i = i + 1;
    }
    return db;
}

fn testEmbeddedObjects(db: SlateDB) -> SlateDB {
    println("=== Testing Nested Objects ===\n");
    
    let innerUser = db.createObject("User");
    let user = unwrap(innerUser);
    user.setField("username", SlateValue.createString("nested_user"));
    
    let middlePost = db.createObject("Post");
    let post = unwrap(middlePost);
    post.setField("title", SlateValue.createString("Nested Post"));
    post.setField("author", SlateValue.createObjectRef(user.objectId));
    
    let outerComment = db.createObject("Comment");
    let comment = unwrap(outerComment);
    comment.setField("text", SlateValue.createString("Great post!"));
    comment.setField("author", SlateValue.createObjectRef(user.objectId));
    comment.setField("post_id", SlateValue.createInt(1));
    
    db.save(comment);
    
    println("✓ Created nested object structure:");
    println("  Comment -> Author (User)");
    println("  Comment -> Post -> Author (User)");
    println("\nNested objects allow you to create rich, interconnected data!");
    return db;
}

fn main() {
    println("=== SlateDB Blog Example ===\n");
    
    let config = SlateConfig.createEncrypted("my-secret-password-123");
    config.pageSize = 8192;
    
    let db = new SlateDB();
    let opened = db.open("blog.slatedb", config);
    
    if (!opened) {
        println("Failed to open database!");
        return 1;
    }
    
    println("✓ Database opened successfully\n");
    
    db = setupSchemas(db);
    db = createSampleData(db);
    db = displayAllPosts(db);
    db = testEmbeddedObjects(db);
    
    db.close();
    println("\n✓ Database closed");
}
