#include "extensions/extdatabase.h"

#include <filesystem>
#include <fstream>
#include <cstring>

#include <iostream>

namespace celebiext {

namespace fs = std::filesystem;

class FileKeyValueStore::Impl {
public:
    explicit Impl(std::string fullpath);
    const std::string getFilepathFromKey(const std::string &key, const ValueType type);
    const std::string getKeyFromFilename(const std::string &filename, const ValueType type);

    static const std::string fileExtension;
    const std::string m_fullpath;
};

const std::string FileKeyValueStore::Impl::fileExtension = ".kv";

FileKeyValueStore::Impl::Impl(const std::string fullpath)
    : m_fullpath(fullpath)
{

}

const std::string FileKeyValueStore::Impl::getFilepathFromKey(const std::string &key,
                                                              const ValueType type)
{
    std::string extension = fileExtension;
    switch (type) {
    case ValueType::STRING:
        extension = "_string" + extension;
        break;
    case ValueType::STRING_SET:
        extension = "_string_set" + extension;
        break;
    }

    return m_fullpath + "/" + key + extension;
}

const std::string FileKeyValueStore::Impl::getKeyFromFilename(const std::string &filename,
                                                              const ValueType type)
{
    // xxx_string.kv
    int extensionLength = ::strlen(".kv");
    switch (type) {
    case ValueType::STRING:
        extensionLength += ::strlen("_string");
        break;
    case ValueType::STRING_SET:
        extensionLength += ::strlen("_string_set");
        break;
    }

    return filename.substr(0, filename.length() - extensionLength);
}

FileKeyValueStore::FileKeyValueStore(const std::string &fullpath)
    : m_impl(std::make_unique<FileKeyValueStore::Impl>(fullpath))
{
    if (!fs::exists(fullpath))
        fs::create_directories(fullpath);
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
            std::string key = m_impl->getKeyFromFilename(p.path().filename(), ValueType::STRING);
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
    std::ofstream os(m_impl->getFilepathFromKey(key, ValueType::STRING),
                     std::ios::out | std::ios::trunc);

    // write value to key file
    os << value;

    // RAII, os.close()
}

void FileKeyValueStore::setKeyValue(const std::string &key,
                                    const std::unordered_set<std::string> &value)
{
    std::fstream os(m_impl->getFilepathFromKey(key, ValueType::STRING_SET),
                     std::ios::out | std::ios::trunc);

    os << value.size() << std::endl;

    for (auto &v : value) {
        os << v.length() << std::endl;
        os << v.c_str() << std::endl;
    }

    // RAII, os.close()
}

// TODO: need to fix bug,
// as the number of digits in the first line increate, the following line will be coverd
void FileKeyValueStore::appendKeyValue(const std::string &key, const std::string &value)
{
    std::fstream stream(m_impl->getFilepathFromKey(key, ValueType::STRING_SET), std::ios::out | std::ios::in);

    // read size
    stream.seekg(0, std::ios::beg);
    long entries;
    stream >> entries;

    entries++;
    stream.seekp(0, std::ios::beg);
    stream << entries << std::endl;

    stream.seekp(0, std::ios::end);
    stream << value.length() << std::endl;
    stream << value.c_str() << std::endl;
}

std::string FileKeyValueStore::getKeyValue(const std::string &key)
{
    std::ifstream is(m_impl->getFilepathFromKey(key, ValueType::STRING));
    std::string value;

    is.seekg(0, std::ios::end);
    value.reserve(is.tellg());
    is.seekg(0, std::ios::beg);

    value.assign(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());

    return value;
}

std::unique_ptr<std::unordered_set<std::string>>
FileKeyValueStore::getKeyValueSet(const std::string &key)
{
    std::string filepath = m_impl->getFilepathFromKey(key, ValueType::STRING_SET);
    if (!fs::exists(filepath))
        return std::make_unique<std::unordered_set<std::string>>();

    std::ifstream is(filepath);
    std::unordered_set<std::string> values;
    std::string value;

    // read size
    int entries;
    is >> entries;
    std::getline(is, value); // skip CRLF

    for (int i = 0; i < entries; i++) {
        std::getline(is, value); // read length, ignore now
        std::getline(is, value);
        values.insert(value);
    }

    return std::make_unique<std::unordered_set<std::string>>(values);
}

};
