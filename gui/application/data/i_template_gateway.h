#ifndef I_TEMPLATEGATEWAY_H
#define I_TEMPLATEGATEWAY_H

#include "model/activitytemplate.h"

/**
 * This is an interface for a template gateway. It is inherited by classes
 * that provide fundamental save, update and store functionality for
 * templates such as storing in the database and in memory (for testing).
 */
class ITemplateGateway
{
public:
    /**
     * Destructor
     */
    virtual ~ITemplateGateway() {};

    /**
     * Saves an activity template.
     * @param at template to be saved
     */
	virtual void save(ActivityTemplate *at) = 0;

	/**
	 * Updates an activity template.
	 * @param at template to be updated
	 */
    virtual void update(ActivityTemplate *at) = 0;

    /**
     * Removes an activity template.
     * @param at template to be removed
     */
    virtual void remove(ActivityTemplate *at) = 0;

    /**
     * Removes the activity template with specified id
     * @param id id of the template to be removed
     */
    virtual void remove(int id) = 0;

    /**
     * Get an activity template with specified id
     * @param id the id of the template to be returned
     * @return template with specified id
     */
	virtual ActivityTemplate* get(int id) = 0;

	/**
	 * Get a list containing all activity templates.
	 * @return list of templates
	 */
	virtual QList<ActivityTemplate *> getAll() = 0;
};
#endif
