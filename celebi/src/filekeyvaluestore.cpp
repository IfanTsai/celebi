#include "extensions/extdatabase.h"

#include <filesystem>
#include <fstream>

namespace celebiext {

namespace fs = std::filesystem;

class FileKeyValueStore::Impl {
public:
    explicit Impl(std::string fullpath);
    const std::string getFilepathFromKey(const std::string &key);
    const std::string getKeyFromFilename(const std::string &filename);

    static const std::string fileExtension;
    const std::string m_fullpath;
};

const std::string FileKeyValueStore::Impl::fileExtension = ".kv";

FileKeyValueStore::Impl::Impl(const std::string fullpath)
    : m_fullpath(fullpath)
{

}

const std::string FileKeyValueStore::Impl::getFilepathFromKey(const std::string &key)
{
    return m_fullpath + "/" + key + fileExtension;
}

const std::string FileKeyValueStore::Impl::getKeyFromFilename(const std::string &filename)
{
    return filename.substr(0, filename.find_last_of('.'));
}

FileKeyValueStore::FileKeyValueStore(const std::string &fullpath)
    : m_impl(std::make_unique<FileKeyValueStore::Impl>(fullpath))
{
    if (!fs::exists(fullpath))
        fs::create_directory(fullpath);
}

FileKeyValueStore::~FileKeyValueStore()
{

}

// Management methods
void FileKeyValueStore::loadKeysInto(std::function<void(std::string key, std::string vlaue)> cb)
{
    for (auto &p : fs::directory_iterator(m_impl->m_fullpath)) {
        if (p.exists() && p.is_regular_file() && p.path().extension() == m_impl->fileExtension) {
            // read value from file
            std::ifstream is(p.path());
            std::string value;

            is.seekg(0, std::ios::end);
            value.reserve(is.tellg());
            is.seekg(0, std::ios::beg);

            value.assign(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());

            // load key-value into through callback
            std::string key = m_impl->getKeyFromFilename(p.path().filename());
            cb(key, value);
        }
    }
}

void FileKeyValueStore::clear()
{
    if (fs::exists(m_impl->m_fullpath))
        fs::remove_all(m_impl->m_fullpath);
}

// Set or get methods
void FileKeyValueStore::setKeyValue(const std::string &key, const std::string &value)
{
    std::ofstream os(m_impl->getFilepathFromKey(key), std::ios::out | std::ios::trunc);

    // write value to key file
    os << value;

    // RAII, os.close()
}

std::string FileKeyValueStore::getKeyValue(const std::string &key)
{
    std::ifstream is(m_impl->getFilepathFromKey(key));
    std::string value;

    is.seekg(0, std::ios::end);
    value.reserve(is.tellg());
    is.seekg(0, std::ios::beg);

    value.assign(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());

    return value;
}

};
