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
    virtual ~Impl();

    virtual const std::string getDirectory() const;

    // Management methods
    static const std::unique_ptr<IDatabase> createEmpty(const std::string &dbName);
    static const std::unique_ptr<IDatabase> load(const std::string &dbName);
    virtual void destroy();

    // Set or get methods
    virtual void setKeyValue(const std::string &key, const std::string &value);
    virtual std::string getKeyValue(const std::string &key);

private:
    const std::string getFilepathFromKey(const std::string &key);
    const std::string getKeyFromFilename(const std::string &filename);

    static const std::string baseDir;
    static const std::string fileExtension;
    std::string m_name;
    std::string m_fullpath;
    std::unordered_map<std::string, std::string> m_keyValueStore;
};

const std::string EmbeddedDatabase::Impl::baseDir = ".celebi";
const std::string EmbeddedDatabase::Impl::fileExtension = ".kv";

EmbeddedDatabase::Impl::Impl(const std::string &dbName, const std::string &fullpath)
    : m_name(dbName), m_fullpath(fullpath)
{
    if (!fs::exists(m_fullpath)) {
        return;
    }

    // load files from disk to memory
    for (auto &p : fs::directory_iterator(m_fullpath)) {
        if (p.exists() && p.is_regular_file() && p.path().extension() == fileExtension) {
            // read value from file
            std::ifstream is(p.path());
            std::string value;

            is.seekg(0, std::ios::end);
            value.reserve(is.tellg());
            is.seekg(0, std::ios::beg);

            value.assign(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());

            // insert key-value to unordered map
            std::string key = getKeyFromFilename(p.path().filename());
            m_keyValueStore.insert({ key, value });
        }
    }
}

EmbeddedDatabase::Impl::~Impl()
{
}

const std::string EmbeddedDatabase::Impl::getFilepathFromKey(const std::string &key)
{
    return m_fullpath + "/" + key + fileExtension;
}

const std::string EmbeddedDatabase::Impl::getKeyFromFilename(const std::string &filename)
{
    return filename.substr(0, filename.find_last_of('.'));
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

const std::unique_ptr<IDatabase> EmbeddedDatabase::Impl::load(const std::string &dbName)
{
    std::string baseDir = ".celebi";
    std::string dbFolder = baseDir + "/" + dbName;

    return std::make_unique<EmbeddedDatabase::Impl>(dbName, dbFolder);
}

void EmbeddedDatabase::Impl::destroy()
{
   if (fs::exists(m_fullpath))
       fs::remove_all(m_fullpath);

   m_keyValueStore.clear();
}

const std::string EmbeddedDatabase::Impl::getDirectory() const
{
    return m_fullpath;
}

// Set or get methods

void EmbeddedDatabase::Impl::setKeyValue(const std::string &key, const std::string &value)
{
    std::ofstream os(getFilepathFromKey(key), std::ios::out | std::ios::trunc);

    // write value to key file
    os << value;

    // also wirte key-value to unordered map
    m_keyValueStore.insert({ key, value });

    // RAII, os.close()
}

std::string EmbeddedDatabase::Impl::getKeyValue(const std::string &key)
{
    // read value from unordered map
    const auto it = m_keyValueStore.find(key);
    if (it == m_keyValueStore.end()) {
        return "";   // TODO: error handling
    }

    return it->second;
#if 0
    std::ifstream is(getFilepathFromKey(key));
    std::string value;

    is.seekg(0, std::ios::end);
    value.reserve(is.tellg());
    is.seekg(0, std::ios::beg);

    value.assign(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());

    return value;
#endif
}


/*
 ****************************************************************************
 * High level database client API implementation below
 ****************************************************************************
 */

EmbeddedDatabase::EmbeddedDatabase(const std::string &dbName, const std::string &fullpath)
    : mImpl(std::make_unique<EmbeddedDatabase::Impl>(Impl(dbName, fullpath)))
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

const std::unique_ptr<IDatabase> EmbeddedDatabase::load(const std::string &dbName)
{
    return EmbeddedDatabase::Impl::load(dbName);
}

void EmbeddedDatabase::destroy()
{
    mImpl->destroy();
}

const std::string EmbeddedDatabase::getDirectory() const
{
    return mImpl->getDirectory();
}

void EmbeddedDatabase::setKeyValue(const std::string &key, const std::string &value)
{
    mImpl->setKeyValue(key, value);
}

std::string EmbeddedDatabase::getKeyValue(const std::string &key)
{
    return mImpl->getKeyValue(key);
}
