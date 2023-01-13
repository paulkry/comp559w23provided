#include <string>
#include "Integrator.hpp"

class ModifiedMidpoint : public Integrator {
public:

    std::string getName() {
        return "modified midpoint";
    }

    void step(VectorXf& p, int n, float t, float h, VectorXf& pout, Function* derivs) {
        // TODO: Objective 5, implmement the modified midpoint (2/3) method.
        // see also efficient memory management suggestion in provided code for the Midpoint method.

    }

};