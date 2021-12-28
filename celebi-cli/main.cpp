#include "cxxopts.hpp"
#include "celebi.h"

#include <cstdlib>
#include <iostream>
#include <string>


cxxopts::Options options("celebi-cli", "CLI for celebi");

static inline void printUsage(const std::string &info = "", int exitCode = 0)
{
    if (info.length())
        std::cout << info << std::endl << std::endl;

    std::cout << options.help() << std::endl;

    ::exit(exitCode);
}

// Incorporating https://github.com/jarro2783/cxxopts as a header only library for options parsing
int main(int argc, char *argv[])
{
    options.add_options()
          ("c,create", "Create a DB")
          ("d,destroy", "Destroy a DB")
          ("s,set", "Set a key in a DB")
          ("g,get", "Get a key from a DB")
          ("q,query", "Query the DB (must also specify a query term. E.g. b for bucket)")
          ("n,name","Database name (required)", cxxopts::value<std::string>())
          ("k,key","Key to set/get", cxxopts::value<std::string>())
          ("v,value","Value to set", cxxopts::value<std::string>())
          ("b,bucket","Bucket stored in", cxxopts::value<std::string>())
          ("h,help", "Print Usage")
        ;

    auto result = options.parse(argc, argv);

    if (result.count("h"))
        printUsage();

    if (result.count("c")) {
        if (!result.count("n")) {
            printUsage("You must specify a database naem with -n <name>", 1);
        }
        // Create database
        std::string dbName = result["n"].as<std::string>();
        std::unique_ptr<celebi::IDatabase> db(celebi::Celebi::createEmptyDB(dbName));
    } else if (result.count("d")) {
        if (!result.count("n")) {
            printUsage("You must specify a database naem with -n <name>", 1);
        }
        // Destroy database
        std::string dbName = result["n"].as<std::string>();
        std::unique_ptr<celebi::IDatabase> db(celebi::Celebi::loadDB(dbName));
        db->destroy();
    } else if (result.count("s")) {
        if (!result.count("n"))
            printUsage("You must specify a database naem with -n <name>", 1);

        if (!result.count("k"))
            printUsage("You must specify a key to set with -k <key>", 1);

        if (!result.count("v"))
            printUsage("You must specify a value to set with -v <value>", 1);

        // Set key-value into database
        std::string key = result["k"].as<std::string>();
        std::string value = result["v"].as<std::string>();
        std::string dbName = result["n"].as<std::string>();
        std::unique_ptr<celebi::IDatabase> db(celebi::Celebi::loadDB(dbName));
        db->setKeyValue(key, value);
    } else if (result.count("g")) {
        if (!result.count("n"))
            printUsage("You must specify a database naem with -n <name>", 1);

        if (!result.count("k"))
            printUsage("You must specify a key to set with -k <key>", 1);

        // Get key-value from database
        std::string key = result["k"].as<std::string>();
        std::string dbName = result["n"].as<std::string>();
        std::unique_ptr<celebi::IDatabase> db(celebi::Celebi::loadDB(dbName));
        std::cout << db->getKeyValue(key) << std::endl;
    } else {
        printUsage("No command specified !");
    }

    return 0;
}
