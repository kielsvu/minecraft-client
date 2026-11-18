#pragma once
#include <cstdint>
#include <unordered_map>
#include <string>

namespace sigs {

struct Entry {
    const char* name;
    const char* pattern;
    int         skip;
};

struct Offset {
    const char* name;
    size_t      value;
};

void init(uint32_t mcVersion);

uintptr_t addr(const char* name);
size_t    off(const char* name);
std::string versionTag();

}
