#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLSL.h"

#include "Particle.hpp"
#include "Spring.hpp"
#include "Integrator.hpp"
#include "ForwardEuler.hpp"
#include "Midpoint.hpp"
#include "ModifiedMidpoint.hpp"
#include "RK4.hpp"
#include "SymplecticEuler.hpp"
#include "Filter.hpp"

#include <Eigen/Dense>
using Eigen::MatrixXf;
using Eigen::VectorXf;

using namespace std;
/**
 * Implementation of a simple particle system
 * @author kry
 */
class ParticleSystem : public Function, Filter {
    
public:
    std::vector<Particle*> particles;
    std::vector<Spring*> springs;
    
    /**
     * Creates an empty particle system
     */
    ParticleSystem() {
        // do nothing
    }

    /**
     * Creates one of a number of simple test systems.
     * @param which
     */
    void createSystem( int which ) {
        
        if ( which == 1) {        
            glm::vec2 p( 100, 100 );
            glm::vec2 d( 20, 0 );            
            Particle* p1 = new Particle( p.x - d.y, p.y + d.x, 0, 0 );
            p1->index = particles.size();
            particles.push_back( p1 );
            Particle* p2 = new Particle( p.x + d.y, p.y - d.x, 0, 0 );
            p2->index = particles.size();
            particles.push_back( p2 );
            springs.push_back( new Spring ( p1, p2 ) );           
            p1->pinned = true;
            p2->pinned = true;            
            p += d;
            p += d;                    
            int N = 10;
            for (int i = 1; i < N; i++ ) {                
                //d.set( 20*Math.cos(i*Math.PI/N), 20*Math.sin(i*Math.PI/N) );                
                Particle* p3 = new Particle( p.x - d.y, p.y + d.x, 0, 0 );
                p3->index = particles.size();
                particles.push_back( p3 );
                Particle* p4 = new Particle( p.x + d.y, p.y - d.x, 0, 0 );
                p4->index = particles.size();
                particles.push_back( p4 );
                springs.push_back( new Spring (p3, p1 ) );
                springs.push_back( new Spring (p3, p2 ) );
                springs.push_back( new Spring (p3, p2 ) );
                springs.push_back( new Spring (p4, p1 ) );
                springs.push_back( new Spring (p4, p2 ) );
                springs.push_back( new Spring (p4, p3 ) );
                p1 = p3;
                p2 = p4;                
                p += d;
                p += d;            
            }
        } else if ( which == 2) {
            Particle* p1 = new Particle( 320, 100, 0, 0 );
            p1->index = particles.size();
            particles.push_back( p1 );
            Particle* p2 = new Particle( 320, 200, 0, 0 );
            p2->index = particles.size();
            particles.push_back( p2 );
            p1->pinned = true;
            springs.push_back( new Spring( p1, p2 ) );
        } else if ( which == 3 ) {
            float ypos = 100;
            Particle* p0 = NULL;
            Particle* p1 = new Particle(320, ypos, 0, 0);
            Particle* p2;
            p1->index = particles.size();
            p1->pinned = true;            
            particles.push_back( p1 );
            int N = 10;
            for ( int i = 0; i < N; i++ ) {
                ypos += 20;
                p2 = new Particle( 320, ypos, 0, 0 );
                p2->index = particles.size();
                particles.push_back( p2 );
                springs.push_back( new Spring( p1, p2 ) );                
                // Hum.. this is not great in comparison to a proper bending energy...
                // use Maple to generate some code though, as it is painful to write by hand! :(
                if ( p0 != NULL ) springs.push_back( new Spring( p2, p0 ) );
                p0 = p1;
                p1 = p2;
            }
        }
    }
   
    
    /**
     * Resets the positions of all particles
     */
    void resetParticles() {
        for ( Particle* p : particles ) {
            p->reset();
        }
        time = 0;
    }
    
    /**
     * Deletes all particles
     */
    void clearParticles() {
        for (Particle* p : particles) { delete p; }
        particles.clear();
        for (Spring* s : springs) { delete s; }
        springs.clear();
    }
    
    /**
     * Gets the phase space state of the particle system
     * @param phaseSpaceState
     */
    void getPhaseSpace( VectorXf& phaseSpaceState ) {
        int count = 0;
        for ( Particle* p : particles ) {
            phaseSpaceState[count++] = p->p.x;
            phaseSpaceState[count++] = p->p.y;
            phaseSpaceState[count++] = p->v.x;
            phaseSpaceState[count++] = p->v.y;
        }
    }
    
