#ifndef EST_GENERATOR_H
#define EST_GENERATOR_H

#include <QList>

class InstanceController;
class Precedence;

/**
 * EST generator class.
 * After the solver has been run on an instance, a list of frames is remembered for the replay functionality. This generator class can "update" an instance to a particular frame, managing constraints and activity starting times.
 */
class EST_Generator
{

    InstanceController * controller;///< the controller of the instance which is managed

public:
    /**
     * Constructor.
     * @param controller the controller of the instance
     */
    EST_Generator (InstanceController * controller);

    /**
     * Updates the instance to a particular frame.
     * @param frameNr the frame number to go to
     */
    void gotoFrame(int frameNr);

    /**
     * Returns a list of precedences which were added in the specified frame number.
     * @param frameNr frame number
     */
    QList<Precedence *> getAdded(int frameNr);
};
#endif // EST_GENERATOR_H
