/**
 * Velocity filter to use with a conjugate gradients solve
 * @author kry
 */
class Filter {
public:
    /**
     * removes disallowed parts of v by projection
     * @param v
     */
    virtual void filter(VectorXf& v) = 0;
};