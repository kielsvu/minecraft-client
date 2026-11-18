#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace outcome {

enum class Cat { Combat, Movement, Visual, World, Player, Misc };

struct Setting {
    std::string name;
    float       f    = 0.f;
    float       fmin = 0.f, fmax = 0.f;
    bool        b    = false;
    int         i    = 0;
    int         imin = 0, imax = 0;
};

class Module {
public:
    Module(Cat c, std::string name, std::string desc)
        : cat(c), name(std::move(name)), desc(std::move(desc)) {}
    virtual ~Module() = default;

    virtual void onEnable()  {}
    virtual void onDisable() {}
    virtual void onTick()    {}
    virtual void onRender(void*) {}

    Setting& addFloat(const char* n, float v, float lo, float hi) {
        settings.push_back({n, v, lo, hi});
        return settings.back();
    }
    Setting& addBool(const char* n, bool v) {
        Setting s; s.name = n; s.b = v;
        settings.push_back(s);
        return settings.back();
    }
    Setting& addInt(const char* n, int v, int lo, int hi) {
        Setting s; s.name = n; s.i = v; s.imin = lo; s.imax = hi;
        settings.push_back(s);
        return settings.back();
    }

    void toggle() { enabled ? disable() : enable(); }
    void enable()  { if (enabled) return; enabled = true;  onEnable();  }
    void disable() { if (!enabled) return; enabled = false; onDisable(); }

    Cat          cat;
    std::string  name, desc;
    bool         enabled = false;
    int          keybind = 0;
    std::vector<Setting> settings;
};

using ModuleList = std::vector<std::unique_ptr<Module>>;

}
