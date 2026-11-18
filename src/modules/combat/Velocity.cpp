#include "outcome/Module.hpp"
#include "ModuleManager.hpp"

using namespace outcome;

namespace {

class Velocity : public Module {
public:
    Velocity() : Module(Cat::Combat, "Velocity", "reduces knockback") {
        hor = &addFloat("Horizontal", 1.f, 0.f, 1.f);
        ver = &addFloat("Vertical", 0.f, 0.f, 1.f);
    }
    // actual work is in hkSetVelocity, module only carries the settings

private:
    Setting* hor;
    Setting* ver;
};

} // namespace
