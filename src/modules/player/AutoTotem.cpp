#include "outcome/Module.hpp"
#include "outcome/SDK.hpp"
#include "Sigs.hpp"
#include "core/Mem.hpp"

using namespace outcome;

namespace {

class AutoTotem : public Module {
public:
    AutoTotem() : Module(Cat::Player, "AutoTotem", "keeps a totem in offhand") {
        hp = &addFloat("Health", 10.f, 1.f, 20.f);
    }

    void onTick() override {
        auto* lp = mc::g_game ? mc::g_game->localPlayer() : nullptr;
        if (!lp) return;
        if (lp->health() > hp->f) return;
        if (offhandIsTotem()) return;

        int slot = findInInventory(0x1D1);
        if (slot < 0) return;
        moveToOffhand(slot);
    }

private:
    static bool offhandIsTotem();
    static int  findInInventory(int itemId);
    static void moveToOffhand(int slot);

    Setting* hp;
};

} // namespace
