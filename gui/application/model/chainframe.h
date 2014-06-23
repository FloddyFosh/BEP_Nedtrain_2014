#ifndef CHAINFRAME_H
#define CHAINFRAME_H

#include <QPoint>

#include "frame.h"

class Chain;

/**
 * The Chainframe class is an extension of the frame class and
 * stores all the chains created by the chaining algorithm.
 */
class ChainFrame : public Frame {
private:
    Chain* chain;
    QList<QPoint*>* selectedProfile;
    QList<QPoint*>* usedProfile;
public:
    /**
     * Constructs a ChainFrame.
     */
    ChainFrame(Chain* c, QList<QPoint*>* prev);

    /**
     * Initializes the profiles, called at constructing.
     */
    void initialize();

    /**
     * Get the chain which this frame is representing.
     * @return the chain representing this frame
     */
    Chain* getChain();

    /**
     * Get the sub-profile which is selected in this frame for visualization.
     * @return A list of Qpoints representing the profile
     */
    QList<QPoint*>* getSelectedProfile();

    /**
     * Get the sub-profile which has been used so far for visualization.
     * @return A list of Qpoints representing the profile
     */
    QList<QPoint*>* getUsedProfile();

    /**
     * Set the sub-profile which has been used so far for visualization.
     * @param *p A list of points
     */
    void setUsedProfile(QList<QPoint *> *p);

    /**
     * Set the sub-profile which is selected in this frame for visualization.
     * @param *p A list of points
     */
    void setSelectedProfile(QList<QPoint*>* p);

    /**
     * Calculates the resulting profile by adding 2 profiles
     * @param *pl1 List of points, representing profile 1
     * @param *pl2 List of points, representing profile 2
     * @return result of the profiles added (piled up)
     */
    QList<QPoint*>* addProfiles(QList<QPoint*>* pl1, QList<QPoint*>* pl2);
};

#endif // CHAINFRAME_H
