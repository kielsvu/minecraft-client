#include "Mem.hpp"
#include <dlfcn.h>
#include <link.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <android/log.h>

#define LOG(...) __android_log_print(ANDROID_LOG_INFO, "outcome", __VA_ARGS__)

namespace mem {

static std::vector<uint8_t> hexToBytes(const char* sig) {
    std::vector<uint8_t> out;
    auto hex = [](char c) -> int {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        return -1;
    };
    for (size_t i = 0; sig[i] && sig[i + 1]; ) {
        if (sig[i] == ' ') { ++i; continue; }
        if (sig[i] == '?') { out.push_back(0); ++i; continue; }
        int h = hex(sig[i]), l = hex(sig[i + 1]);
        if (h < 0 || l < 0) break;
        out.push_back((uint8_t)((h << 4) | l));
        i += 2;
    }
    return out;
}

uintptr_t base(const char* so) {
    void* h = dlopen(so, RTLD_NOW | RTLD_NOLOAD);
    if (!h) { LOG("no handle for %s", so); return 0; }
    auto b = reinterpret_cast<uintptr_t>(dlsym(h, "_ZN6PlayerC1Ev"));
    return b ? b & ~0xFFFULL : 0;
}

size_t size(const char* so) {
    void* h = dlopen(so, RTLD_NOW | RTLD_NOLOAD);
    if (!h) return 0;

    size_t total = 0;
    struct Ctx { void* h; size_t* out; } ctx{ h, &total };

    dl_iterate_phdr([](dl_phdr_info* info, size_t, void* data) -> int {
        auto* c = static_cast<Ctx*>(data);
        if (strstr(info->dlpi_name, "minecraftpe") == nullptr) return 0;
        for (int i = 0; i < info->dlpi_phnum; ++i) {
            const auto& ph = info->dlpi_phdr[i];
            if (ph.p_type != PT_LOAD) continue;
            size_t end = info->dlpi_addr + ph.p_vaddr + ph.p_memsz;
            if (end > *c->out) *c->out = end;
        }
        return 1;
    }, &ctx);

    return total ? total - base(so) : 0;
}

uintptr_t scan(const char* so, const char* pattern, int skip) {
    auto bytes = hexToBytes(pattern);
    if (bytes.empty()) return 0;

    uintptr_t start = base(so);
    size_t    len   = size(so);
    if (!start || !len) return 0;

    auto* p = reinterpret_cast<uint8_t*>(start);
    int   hits = 0;

    for (size_t i = 0; i + bytes.size() < len; ++i) {
        bool ok = true;
        for (size_t j = 0; j < bytes.size(); ++j) {
            if (bytes[j] && p[i + j] != bytes[j]) { ok = false; break; }
        }
        if (ok && hits++ == skip) return start + i;
    }
    return 0;
}

bool unprotect(uintptr_t addr, size_t len) {
    long page = sysconf(_SC_PAGESIZE);
    uintptr_t begin = addr & ~(page - 1);
    uintptr_t end   = (addr + len + page - 1) & ~(page - 1);
    return mprotect(reinterpret_cast<void*>(begin), end - begin,
                    PROT_READ | PROT_WRITE | PROT_EXEC) == 0;
}

bool patch(uintptr_t addr, const std::vector<uint8_t>& bytes) {
    if (!addr) return false;
    if (!unprotect(addr, bytes.size())) { LOG("mprotect failed @ %p", (void*)addr); return false; }
    memcpy(reinterpret_cast<void*>(addr), bytes.data(), bytes.size());
    __builtin___clear_cache(reinterpret_cast<char*>(addr),
                            reinterpret_cast<char*>(addr) + bytes.size());
    return true;
}

}
