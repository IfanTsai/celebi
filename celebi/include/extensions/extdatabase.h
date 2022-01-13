#ifndef __CELEBI_EXTENSION_EXTDATABASE_H__
#define __CELEBI_EXTENSION_EXTDATABASE_H__

#include "celebi.h"

namespace celebiext {

using namespace celebi;

enum class ValueType {
    STRING,
    STRING_SET,
};

/**
 * @brief The MemoryKeyValueStore class is memroy key-value store for database
 */
class MemoryKeyValueStore : public KeyValueStore
{
public:
    MemoryKeyValueStore();
    MemoryKeyValueStore(std::unique_ptr<KeyValueStore> &toCache);
    virtual ~MemoryKeyValueStore();

    // Management methods
    virtual void loadKeysInto(std::function<void(std::string key,
                                                 std::string vlaue)>) override;
    virtual void clear() override;

    // Set or get methods
    virtual void setKeyValue(const std::string &key, const std::string &value) override;
    virtual void setKeyValue(const std::string &key,
                             const std::unordered_set<std::string> &value) override;

    virtual void appendKeyValue(const std::string &key, const std::string &value) override;

    virtual std::string getKeyValue(const std::string &key) override;
    virtual std::unique_ptr<std::unordered_set<std::string>>
                        getKeyValueSet(const std::string &key) override;

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

/**
 * @brief The FileKeyValueStore class is file key-value store for database
 */
class FileKeyValueStore : public KeyValueStore {
public:
    FileKeyValueStore(const std::string &fullpath);
    virtual ~FileKeyValueStore();

    // Management methods
    virtual void loadKeysInto(std::function<void(std::string key,
                                                 std::string vlaue)>) override;
    virtual void clear() override;

    // Set or get methods
    virtual void setKeyValue(const std::string &key, const std::string &value) override;
    virtual void setKeyValue(const std::string &key,
                             const std::unordered_set<std::string> &value) override;

    virtual void appendKeyValue(const std::string &key, const std::string &value) override;

    virtual std::string getKeyValue(const std::string &key) override;
    virtual std::unique_ptr<std::unordered_set<std::string>>
                        getKeyValueSet(const std::string &key) override;

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

/**
 * @brief The EmbeddedDatabase class is server proxy API
 */
class EmbeddedDatabase: public IDatabase {
public:
    EmbeddedDatabase(const std::string &dbName, const std::string &fullpath);
    EmbeddedDatabase(const std::string &dbName, const std::string &fullpath,
                     std::unique_ptr<KeyValueStore> &kvStore);
    virtual ~EmbeddedDatabase();

    virtual const std::string getDirectory() const override;

    // Management methods
    static const std::unique_ptr<IDatabase> createEmpty(const std::string &dbName);
    static const std::unique_ptr<IDatabase> createEmpty(const std::string &dbName,
                                                        std::unique_ptr<KeyValueStore> &kvStore);
    static const std::unique_ptr<IDatabase> load(const std::string &dbName);
    virtual void destroy() override;

    // Set methods
    virtual void setKeyValue(const std::string &key, const std::string &value) override;
    virtual void setKeyValue(const std::string &key,
                             const std::unordered_set<std::string> &value) override;

    // Set methods with bucket
    virtual void setKeyValue(const std::string &key, const std::string &value,
                             const std::string &bucket) override;
    virtual void setKeyValue(const std::string &key,
                             const std::unordered_set<std::string> &value,
                             const std::string &bucket) override;

    // Get methods
    virtual std::string getKeyValue(const std::string &key) override;
    virtual std::unique_ptr<std::unordered_set<std::string>>
                        getKeyValueSet(const std::string &key) override;

    // Query records methods
    virtual std::unique_ptr<IQueryResult> query(Query &q) const override;
    virtual std::unique_ptr<IQueryResult> query(BucketQuery &q) const override;

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;   // server side hidden implemention in this
};

}


#endif // __CELEBI_EXTENSION_EXTDATABASE_H__
