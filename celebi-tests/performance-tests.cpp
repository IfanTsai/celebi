#include "catch.hpp"
#include "celebi.h"
#include "extensions/extdatabase.h"

#include <unordered_map>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

static void testPerformance(std::unique_ptr<celebi::IDatabase> db)
{
    std::cout << "------------------------------------------" << std::endl;
    // 1. Prepare the 100k key-value pairs
    long total = 100000;
    std::unordered_map<std::string, std::string> keyValues;
    for (long i = 0; i < total; i++)
        keyValues.emplace(std::to_string(i), std::to_string(i));

    // 2. Store the 100k key-value pairs
    std::cout << "=========== SET ===========" << std::endl;
    auto begin = std::chrono::steady_clock::now();
    for (auto &it : keyValues)
       db->setKeyValue(it.first, it.second);
    auto end = std::chrono::steady_clock::now();
    std::cout << "  " << keyValues.size() << " completed in "
              << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin)).count() / 1000.0 / 1000.0
              << " seconds" << std::endl;
    std::cout << "  " << keyValues.size() * 1000.0 * 1000.0 / (std::chrono::duration_cast<std::chrono::microseconds>(end - begin)).count()
              << " requests per second" << std::endl;

    // 3. Retrieve the 100k key-value pairs
    std::cout << "=========== GET ===========" << std::endl;
    std::string res;
    begin = std::chrono::steady_clock::now();
    for (auto &it : keyValues)
        res = db->getKeyValue(it.first);
    end = std::chrono::steady_clock::now();
    std::cout << "  " << keyValues.size() << " completed in "
              << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin)).count() / 1000.0 / 1000.0
              << " seconds" << std::endl;
    std::cout << "  " << keyValues.size() * 1000.0 * 1000.0 / (std::chrono::duration_cast<std::chrono::microseconds>(end - begin)).count()
              << " requests per second" << std::endl;

    db->destroy();
    std::cout << "------------------------------------------" << std::endl << std::endl;
}

TEST_CASE("Measure basic performance", "[setKeyValue, getKeyValue]") {
    // Story:-
    //   [Who]   As a database administrator
    //   [What]  I need to know the performance characteristics of each celebi storage class
    //   [Value] So I can configure the most appropriate settings for each of my user's databases
    SECTION("Store and retrieve 100k keys - Memory and persistent store") {
        std::cout <<  "Default key-value store" << std::endl;
        std::string dbName("my-empty-db");
        std::unique_ptr<celebi::IDatabase> db(celebi::Celebi::createEmptyDB(dbName));

        testPerformance(std::move(db));
    }

    SECTION("Store and retrieve 100k keys - Memory store") {
        std::cout << "Memory key-value store" << std::endl;
        std::string dbName("my-empty-db");
        std::unique_ptr<celebi::KeyValueStore> memoryStore = std::make_unique<celebiext::MemoryKeyValueStore>();
        std::unique_ptr<celebi::IDatabase> db(celebi::Celebi::createEmptyDB(dbName, memoryStore));

        testPerformance(std::move(db));
    }

    SECTION("Store and retrieve 100k keys - File store") {
        std::cout << "File key-value store" << std::endl;
        std::string dbName("my-empty-db");
        std::string fullpath = ".celebi/" + dbName;
        std::unique_ptr<celebi::KeyValueStore> fileStore = std::make_unique<celebiext::FileKeyValueStore>(fullpath);
        std::unique_ptr<celebi::IDatabase> db(celebi::Celebi::createEmptyDB(dbName, fileStore));

        testPerformance(std::move(db));
    }
}