    /**
     * Gets the dimension of the phase space state
     * (particles * 2 dimensions * 2 for velocity and position)
     * @return dimension
     */
    int getPhaseSpaceDim() {        
        return particles.size() * 4;
    }
    
    /**
     * Sets the phase space state of the particle system
     * @param phaseSpaceState
     */
    void setPhaseSpace( VectorXf& phaseSpaceState ) {
        int count = 0;
        for ( Particle* p : particles ) {
            if ( p->pinned ) {
                count += 4;
            } else {
                p->p.x = phaseSpaceState[count++];
                p->p.y = phaseSpaceState[count++];
                p->v.x = phaseSpaceState[count++];
                p->v.y = phaseSpaceState[count++];
            }
        }
    }
    
    /**
     * Fixes positions and velocities after a step to deal with collisions 
     */
    void postStepFix() {
        for ( Particle* p : particles ) {
            if ( p->pinned ) {
                p->v = glm::vec2(0,0);
            }
        }
        // do wall collisions
        float r = restitution;
        for ( Particle* p : particles ) {            
            if ( p->p.x <= 0 ) {
                p->p.x = 0;
                if ( p->v.x < 0 ) p->v.x = - p->v.x * r;
                if ( p->f.x < 0 ) p->f.x = 0;                
            }
            if ( p->p.x >= width ) {
                p->p.x = width;
                if (p->v.x > 0 ) p->v.x = - p->v.x * r;
                if (p->f.x > 0 ) p->f.x = 0;
            } 
            
            if ( p->p.y >= height ) {
                p->p.y = height;
                if ( p->v.y > 0 ) p->v.y = - p->v.y * r;
                if ( p->f.y > 0 ) p->f.y = 0;
            } 
            if ( p->p.y <= 0 ) {
                p->p.y = 0;
                if ( p->v.y < 0 ) p->v.y = - p->v.y * r;
                if ( p->f.y < 0 ) p->f.y = 0;
            }
        }
    }
    
    /** Elapsed simulation time */
    double time = 0;

    /** The explicit integrator to use, if not performing backward Euler implicit integration */
    Integrator* integrator;
    
    VectorXf state;
    VectorXf stateOut;

    // these get created in init() and are probably useful for Backward Euler computations
    //ConjugateGradientMTJ CG;
    MatrixXf A;
    MatrixXf dfdx;
    MatrixXf dfdv;
    VectorXf deltaxdot;
    VectorXf b;
    VectorXf f;
    VectorXf xdot;
    
    /**
     * Fills in the provided vector with the particle velocities.
     * @param xd
     */
    void getVelocities(VectorXf& xd) {
        for ( Particle* p : particles ) {
            int j = p->index * 2;
            if( p->pinned ) {
                xd[j] = 0;
                xd[j + 1]= 0;
            } else {
                xd[j] = p->v.x;
                xd[j + 1] = p->v.y;
            }
        }       
    }

    /**
     * Sets the velocities of the particles given a vector
     * @param xd
     */
    void setVelocities(VectorXf& xd) {
        for ( Particle* p : particles ) {
            int j = p->index * 2;
            if( p->pinned ) {
                p->v = glm::vec2(0, 0);
            } else {
                p->v.x = xd[j];
                p->v.y = xd[j + 1];
            }
        }
    }
    
    /**
     *  Evaluates derivatives for ODE integration.
     * @param t time 
     * @param p phase space state (don't modify)
     * @param dydt to be filled with the derivative
     */
    void derivs(float t, VectorXf& p, VectorXf& dpdt) {
        // set particle positions to given values
        setPhaseSpace( p );
        
        // TODO: Objective 2, for explicit integrators, compute forces, and accelerations, and set dpdt
        
        

    }
    
    /** Time in seconds that was necessary to advance the system */
    float computeTime;
    
