#include "outcome/Module.hpp"
#include "Sigs.hpp"
#include "core/Mem.hpp"
#include <vector>

using namespace outcome;

namespace {

class Xray : public Module {
public:
    Xray() : Module(Cat::Visual, "Xray", "hides non-ore blocks") {
        diamonds = &addBool("Diamonds", true);
        gold     = &addBool("Gold", true);
        iron     = &addBool("Iron", false);
        debris   = &addBool("Debris", true);
    }

    void onEnable() override {
        // opaque-block render pass gets its alpha forced to zero; the
        // shader still runs, it just writes nothing visible
        uintptr_t pass = sigs::addr("RenderChunk_opaque");
        if (!pass) return;
        std::vector<uint8_t> nop = { 0x1F, 0x20, 0x03, 0xD5 }; // nop
        mem::patch(pass, nop);
        patched = true;
    }

    void onDisable() override {
        if (!patched) return;
        uintptr_t pass = sigs::addr("RenderChunk_opaque");
        if (pass) mem::patch(pass, original);
        patched = false;
    }

private:
    Setting* diamonds;
    Setting* gold;
    Setting* iron;
    Setting* debris;
    std::vector<uint8_t> original;
    bool patched = false;
};

} // namespace
