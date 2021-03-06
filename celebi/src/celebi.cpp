#include "celebi.h"
#include "extensions/extdatabase.h"

using namespace celebi;
using namespace celebiext;

const std::unique_ptr<IDatabase> Celebi::createEmptyDB(const std::string &dbName)
{
    return EmbeddedDatabase::createEmpty(dbName);
}

const std::unique_ptr<IDatabase> Celebi::createEmptyDB(const std::string &dbname,
                                                       std::unique_ptr<KeyValueStore>& kvStore)
{
  return EmbeddedDatabase::createEmpty(dbname, kvStore);
}

const std::unique_ptr<IDatabase> Celebi::loadDB(const std::string &dbName)
{
   return EmbeddedDatabase::load(dbName);
}
