#include <string>
#include "Integrator.hpp"

class RK4 : public Integrator {
public:
    std::string getName() {
        return "RK4";
    }

    void step(VectorXf& p, int n, float t, float h, VectorXf& pout, Function* derivs) {
        // TODO: Objective 6, implement the RK4 integration method
        // see also efficient memory management suggestion in provided code for the Midpoint method.

    }
};