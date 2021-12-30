#ifndef __CELEBI_EXTENSION_EXTDATABASE_H__
#define __CELEBI_EXTENSION_EXTDATABASE_H__

#include "celebi.h"

namespace celebiext {

using namespace celebi;

/**
 * @brief The EmbeddedDatabase class is server proxy API
 */
class EmbeddedDatabase: public IDatabase {
public:
    EmbeddedDatabase(const std::string &dbName, const std::string &fullpath);
    virtual ~EmbeddedDatabase();

    virtual const std::string getDirectory() const;

    // Management methods
    static const std::unique_ptr<IDatabase> createEmpty(const std::string &dbName);
    static const std::unique_ptr<IDatabase> load(const std::string &dbName);
    virtual void destroy();

    // Set or get methods
    virtual void setKeyValue(const std::string &key, const std::string &value);
    virtual std::string getKeyValue(const std::string &key);

private:
    class Impl;
    std::unique_ptr<Impl> mImpl;   // server side hidden implemention in this
};

}


#endif // __CELEBI_EXTENSION_EXTDATABASE_H__
