#include "catch.hpp"
#include "celebi.h"

#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

TEST_CASE("query tests", "[query]") {
    // Story:-
    //   [Who]   As a database user
    //   [What]  I need to store with the bucket and retrieve a value with a simple name to dtabase
    //   [Value] So I can persist data for later use
    SECTION("Bucket set and get") {
        std::string dbname("my-empty-db");
        std::unique_ptr<celebi::IDatabase> db(celebi::Celebi::createEmptyDB(dbname));

        std::string key1 = "first key";
        std::string val1 = "first value";

        std::string key2 = "second key";
        std::string val2 = "second value";

        std::string key3 = "third key";
        std::string val3 = "third value";

        std::string key4 = "fourth key";
        std::string val4 = "fourth value";

        std::string bucket = "bucket1";
        db->setKeyValue(key1, val1, bucket);
        db->setKeyValue(key2, val2);
        db->setKeyValue(key3, val3);
        db->setKeyValue(key4, val4, bucket);

        celebi::BucketQuery bq(bucket);
        std::unique_ptr<celebi::IQueryResult> res = db->query(bq);
        auto recordKeys = res->recordKeys();

        for (auto it = recordKeys->begin(); it != recordKeys->end(); it++)
            std::cout << "key in bucket: " << *it << std::endl;

        REQUIRE(recordKeys->size() == 2);
        REQUIRE(recordKeys->find(key1) != recordKeys->end());
        REQUIRE(recordKeys->find(key2) == recordKeys->end());
        REQUIRE(recordKeys->find(key3) == recordKeys->end());
        REQUIRE(recordKeys->find(key4) != recordKeys->end());


        db->destroy();
        REQUIRE(!fs::exists(fs::status(db->getDirectory())));
    }
}

