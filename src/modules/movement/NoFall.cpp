#include "outcome/Module.hpp"
#include "ModuleManager.hpp"

using namespace outcome;

namespace {

// real logic is the Player_hurt detour in Hooks.cpp, this carries config
class NoFall : public Module {
public:
    NoFall() : Module(Cat::Movement, "NoFall", "cancels fall damage") {
        thresh = &addFloat("Threshold", 3.f, 0.f, 20.f);
    }
private:
    Setting* thresh;
};

} // namespace
