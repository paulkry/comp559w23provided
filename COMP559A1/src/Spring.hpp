#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Eigen/Dense>
using Eigen::MatrixXf;
using Eigen::VectorXf;

#include "Particle.hpp"

/**
 * Spring class for 599 assignment 1
 * @author kry
 */
class Spring {

public:

    Particle* p1;
    Particle* p2;

    /** Spring stiffness, sometimes written k_s in equations */
    float k = 1;
    /** Spring damping (along spring direction), sometimes written k_d in equations */
    float c = 1;
    /** Rest length of this spring */
    double l0 = 0;

    /**
     * Creates a spring between two particles
     * @param p1
     * @param p2
     */
    Spring(Particle* p1, Particle* p2) {
        this->p1 = p1;
        this->p2 = p2;
        recomputeRestLength();
        p1->springs.push_back(this);
        p2->springs.push_back(this);
    }

    /**
     * Computes and sets the rest length based on the original position of the two particles
     */
    void recomputeRestLength() {
        glm::vec2 diff = p1->p0 - p2->p0;
        l0 = sqrt(diff.x * diff.x + diff.y * diff.y);
    }

    /**
     * Applies the spring force by adding a force to each particle
     */
    void apply() {
        // TODO: Objective 1, FINISH THIS CODE!

   

    }

    /** TODO: the functions below are for the backwards Euler solver */

    /**
     * Computes the force and adds it to the appropriate components of the force vector.
     * (This function is something you might use for a backward Euler integrator)
     * @param f
     */
    void addForce(VectorXf& f) {
        // TODO: Objective 8, FINISH THIS CODE for backward Euler method (probably very simlar to what you did above)

    }

    /**
     * Adds this springs contribution to the stiffness matrix
     * @param dfdx
     */
    void addDfdx(MatrixXf& dfdx) {
        // TODO: Objective 8, FINISH THIS CODE... necessary for backward euler integration

    }

    /**
     * Adds this springs damping contribution to the implicit damping matrix
     * @param dfdv
     */
    void addDfdv(MatrixXf& dfdv) {
        // TODO: Objective 8, FINISH THIS CODE... necessary for backward Euler integration

    }

};