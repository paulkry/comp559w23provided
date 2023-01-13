#include <string>
#include "Integrator.hpp"


class SymplecticEuler : public Integrator {
public:
    std::string getName() {
        return "symplectic Euler";
    }

    void step(VectorXf &p, int n, float t, float h, VectorXf &pout, Function* derivs) {
        // TODO: Objective 7, complete the symplectic Euler integration method.
        // note you'll need to know how p is packed to properly implement this, so go
        // look at ParticleSystem.getPhaseSpace()
   
       
    }

};