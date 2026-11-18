#include "outcome/Module.hpp"
#include "outcome/SDK.hpp"
#include <cmath>

using namespace outcome;

namespace {

class AutoTool : public Module {
public:
    AutoTool() : Module(Cat::World, "AutoTool", "picks best tool per block") {}

    void onTick() override {
        auto* lp = mc::g_game ? mc::g_game->localPlayer() : nullptr;
        if (!lp) return;

        // no active break target means nothing to switch for
        uintptr_t e = reinterpret_cast<uintptr_t>(lp->entity());
        if (!*reinterpret_cast<uint8_t*>(e + 0x5C0)) return;

        int best = bestSlotFor(blockId());
        if (best >= 0) lp->selectSlot(best);
    }

private:
    static int  bestSlotFor(int id);
    static int  blockId();
};

} // namespace
