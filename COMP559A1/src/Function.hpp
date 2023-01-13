#ifndef COMP599_FUNCTION
#define COMP599_FUNCTION
#include <Eigen/Dense>
using Eigen::MatrixXf;
using Eigen::VectorXf;

/**
    * Interface for a class that computes an unknown function's derivative
    * and checks that a provided state is valid.
    * @author kry
    */
class Function {
public:
    /**
        * Evaluates derivatives for ODE integration.
        * The forces could be time varying, which is why t is provided, but
        * in the main objectives of the assignment you will note that there
        * is no time dependence for the forces.
        *
        * @param t time
        * @param p phase space state (don't modify, passed by ref for efficiency)
        * @param dpdt to be filled with the derivative
        */
    virtual void derivs(float t, VectorXf& p, VectorXf& dpdt) = 0;

};
#endif
