#include "query.h"
#include "extensions/extquery.h"

#include <string>

using namespace celebi;
using namespace celebiext;

class BucketQuery::Impl {
public:
    explicit Impl(const std::string &bucket);
    ~Impl() = default;

    std::string m_bucket;
};

BucketQuery::Impl::Impl(const std::string& bucket)
    : m_bucket(bucket)
{

}

BucketQuery::BucketQuery(const std::string& bucket)
    : m_impl(std::make_unique<Impl>(bucket))
{

}

BucketQuery::~BucketQuery()
{

}

const std::string BucketQuery::bucket() const
{
    return m_impl->m_bucket;
}

DefaultQueryResult::DefaultQueryResult()
    : m_recordKeys()
{

}

DefaultQueryResult::DefaultQueryResult(std::unique_ptr<std::unordered_set<std::string>> recordKeys)
    : m_recordKeys(std::move(recordKeys))
{

}

const std::unique_ptr<std::unordered_set<std::string>>
DefaultQueryResult::recordKeys()
{
    return std::move(m_recordKeys);
}
