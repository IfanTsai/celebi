#include "extensions/extdatabase.h"

namespace celebiext {

class MemoryKeyValueStore::Impl {
public:
    Impl();
    explicit Impl(std::unique_ptr<KeyValueStore> &persistentStore);

    std::unordered_map<std::string, std::string>  m_keyValueStore;
    std::optional<std::unique_ptr<KeyValueStore>> m_persistentStore;
};

MemoryKeyValueStore::Impl::Impl()
    : m_keyValueStore(), m_persistentStore()
{

}

MemoryKeyValueStore::Impl::Impl(std::unique_ptr<KeyValueStore> &persistentStore)
    : m_keyValueStore(), m_persistentStore(persistentStore.release())
{

}

MemoryKeyValueStore::MemoryKeyValueStore()
    : m_impl(std::make_unique<MemoryKeyValueStore::Impl>())
{

}

MemoryKeyValueStore::MemoryKeyValueStore(std::unique_ptr<KeyValueStore> &toCache)
    : m_impl(std::make_unique<MemoryKeyValueStore::Impl>(toCache))
{

}

MemoryKeyValueStore::~MemoryKeyValueStore()
{

}

// Management methods
void MemoryKeyValueStore::loadKeysInto(std::function<void(std::string key, std::string vlaue)> cb)
{
    for (auto &it: m_impl->m_keyValueStore)
        cb(it.first, it.second);
}

void MemoryKeyValueStore::clear()
{
    m_impl->m_keyValueStore.clear();

    if (m_impl->m_persistentStore)
        m_impl->m_persistentStore->get()->clear();
}

// Set or get methods
void MemoryKeyValueStore::setKeyValue(const std::string &key, const std::string &value)
{
    m_impl->m_keyValueStore.emplace(key, value);

    // also write persistent store, if persistent store is exist
    if (m_impl->m_persistentStore)
        m_impl->m_persistentStore->get()->setKeyValue(key, value);
}

std::string MemoryKeyValueStore::getKeyValue(const std::string &key)
{
    const auto &it = m_impl->m_keyValueStore.find(key);
    if (it != m_impl->m_keyValueStore.end())
        return it->second;

    return "";
}


}
