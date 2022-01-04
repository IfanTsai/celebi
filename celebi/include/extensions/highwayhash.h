#ifndef __CELEBI_EXTENSION_HIGHWAYHASH_H__
#define __CELEBI_EXTENSION_HIGHWAYHASH_H__

#include "highwayhash/highwayhash.h"

#include <string>

namespace celebiext {

using namespace highwayhash;

class HighwayHash {
public:
    HighwayHash();
    HighwayHash(std::uint64_t s1, std::uint64_t s2, std::uint64_t s3, std::uint64_t s4);
    ~HighwayHash();

    std::size_t operator()(const std::string &s) const noexcept;

private:
    const HHKey m_key HH_ALIGNAS(64);
    HighwayHashCatT<HH_TARGET> *m_hh;
    HHResult64 *m_result;
};

}

#endif // __CELEBI_EXTENSION_HIGHWAYHASH_H__
