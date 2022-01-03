#include "database.h"
#include "extensions/extdatabase.h"

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

    virtual const std::string getDirectory() const;

    // Management methods
    static const std::unique_ptr<IDatabase> createEmpty(const std::string &dbName);
    static const std::unique_ptr<IDatabase> createEmpty(const std::string &dbName,
                                                        std::unique_ptr<KeyValueStore> &kvStore);
    static const std::unique_ptr<IDatabase> load(const std::string &dbName);
    virtual void destroy();

    // Set or get methods
    virtual void setKeyValue(const std::string &key, const std::string &value);
    virtual std::string getKeyValue(const std::string &key);

private:
    const std::string getKeyFromFilename(const std::string &filename);

    static const std::string baseDir;
    static const std::string fileExtension;
    std::string m_name;
    std::string m_fullpath;
    std::unique_ptr<KeyValueStore> m_keyValueStore;
};

const std::string EmbeddedDatabase::Impl::baseDir = ".celebi";
const std::string EmbeddedDatabase::Impl::fileExtension = ".kv";

// Use memory storage and file persistence by default
EmbeddedDatabase::Impl::Impl(const std::string &dbName, const std::string &fullpath)
    : m_name(dbName), m_fullpath(fullpath)
{
    std::unique_ptr<KeyValueStore> fileStore = std::make_unique<FileKeyValueStore>(fullpath);
    std::unique_ptr<KeyValueStore> memoryStore = std::make_unique<MemoryKeyValueStore>(fileStore);
    m_keyValueStore = std::move(memoryStore);
}

// User can specify kv store for database
EmbeddedDatabase::Impl::Impl(const std::string &dbName, const std::string &fullpath,
                             std::unique_ptr<KeyValueStore> &kvStore)
    : m_name(dbName), m_fullpath(fullpath), m_keyValueStore(kvStore.release())
{

}

EmbeddedDatabase::Impl::~Impl()
{
}

// Management methods

const std::unique_ptr<IDatabase> EmbeddedDatabase::Impl::createEmpty(const std::string &dbName)
{
    if (!fs::exists(baseDir))
        fs::create_directory(baseDir);

    std::string dbFolder = baseDir + "/" + dbName;
    if (!fs::exists(dbFolder))
        fs::create_directory(dbFolder);

    return std::make_unique<EmbeddedDatabase::Impl>(dbName, dbFolder);
}

const std::unique_ptr<IDatabase> EmbeddedDatabase::Impl::createEmpty(const std::string &dbName,
                                                                     std::unique_ptr<KeyValueStore> &kvStore)
{
    if (!fs::exists(baseDir))
        fs::create_directory(baseDir);

    std::string dbFolder = baseDir + "/" + dbName;

    return std::make_unique<EmbeddedDatabase::Impl>(dbName, dbFolder, kvStore);
}

const std::unique_ptr<IDatabase> EmbeddedDatabase::Impl::load(const std::string &dbName)
{
    std::string baseDir = ".celebi";
    std::string dbFolder = baseDir + "/" + dbName;

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

// Set or get methods

void EmbeddedDatabase::Impl::setKeyValue(const std::string &key, const std::string &value)
{
    m_keyValueStore->setKeyValue(key, value);
}

std::string EmbeddedDatabase::Impl::getKeyValue(const std::string &key)
{
    // read value from unordered map
    /*
    const auto it = m_keyValueStore.find(key);
    if (it == m_keyValueStore.end()) {
        return "";   // TODO: error handling
    }

    return it->second;
    */

    return m_keyValueStore->getKeyValue(key);
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

void EmbeddedDatabase::setKeyValue(const std::string &key, const std::string &value)
{
    m_impl->setKeyValue(key, value);
}

std::string EmbeddedDatabase::getKeyValue(const std::string &key)
{
    return m_impl->getKeyValue(key);
}
