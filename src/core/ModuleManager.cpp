#include "ModuleManager.hpp"
#include <fstream>
#include <sstream>
#include <android/log.h>

#define LOG(...) __android_log_print(ANDROID_LOG_INFO, "outcome", __VA_ARGS__)

namespace outcome {

ModuleManager& ModuleManager::get() {
    static ModuleManager inst;
    return inst;
}

void ModuleManager::registerModule(std::unique_ptr<Module> m) {
    index[m->name] = m.get();
    modules.push_back(std::move(m));
}

void ModuleManager::initAll() {
    // order only matters for the gui, not execution
    extern void registerCombat();
    extern void registerMovement();
    extern void registerVisual();
    extern void registerWorld();
    extern void registerPlayer();

    registerCombat();
    registerMovement();
    registerVisual();
    registerWorld();
    registerPlayer();

    LOG("loaded %zu modules", modules.size());
}

void ModuleManager::tickAll() {
    for (auto& m : modules)
        if (m->enabled) m->onTick();
}

void ModuleManager::renderAll(void* ctx) {
    for (auto& m : modules)
        if (m->enabled) m->onRender(ctx);
}

Module* ModuleManager::find(const std::string& n) {
    auto it = index.find(n);
    return it == index.end() ? nullptr : it->second;
}

bool ModuleManager::save(const char* path) {
    std::ofstream f(path, std::ios::trunc);
    if (!f) return false;
    for (auto& m : modules) {
        f << m->name << "=" << (m->enabled ? 1 : 0) << "\n";
        for (auto& s : m->settings)
            f << m->name << "." << s.name << "="
              << s.b << "|" << s.f << "|" << s.i << "\n";
    }
    return true;
}

bool ModuleManager::load(const char* path) {
    std::ifstream f(path);
    if (!f) return false;
    std::string line;
    while (std::getline(f, line)) {
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string k = line.substr(0, eq);
        std::string v = line.substr(eq + 1);

        if (k.find('.') == std::string::npos) {
            if (auto* m = find(k)) { if (v == "1") m->enable(); else m->disable(); }
            continue;
        }
        std::string mod = k.substr(0, k.find('.'));
        std::string set = k.substr(k.find('.') + 1);
        auto* m = find(mod);
        if (!m) continue;
        for (auto& s : m->settings) {
            if (s.name != set) continue;
            std::stringstream ss(v);
            std::string a, b, c;
            std::getline(ss, a, '|');
            std::getline(ss, b, '|');
            std::getline(ss, c, '|');
            s.b = a == "1";
            s.f = std::stof(b.empty() ? "0" : b);
            s.i = std::stoi(c.empty() ? "0" : c);
        }
    }
    return true;
}

} // namespace outcome
