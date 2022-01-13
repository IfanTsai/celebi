#include "database.h"
#include "extensions/extdatabase.h"
#include "extensions/extquery.h"

#include <string>
#include <fstream>
#include <filesystem>
#include <unordered_map>

using namespace celebi;
using namespace celebiext;

namespace fs = std::filesystem;

/*
 ************************************************************
 * Server side implemention hidden in Impl class
 ************************************************************
 */
class EmbeddedDatabase::Impl : public IDatabase {
public:
    Impl(const std::string &dbName, const std::string &fullpath);
    Impl(const std::string &dbName, const std::string &fullpath,
         std::unique_ptr<KeyValueStore> &kvStore);
    virtual ~Impl();

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

    virtual std::string getKeyValue(const std::string &key) override;
    virtual std::unique_ptr<std::unordered_set<std::string>>
                        getKeyValueSet(const std::string &key) override;

    // Query records methods
    virtual std::unique_ptr<IQueryResult> query(Query &query) const override;
    virtual std::unique_ptr<IQueryResult> query(BucketQuery &query) const override;

private:
    const std::string getIndexDirPath() const;
    static const std::string getDbDirPath(const std::string &dbName);
    static const std::string getIndexKey(const std::string &bucket);
    void indexForBucket(const std::string &key, const std::string &bucket);
    void appendKeyForBucket(const std::string &key, const std::string &bucket);

    static const std::string baseDir;
    static const std::string indexDir;
    std::string m_name;
    std::string m_fullpath;
    std::unique_ptr<KeyValueStore> m_keyValueStore;
    std::unique_ptr<KeyValueStore> m_indexStore;
};

const std::string EmbeddedDatabase::Impl::baseDir = ".celebi";
const std::string EmbeddedDatabase::Impl::indexDir = ".indexes";

// Use memory storage and file persistence by default
EmbeddedDatabase::Impl::Impl(const std::string &dbName, const std::string &fullpath)
    : m_name(dbName), m_fullpath(fullpath)
{
    std::unique_ptr<KeyValueStore> fileStore = std::make_unique<FileKeyValueStore>(fullpath);
    std::unique_ptr<KeyValueStore> memoryStore = std::make_unique<MemoryKeyValueStore>(fileStore);
    m_keyValueStore = std::move(memoryStore);

    std::unique_ptr<KeyValueStore> fileIndexStore =
            std::make_unique<FileKeyValueStore>(getIndexDirPath());
    std::unique_ptr<KeyValueStore> memoryIndexStore =
            std::make_unique<MemoryKeyValueStore>(fileIndexStore);
    m_indexStore = std::move(memoryIndexStore);
}

// User can specify kv store for database
EmbeddedDatabase::Impl::Impl(const std::string &dbName, const std::string &fullpath,
                             std::unique_ptr<KeyValueStore> &kvStore)
    : m_name(dbName), m_fullpath(fullpath), m_keyValueStore(kvStore.release())
{
    std::unique_ptr<KeyValueStore> fileIndexStore =
            std::make_unique<FileKeyValueStore>(getIndexDirPath());
    std::unique_ptr<KeyValueStore> memoryIndexStore =
            std::make_unique<MemoryKeyValueStore>(fileIndexStore);
    m_indexStore = std::move(memoryIndexStore);
}

EmbeddedDatabase::Impl::~Impl()
{
}

inline const std::string EmbeddedDatabase::Impl::getIndexDirPath() const
{
    return m_fullpath + "/" + indexDir;
}

inline const std::string EmbeddedDatabase::Impl::getDbDirPath(const std::string &dbName)
{
    if (!fs::exists(baseDir))
        fs::create_directory(baseDir);

    return baseDir + "/" + dbName;
}

inline const std::string EmbeddedDatabase::Impl::getIndexKey(const std::string &bucket)
{
    return "bucket::" + bucket;
}

// Management methods

const std::unique_ptr<IDatabase> EmbeddedDatabase::Impl::createEmpty(const std::string &dbName)
{
    const std::string dbFolder = getDbDirPath(dbName);
    if (!fs::exists(dbFolder))
        fs::create_directory(dbFolder);

    return std::make_unique<EmbeddedDatabase::Impl>(dbName, dbFolder);
}

const std::unique_ptr<IDatabase>
EmbeddedDatabase::Impl::createEmpty(const std::string &dbName,
                                    std::unique_ptr<KeyValueStore> &kvStore)
{
    const std::string dbFolder = getDbDirPath(dbName);

    return std::make_unique<EmbeddedDatabase::Impl>(dbName, dbFolder, kvStore);
}

const std::unique_ptr<IDatabase> EmbeddedDatabase::Impl::load(const std::string &dbName)
{
    std::string dbFolder = getDbDirPath(dbName);

    return std::make_unique<EmbeddedDatabase::Impl>(dbName, dbFolder);
}

void EmbeddedDatabase::Impl::destroy()
{
   m_keyValueStore->clear();
}

const std::string EmbeddedDatabase::Impl::getDirectory() const
{
    return m_fullpath;
}

void EmbeddedDatabase::Impl::indexForBucket(const std::string &key, const std::string &bucket)
{
    // add to bucket index
    std::string indexKey = getIndexKey(bucket);
    // query the key index
    std::unique_ptr<std::unordered_set<std::string>> recordKeys =
            m_indexStore->getKeyValueSet(indexKey);

    if (recordKeys->find(key) == recordKeys->end()) {
        recordKeys->insert(key);
        m_indexStore->setKeyValue(indexKey, *recordKeys.release());
    }
}

