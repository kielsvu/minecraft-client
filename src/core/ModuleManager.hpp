#pragma once
#include "outcome/Module.hpp"
#include <unordered_map>

namespace outcome {

class ModuleManager {
public:
    static ModuleManager& get();

    void registerModule(std::unique_ptr<Module> m);
    void initAll();       // construct every module
    void tickAll();
    void renderAll(void* ctx);

    Module* find(const std::string& name);
    ModuleList& all() { return modules; }

    bool save(const char* path);
    bool load(const char* path);

private:
    ModuleList modules;
    std::unordered_map<std::string, Module*> index;
};

}
