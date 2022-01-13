#ifndef __CELEBI_QUERY_H__
#define __CELEBI_QUERY_H__

#include <memory>
#include <unordered_set>
#include <string>

namespace celebi {

class Query {
public:
    Query() = default;
    virtual ~Query() = default;
};

class BucketQuery : public Query {
public:
    BucketQuery(const std::string &bucket);
    virtual ~BucketQuery();

    virtual const std::string bucket() const;

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

class IQueryResult {
public:
    IQueryResult() = default;
    virtual ~IQueryResult() = default;

    virtual const std::unique_ptr<std::unordered_set<std::string>> recordKeys() = 0;
};

}

#endif // __CELEBI_QUERY_H__
