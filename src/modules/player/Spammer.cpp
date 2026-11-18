#include "outcome/Module.hpp"
#include "Sigs.hpp"
#include "core/Mem.hpp"
#include <chrono>

using namespace outcome;

namespace {

class Spammer : public Module {
public:
    Spammer() : Module(Cat::Player, "Spammer", "repeats a chat message") {
        interval = &addInt("Interval", 30, 1, 300);
        randomize = &addBool("Randomize", false);
    }

    void onTick() override {
        auto now = std::chrono::steady_clock::now();
        auto sec = std::chrono::duration_cast<std::chrono::seconds>(now - last).count();
        if (sec < interval->i) return;
        last = now;

        static const char* msgs[] = {
            "gg", "nice", "ez", "outcome on top", "lag?"
        };
        static int idx = 0;

        const char* text = randomize->b ? msgs[rand() % 5] : msgs[0];
        sendChat(text);
    }

private:
    static void sendChat(const char* text) {
        static uintptr_t fn = 0;
        if (!fn) fn = sigs::addr("Player_chat");
        if (!fn) return;
        mem::call<void>(fn, mc::g_game->localPlayer(), text);
    }

    Setting* interval;
    Setting* randomize;
    std::chrono::steady_clock::time_point last{};
};

} // namespace
