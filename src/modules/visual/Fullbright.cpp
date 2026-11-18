#include "outcome/Module.hpp"
#include "Sigs.hpp"
#include "core/Mem.hpp"

using namespace outcome;

namespace {

class Fullbright : public Module {
public:
    Fullbright() : Module(Cat::Visual, "Fullbright", "removes darkness") {
        level = &addFloat("Light", 15.f, 1.f, 15.f);
    }

    void onTick() override {
        // the lightmap sampler reads this float every frame
        static uintptr_t addr = 0;
        if (!addr) addr = sigs::addr("Lightmap_gamma");
        if (!addr) return;
        mem::write<float>(addr, level->f);
    }

private:
    Setting* level;
};

} // namespace