void EmbeddedDatabase::Impl::appendKeyForBucket(const std::string &key, const std::string &bucket)
{
    // add to bucket index
    std::string indexKey = getIndexKey(bucket);
    // query the key index
    std::unique_ptr<std::unordered_set<std::string>> recordKeys =
            m_indexStore->getKeyValueSet(indexKey);

    if (0 == recordKeys->size()) {
        recordKeys->insert(key);
        m_indexStore->setKeyValue(indexKey, *recordKeys.release());
    } else if (recordKeys->find(key) == recordKeys->end()) {
        m_indexStore->appendKeyValue(indexKey, key);
    }
}

// Set or get methods

void EmbeddedDatabase::Impl::setKeyValue(const std::string &key,
                                         const std::string &value)
{
    m_keyValueStore->setKeyValue(key, value);
}

void EmbeddedDatabase::Impl::setKeyValue(const std::string &key,
                                         const std::unordered_set<std::string> &value)
{
    m_keyValueStore->setKeyValue(key, value);
}

void EmbeddedDatabase::Impl::setKeyValue(const std::string &key,
                                         const std::string &value,
                                         const std::string &bucket)
{
    setKeyValue(key, value);
    indexForBucket(key, bucket);
}

void EmbeddedDatabase::Impl::setKeyValue(const std::string &key,
                                         const std::unordered_set<std::string> &value,
                                         const std::string &bucket)
{
    setKeyValue(key, value);
    indexForBucket(key, bucket);
}

std::string EmbeddedDatabase::Impl::getKeyValue(const std::string &key)
{
    return m_keyValueStore->getKeyValue(key);
}

std::unique_ptr<std::unordered_set<std::string>>
EmbeddedDatabase::Impl::getKeyValueSet(const std::string &key)
{
    return m_keyValueStore->getKeyValueSet(key);
}

std::unique_ptr<IQueryResult> EmbeddedDatabase::Impl::query(Query &q) const
{
    // Query is abstract, so try overloading here
    return query(static_cast<decltype(q)>(q));
}

std::unique_ptr<IQueryResult> EmbeddedDatabase::Impl::query(BucketQuery &q) const
{
    const std::string indexKey = getIndexKey(q.bucket());

    return std::make_unique<DefaultQueryResult>(m_indexStore->getKeyValueSet(indexKey));
}

/*
 ****************************************************************************
 * High level database client API implementation below
 ****************************************************************************
 */

EmbeddedDatabase::EmbeddedDatabase(const std::string &dbName, const std::string &fullpath)
    : m_impl(std::make_unique<EmbeddedDatabase::Impl>(dbName, fullpath))
{

}

EmbeddedDatabase::EmbeddedDatabase(const std::string &dbName, const std::string &fullpath,
                                   std::unique_ptr<KeyValueStore> &kvStore)
    : m_impl(std::make_unique<EmbeddedDatabase::Impl>(dbName, fullpath, kvStore))
{

}

EmbeddedDatabase::~EmbeddedDatabase()
{

}

// Management methods
const std::unique_ptr<IDatabase> EmbeddedDatabase::createEmpty(const std::string &dbName)
{
    return EmbeddedDatabase::Impl::createEmpty(dbName);
}

const std::unique_ptr<IDatabase> EmbeddedDatabase::createEmpty(const std::string &dbName,
                                                               std::unique_ptr<KeyValueStore> &kvStore)
{
    return EmbeddedDatabase::Impl::createEmpty(dbName, kvStore);
}

const std::unique_ptr<IDatabase> EmbeddedDatabase::load(const std::string &dbName)
{
    return EmbeddedDatabase::Impl::load(dbName);
}

void EmbeddedDatabase::destroy()
{
    m_impl->destroy();
}

const std::string EmbeddedDatabase::getDirectory() const
{
    return m_impl->getDirectory();
}

// Set or get methods
void EmbeddedDatabase::setKeyValue(const std::string &key, const std::string &value)
{
    m_impl->setKeyValue(key, value);
}

void EmbeddedDatabase::setKeyValue(const std::string &key,
                                   const std::unordered_set<std::string> &value)
{
    m_impl->setKeyValue(key, value);
}

void EmbeddedDatabase::setKeyValue(const std::string &key,
                                   const std::string &value,
                                   const std::string &bucket)
{
    m_impl->setKeyValue(key, value, bucket);
}

void EmbeddedDatabase::setKeyValue(const std::string &key,
                                   const std::unordered_set<std::string> &value,
                                   const std::string &bucket)
{
    m_impl->setKeyValue(key, value, bucket);
}


std::string EmbeddedDatabase::getKeyValue(const std::string &key)
{
    return m_impl->getKeyValue(key);
}

std::unique_ptr<std::unordered_set<std::string>>
EmbeddedDatabase::getKeyValueSet(const std::string &key)
{
    return m_impl->getKeyValueSet(key);
}

// Query records methods
std::unique_ptr<IQueryResult> EmbeddedDatabase::query(Query &q) const
{
    return m_impl->query(q);
}

std::unique_ptr<IQueryResult> EmbeddedDatabase::query(BucketQuery &q) const
{
    return m_impl->query(q);
}