    /**
     * Advances the state of the system
     * @param elapsed
     */
    void advanceTime( float elapsed ) {
        for (Spring* s : springs) {
            s->k = springStiffness;
            s->c = springDamping;
        }
            
        int n = getPhaseSpaceDim();
        
        double now = glfwGetTime();
        
        if (useExplicitIntegration) {
            if ( n != state.size() ) {
                state.resize(n);;
                stateOut.resize(n);
            }
            // TODO: See explicit stepping here
            getPhaseSpace(state);         
            integrator->step( state, n, time, elapsed, stateOut, this);                
            setPhaseSpace(stateOut);
        } else {        
            if ( f.size() != n ) {
                init();
            }
            
            // TODO: Objective 8, your backward Euler implementation will go here!
            // Note that the init() method called above creates a bunch of very 
            // useful MTJ working variables for you, and the ConjugateGradientMTJ object.
            // Go look at that code now!
            
            
            
            
        }
        time = time + elapsed;
        postStepFix();
        computeTime = (glfwGetTime() - now);
    }
    
    void filter(VectorXf& v) {
        for ( Particle* p : particles ) {
            if ( !p->pinned ) continue;
            v[ p->index*2+0] = 0;
            v[ p->index*2+1] = 0;
        }
    }

    /**
     * Creates a new particle and adds it to the system
     * @param x
     * @param y
     * @param vx
     * @param vy
     * @return the new particle
     */
    Particle* createParticle( float x, float y, float vx, float vy ) {
        Particle* p = new Particle( x, y, vx, vy );
        p->index = particles.size();
        particles.push_back( p );
        return p;
    }
    
    void remove( Particle* p ) {
    	for ( Spring* s : p->springs ) {
    		Particle* other = s->p1 == p ? s->p2 : s->p1; 
            // remove from the other particle's spring list
            vector<Spring*>::iterator osi = std::remove(other->springs.begin(), other->springs.end(), s);
            other->springs.erase(osi);
            // and remove from the master list
            vector<Spring*>::iterator si = std::remove(springs.begin(), springs.end(), s);
            springs.erase(si);
            delete s;
    	}

    	particles.erase( std::remove(particles.begin(),particles.end(), p ) );
    	// reset indices of each particle :(
    	for ( int i = 0 ; i < particles.size(); i++ ) {
    		particles[i]->index = i;
    	}
    }
    
    /**
     * Creates a new spring between two particles and adds it to the system.
     * @param p1
     * @param p2
     * @return the new spring
     */
    Spring* createSpring( Particle* p1, Particle* p2 ) {
        Spring* s = new Spring( p1, p2 ); 
        springs.push_back( s );         
        return s;
    }
    
    /**
     * Removes a spring between p1 and p2 if it exists, does nothing otherwise
     * @param p1
     * @param p2
     * @return true if the spring was found and removed
     */
    bool removeSpring( Particle* p1 = new Particle, Particle* p2 = new Particle ) {
    	Spring* found = NULL;
    	for ( Spring* s : springs ) {
    		if ( ( s->p1 == p1 && s->p2 == p2 ) || ( s->p1 == p2 && s->p2 == p1 ) ) {
    			found = s;
    			break;
    		}
    	}
    	if ( found != NULL ) {
            found->p1->springs.erase(std::remove(found->p1->springs.begin(), found->p1->springs.end(), found));
            found->p2->springs.erase(std::remove(found->p2->springs.begin(), found->p2->springs.end(), found));
            springs.erase(std::remove(springs.begin(),springs.end(), found));
			return true;
    	}
    	return false;
    }
    
    void init() {
        // do nothing
    }

    int height;
    int width;

    void display() {

        glPointSize( 10 );
        glBegin( GL_POINTS );
        for ( Particle* p : particles ) {
            double alpha = 0.5;
            if ( p->pinned ) {
                glColor4d( 1, 0, 0, alpha );
            } else {
                glColor4d( p->color.x, p->color.y, p->color.z, alpha );
            }
            glVertex2d( p->p.x, p->p.y );
        }
        glEnd();
        
        glColor4d(0,.5,.5,.5);
        glLineWidth(2.0f);
        glBegin( GL_LINES );
        for (Spring* s : springs) {
            glVertex2d( s->p1->p.x, s->p1->p.y );
            glVertex2d( s->p2->p.x, s->p2->p.y );
        }
        glEnd();
    }
    
    bool useGravity = true;
    float gravity = 9.8;
    float springStiffness = 100;
    float springDamping = 0;
    float viscousDamping = 0;
    /** should only go between 0 and 1 for bouncing off walls */
    float restitution = 0;
    int solverIterations = 100;
    bool useExplicitIntegration = true;
};