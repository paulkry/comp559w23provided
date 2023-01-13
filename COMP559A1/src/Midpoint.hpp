#include <string>
#include "Integrator.hpp"

class Midpoint : public Integrator {
public:
    std::string getName() {
        return "midpoint";
    }

    float* tmp;
    int tmplength;

    void step(VectorXf& p, int n, float t, float h, VectorXf& pout, Function* derivs) {
        // TODO: Objective 4, implement midpoint method

        // You will probably want a temporary array in this method and perhaps
        // multiple temporary arrays in other higher order explicit integrators.
        // Avoid thrashing memory by reallocating only when necessary.
        if (tmp == NULL || tmplength != n) {
            if (tmp != NULL) delete tmp;
            tmp = new float[n];
            tmplength = n;
        }

    }

};