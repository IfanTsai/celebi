#ifndef __CELEBI_H__
#define __CELEBI_H__

#include <string>

#include "database.h"

namespace celebi {

/**
 * @brief The Celebi class is client factory API
 */
class Celebi
{
public:
    Celebi() = delete;

    static const std::unique_ptr<IDatabase> createEmptyDB(const std::string &dbName);
    static const std::unique_ptr<IDatabase> loadDB(const std::string &dbName);
};

}

#endif // __CELEBI_H__
