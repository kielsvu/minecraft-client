#pragma once
#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>

namespace mem {

uintptr_t base(const char* so);
size_t    size(const char* so);
uintptr_t scan(const char* so, const char* pattern, int skip = 0);

inline uintptr_t at(uintptr_t p, size_t off) {
    return *reinterpret_cast<uintptr_t*>(p + off);
}

template <typename T>
inline T read(uintptr_t p) { return *reinterpret_cast<T*>(p); }

template <typename T>
inline void write(uintptr_t p, T v) { *reinterpret_cast<T*>(p) = v; }

template <typename R, typename... A>
inline R call(uintptr_t p, A... a) {
    return reinterpret_cast<R(*)(A...)>(p)(a...);
}

bool patch(uintptr_t addr, const std::vector<uint8_t>& bytes);
bool unprotect(uintptr_t addr, size_t len);

}
