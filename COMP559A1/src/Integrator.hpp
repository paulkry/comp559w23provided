#ifndef COMP599_INTEGRATOR
#define COMP599_INTEGRATOR
#include <string>

#include "Function.hpp"

/**
 * Interface for a numerical integration method
 * 
 * Note that the state is stored in a variable called p, but other good variable names 
 * would have been y as seen in Numerical Recipies, or q as used in class for a system 
 * with changed variables or some other representation of state.  Here we use p because
 * in this assignment you should be thinking of it as the phase space state.  
 * 
 * The reason that y is sometime used is because you can draw a graph with time in the 
 * horizontal x axis, then y would be your normal vertical axis.  This is also the conventional variable
 * name in the numerical recipes book.  However, you will use p.
 * 
 * p (phase space state)
 * ^
 * |
 * | ,--._/ trajectory
 * |/
 * |
 * +-------> t (time)
 *
 * See the NR book online, chapter 17, for additional information on integration of ODEs:
 * http://www.nrbook.com/nr3/
 * 
 * @author kry
 */

class Integrator {

public:
    /**
     * @return the name of this numerical integration method
     */
    virtual std::string getName() = 0;

    /**
     * Advances the system at t by h
     * @param p The state at time h (don't modify, passed by ref for efficiency)
     * @param n The dimension of the state (i.e., p.length)
     * @param t The current time (in case the derivs function is time dependent)
     * @param h The step size
     * @param pout  The state of the system at time t+h
     * @param derivs The object which computes the derivative of the system state
     */
	virtual void step(VectorXf& p, int n, float t, float h, VectorXf& pout, Function* derivs) = 0;
};
#endif