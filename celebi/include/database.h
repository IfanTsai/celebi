#ifndef __CELEBI_DATABASE_H__
#define __CELEBI_DATABASE_H__

#include <string>
#include <memory>

namespace celebi {

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
    static const std::unique_ptr<IDatabase> createEmpty(const std::string &dbName);
    static const std::unique_ptr<IDatabase> load(const std::string &dbName);
    virtual void destroy() = 0;

    // Set or get methods
    virtual void setKeyValue(const std::string &key, const std::string &value) = 0;
    virtual std::string getKeyValue(const std::string &key) = 0;
};

}

#endif // __CELEBI_DATABASE_H__
