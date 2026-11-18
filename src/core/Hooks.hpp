#pragma once
#include <cstdint>
#include <functional>
#include <unordered_map>

namespace hooks {

struct Hook {
    const char* name;
    void*       target;
    void*       detour;
    void**      original;
};

bool install(const char* name, void* target, void* detour, void** original);
bool remove(const char* name);
void installAll();
void removeAll();

} // namespace hooks
