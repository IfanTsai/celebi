#ifndef __CELEBI_DATABASE_H__
#define __CELEBI_DATABASE_H__

#include "query.h"

#include <string>
#include <memory>
#include <functional>
#include <unordered_set>

namespace celebi {

class Store {
public:
    Store() = default;
    virtual ~Store() = default;
};

/**
 * @brief The KeyValueStore class is key-value store layer for database
 */
class KeyValueStore : public Store {
public:
    KeyValueStore() = default;
    virtual ~KeyValueStore() = default;

    // Management methods
    virtual void loadKeysInto(std::function<void(std::string key,
                                                 std::string vlaue)>) = 0;
    virtual void clear() = 0;

    // Set or get methods
    virtual void setKeyValue(const std::string &key, const std::string &value) = 0;
    virtual void setKeyValue(const std::string &key,
                             const std::unordered_set<std::string> &value) = 0;
    virtual void appendKeyValue(const std::string &key, const std::string &value) = 0;

    virtual std::string getKeyValue(const std::string &key) = 0;
    virtual std::unique_ptr<std::unordered_set<std::string>>
                        getKeyValueSet(const std::string &key) = 0;
};

/**
 * @brief The IDatabase class which is client API and only knowledged by user
 */
class IDatabase
{
public:
    IDatabase() = default;
    virtual ~IDatabase() = default;

    virtual const std::string getDirectory() const = 0;

    // Management methods
    static const std::unique_ptr<IDatabase> createEmpty(const std::string &dbName,
                                                        std::unique_ptr<KeyValueStore> &kvStore);
    static const std::unique_ptr<IDatabase> load(const std::string &dbName);
    virtual void destroy() = 0;

    // Set methods
    virtual void setKeyValue(const std::string &key, const std::string &value) = 0;
    virtual void setKeyValue(const std::string &key,
                             const std::unordered_set<std::string> &value) = 0;

    // Set methods with bucket
    virtual void setKeyValue(const std::string &key, const std::string &value,
                             const std::string &bucket) = 0;
    virtual void setKeyValue(const std::string &key,
                             const std::unordered_set<std::string> &value,
                             const std::string &bucket) = 0;

    // Get methods
    virtual std::string getKeyValue(const std::string &key) = 0;
    virtual std::unique_ptr<std::unordered_set<std::string>>
                        getKeyValueSet(const std::string &key) = 0;

    // Query records methods
    virtual std::unique_ptr<IQueryResult> query(Query &q) const = 0;
    virtual std::unique_ptr<IQueryResult> query(BucketQuery &q) const = 0;
};

}

#endif // __CELEBI_DATABASE_H__
