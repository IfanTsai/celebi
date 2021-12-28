#include "catch.hpp"
#include "celebi.h"

#include <filesystem>

namespace fs = std::filesystem;

TEST_CASE("Store and retrieve a value", "[setKeyValue, getKeyValue]") {
    // Story:-
    //   [Who]   As a database user
    //   [What]  I need to store and retrieve a value with a simple name to dtabase
    //   [Value] So I can persist data for later use
    std::string dbname("my-empty-db");
    std::unique_ptr<celebi::IDatabase> db(celebi::Celebi::createEmptyDB(dbname));

    // We know we have been successful when:-
    // 1. The retrieved value is the same as the stored value
    std::string key = "simple string";
    std::string value = "some highly valuable values";
    db->setKeyValue(key, value);
    REQUIRE(value == db->getKeyValue(key));

    db->destroy();
    REQUIRE(!fs::exists(fs::status(db->getDirectory())));
}
