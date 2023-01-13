#include <string>
#include "Integrator.hpp"
#include "Function.hpp"

/**
 * TODO: finish this class!
 */
class ForwardEuler : public Integrator {
public:

    std::string getName() {
        return "Forward Euler";
    }

    /**
     * Advances the system at t by h
     * @param p The state at time h (don't modify, passed by ref for speed)
     * @param n The dimension of the state (i.e., p.length)
     * @param t The current time (in case the derivs function is time dependent)
     * @param h The step size
     * @param pout  The state of the system at time t+h
     * @param derivs The object which computes the derivative of the system state
     */
    void step(VectorXf& p, int n, float t, float h, VectorXf& pout, Function* derivs) {
        // TODO: Objective 3, implement the forward Euler method

    }

};