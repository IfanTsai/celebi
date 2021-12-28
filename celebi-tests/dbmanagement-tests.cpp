#include "tests.h"

#include <filesystem>
#include <string>

#include "celebi.h"

namespace fs = std::filesystem;

TEST_CASE("db-create", "[createEmptyDB]") {
    // Story:-
    //   [Who]   As a database administrator
    //   [What]  I need to create a new empty database
    //   [Value] So I can later store and retrieve data

    SECTION("Default settings") {
       std::string dbName = "my-empty-db";
       std::unique_ptr<celebi::IDatabase> db(celebi::Celebi::createEmptyDB(dbName));

       // We know we have been successful when:-
       // 1. We have a valid database reference returned
       //   - No test -> The above would have errored
       // 2. The DB has a folder that exists on the file system
       REQUIRE(fs::is_directory(fs::status(db->getDirectory())));
       // C++17 Ref: https://en.cppreference.com/w/cpp/filesystem/is_directory

       // 3. The database folder is empty (no database files yet)
       const auto &p = fs::directory_iterator(db->getDirectory());
       REQUIRE(p == end(p));
       // C++17 Ref: https://en.cppreference.com/w/cpp/filesystem/directory_iterator

       // 4. Destroy the database
       db->destroy();
       REQUIRE(!fs::exists(fs::status(db->getDirectory())));
    }
}

TEST_CASE("db-load", "[loadDB]") {
    // Story:-
    //   [Who]   As a database user
    //   [What]  I need to create a reference to an existing database
    //   [Value] So I can later store and retrieve data

    SECTION("Default settings") {
        std::string dbName = "my-empty-db";
        std::unique_ptr<celebi::IDatabase> db1(celebi::Celebi::createEmptyDB(dbName));
        std::unique_ptr<celebi::IDatabase> db2(celebi::Celebi::loadDB(dbName));

        // We know we have been successful when:-
        // 1. We have a valid database reference returned
        //   - No test -> The above would have errored
        // 2. The DB has a folder that exists on the file system
        REQUIRE(fs::is_directory(fs::status(db2->getDirectory())));
        // C++17 Ref: https://en.cppreference.com/w/cpp/filesystem/is_directory

        // 3. The database folder is empty (no database files yet)
        const auto &p = fs::directory_iterator(db2->getDirectory());
        REQUIRE(p == end(p));
        // C++17 Ref: https://en.cppreference.com/w/cpp/filesystem/directory_iterator

        // 4. Destroy the database
        db2->destroy();
        REQUIRE(!fs::exists(fs::status(db2->getDirectory())));
    }
}
