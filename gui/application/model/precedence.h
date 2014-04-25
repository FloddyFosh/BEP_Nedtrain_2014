#ifndef PRECEDENCE_H_
#define PRECEDENCE_H_

#include <QString>

class Activity;

/**
 * Precedence constraint between two activities. The precedence relation is
 * \f$a_{i_{1}j_{1}} \prec a_{i_{2}j_{2}}\f$.
 * Precedence constraints can be <i>soft</i> or <i>hard</i>. <i>Soft</i> constraints are added by the solver during the solving process, whereas <i>hard</i> constraints are added as part of the problem specification.
 */
class Precedence {
private:
    Activity *_a1; ///< preceeding activity
    Activity *_a2; ///< preceeded activity
    bool hard; ///< whether this precedence is <i>hard</i>, that is, a constraint from the problem (in contrast to <i>soft</i> precedences, which are added by the solver during the solving process).
    int frameNumber;
public:
    /**
     * Constructs a hard precedence from two activities.
     * @param a1 preceeding activity
     * @param a2 preceeded activity
     */
    Precedence(Activity *a1, Activity *a2);
    /**
     * Constructs a precedence from two activities. Whether this is a hard constraint can be specified in the third argument.
     * @param a1 preceeding activity
     * @param a2 preceeded activity
     * @param hard whether this is a hard constraint
     */
    Precedence(Activity *a1, Activity *a2, bool hard);

    /**
     * @return Preceeding activity
     */
    Activity* a1();
    /**
     * @return Preceeded activity
     */
    Activity* a2();

    /**
     * @return whether the given activity equals the preceeded activity in this constraint.
     */
    bool isIncoming(Activity *);
    /**
     * @return whether the given activity equals the preceeding activity in this constraint.
     */
    bool isOutgoing(Activity *);
    /**
     * equality operator for Precedence objects.
     */
    bool operator==(const Precedence &other) const;

    /**
     * Returns true if the precedence is part of the instance
     * and not added by the algorithm.
     */
    bool isHard();

    /**
     * @return The frame number at which this constraint was posted. Applies to <i>soft</i> precedences only.
     */
    int getFrameNr();
    /**
     * Sets the frame number. Applies to soft precedences.
     */
    void setFrameNr(int frameNr);

    /**
     * @return a string representation of this constraint, parseable by the solver.
     */
    QString toString();
};


#endif /* PRECEDENCE_H_ */
