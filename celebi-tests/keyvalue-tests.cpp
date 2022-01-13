#include "catch.hpp"
#include "celebi.h"

#include <filesystem>

namespace fs = std::filesystem;

TEST_CASE("Store and retrieve a string value", "[setKeyValue, getKeyValue]") {
    // Story:-
    //   [Who]   As a database user
    //   [What]  I need to store and retrieve a value with a simple name to dtabase
    //   [Value] So I can persist data for later use
    SECTION("Basic set and get") {
        std::string dbname("my-empty-db");
        std::unique_ptr<celebi::IDatabase> db(celebi::Celebi::createEmptyDB(dbname));

        // We know we have been successful when:-
        // 1. The retrieved value is the same as the stored value
        std::string key = "simple string";
        std::string value = "some highly valuable values";
        db->setKeyValue(key, value);
        REQUIRE(value == db->getKeyValue(key));

        std::string value2 = "some highly valuable values2";
        db->setKeyValue(key, value2);
        REQUIRE(value2 == db->getKeyValue(key));

        db->destroy();
        REQUIRE(!fs::exists(fs::status(db->getDirectory())));
    }

    SECTION("Bucket set and get") {
        std::string dbname("my-empty-db");
        std::unique_ptr<celebi::IDatabase> db(celebi::Celebi::createEmptyDB(dbname));

        std::string key = "simple string";
        std::string value = "some highly valuable values";
        std::string bucket = "bucket1";
        db->setKeyValue(key, value, bucket);
        REQUIRE(value == db->getKeyValue(key));

        db->destroy();
        REQUIRE(!fs::exists(fs::status(db->getDirectory())));
    }
}


TEST_CASE("Store and retrive a list value",
          "[setKeyValue(std::string,std::unordered_set<std::string>), getKeyValueSet]") {
    SECTION("Basic set and get") {
        std::string dbname("my-empty-db");
        std::unique_ptr<celebi::IDatabase> db(celebi::Celebi::createEmptyDB(dbname));

        std::string key = "simple string";
        std::string value1 = "some highly valuable value1";
        std::string value2 = "some highly valuable value2";
        std::string value3 = "some highly valuable value3";
        std::unordered_set<std::string> set = { value1, value2, value3 };
        db->setKeyValue(key, set);

        auto result = db->getKeyValueSet(key);
        REQUIRE(3 == result->size());
        REQUIRE(result->find(value1) != result->end());
        REQUIRE(result->find(value2) != result->end());
        REQUIRE(result->find(value3) != result->end());

        std::string value4 = "some highly valuable value4";
        std::string value5 = "some highly valuable value5";
        std::string value6 = "some highly valuable value6";
        set = { value4, value5, value6 };
        db->setKeyValue(key, set);

        result = db->getKeyValueSet(key);
        REQUIRE(3 == result->size());
        REQUIRE(result->find(value4) != result->end());
        REQUIRE(result->find(value5) != result->end());
        REQUIRE(result->find(value6) != result->end());

        db->destroy();
        REQUIRE(!fs::exists(fs::status(db->getDirectory())));
    }

    SECTION("Bucket set and get") {
        std::string dbname("my-empty-db");
        std::unique_ptr<celebi::IDatabase> db(celebi::Celebi::createEmptyDB(dbname));

        std::string key = "simple string";
        std::string value1 = "some highly valuable value1";
        std::string value2 = "some highly valuable value2";
        std::string value3 = "some highly valuable value3";
        std::unordered_set<std::string> set = { value1, value2, value3 };
        std::string bucket = "bucket1";
        db->setKeyValue(key, set, bucket);

        auto result = db->getKeyValueSet(key);
        REQUIRE(3 == result->size());
        REQUIRE(result->find(value1) != result->end());
        REQUIRE(result->find(value2) != result->end());
        REQUIRE(result->find(value3) != result->end());

        db->destroy();
        REQUIRE(!fs::exists(fs::status(db->getDirectory())));
    }
}
