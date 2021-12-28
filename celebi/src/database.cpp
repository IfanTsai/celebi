#include "database.h"
#include "extensions/extdatabase.h"

#include <string>
#include <fstream>
#include <filesystem>

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
    virtual void destroy() const;

    // Set or get methods
    virtual void setKeyValue(const std::string &key, const std::string &value);
    virtual std::string getKeyValue(const std::string &key);

private:
    std::string m_name;
    std::string m_fullpath;
};


EmbeddedDatabase::Impl::Impl(const std::string &dbName, const std::string &fullpath)
    : m_name(dbName), m_fullpath(fullpath)
{

}

EmbeddedDatabase::Impl::~Impl()
{
}

// Management methods
const std::unique_ptr<IDatabase> EmbeddedDatabase::Impl::createEmpty(const std::string &dbName)
{
    std::string baseDir = ".celebi";
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

void EmbeddedDatabase::Impl::destroy() const
{
   if (fs::exists(m_fullpath))
       fs::remove_all(m_fullpath);
}

const std::string EmbeddedDatabase::Impl::getDirectory() const
{
    return m_fullpath;
}

void EmbeddedDatabase::Impl::setKeyValue(const std::string &key, const std::string &value)
{
    std::ofstream os(m_fullpath + "/" + key + "_string.kv", std::ios::out | std::ios::trunc);

    os << value;

    // RAII, os.close()
}

std::string EmbeddedDatabase::Impl::getKeyValue(const std::string &key)
{
    std::ifstream is(m_fullpath + "/" + key + "_string.kv");
    std::string value;

    is.seekg(0, std::ios::end);
    value.reserve(is.tellg());
    is.seekg(0, std::ios::beg);

    value.assign(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());

    return value;
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

void EmbeddedDatabase::destroy() const
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
