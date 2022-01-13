#ifndef __CELEBI_EXTENSION_EXTQUERY_H__
#define __CELEBI_EXTENSION_EXTQUERY_H__

#include "query.h"

namespace celebiext {

using namespace celebi;

class DefaultQueryResult : public IQueryResult {
public:
    DefaultQueryResult();
    DefaultQueryResult(std::unique_ptr<std::unordered_set<std::string>> recordKeys);
    virtual ~DefaultQueryResult() = default;

    virtual const std::unique_ptr<std::unordered_set<std::string>> recordKeys() override;

private:
    std::unique_ptr<std::unordered_set<std::string>> m_recordKeys;
};

}

#endif // __CELEBI_EXTENSION_EXTQUERY_H__
