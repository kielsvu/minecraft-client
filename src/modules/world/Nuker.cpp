#include "outcome/Module.hpp"
#include "outcome/SDK.hpp"
#include "Sigs.hpp"
#include "core/Mem.hpp"
#include <cmath>

using namespace outcome;

namespace {

class Nuker : public Module {
public:
    Nuker() : Module(Cat::World, "Nuker", "breaks blocks in radius") {
        range = &addFloat("Range", 4.f, 1.f, 8.f);
    }

    void onTick() override {
        auto* lp = mc::g_game ? mc::g_game->localPlayer() : nullptr;
        if (!lp) return;

        mc::Vec3 p = lp->pos();
        int r = (int)ceilf(range->f);

        for (int x = -r; x <= r; ++x)
        for (int y = -r; y <= r; ++y)
        for (int z = -r; z <= r; ++z) {
            if (x*x + y*y + z*z > range->f * range->f) continue;
            breakBlock(floorf(p.x) + x, floorf(p.y) + y, floorf(p.z) + z);
        }
    }

private:
    static void breakBlock(float x, float y, float z) {
        static uintptr_t fn = 0;
        if (!fn) fn = sigs::addr("GameMode_breakBlock");
        if (!fn) return;
        mem::call<void>(fn, mc::g_game->clientInstance(),
                        (int)x, (int)y, (int)z);
    }
    Setting* range;
};

} // namespace
