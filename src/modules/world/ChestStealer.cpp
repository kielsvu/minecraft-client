#include "outcome/Module.hpp"
#include "outcome/SDK.hpp"
#include <chrono>

using namespace outcome;

namespace {

class ChestStealer : public Module {
public:
    ChestStealer() : Module(Cat::World, "ChestStealer", "drains opened containers") {
        delay = &addInt("Delay", 2, 1, 10);
    }

    void onTick() override {
        auto* game = mc::g_game;
        if (!game) return;
        if (!containerOpen()) return;

        auto now = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - last).count();
        if (ms < delay->i * 50) return;
        last = now;

        // one slot per tick, top-left first, skip empty
        if (slotCount() == 0) return;
        if (!slotEmpty(cursor)) { ++cursor; return; }
        shiftClick(cursor++);
        if (cursor >= slotCount()) cursor = 0;
    }

private:
    static bool containerOpen();
    static int  slotCount();
    static bool slotEmpty(int s);
    static void shiftClick(int s);

    Setting* delay;
    int cursor = 0;
    std::chrono::steady_clock::time_point last{};
};

} // namespace
