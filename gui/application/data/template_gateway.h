#ifndef TEMPLATEGATEWAY_H
#define TEMPLATEGATEWAY_H

#include "data/i_template_gateway.h"

class ActivityTemplate;

/**
 * This class implements the ITemplateGateway and maintains activity templates
 * in the database. Saving, updating and removing of templates in the application
 * is done by this class.
 */
class TemplateGateway
{

public:
    static ITemplateGateway* instance;
	static void save(ActivityTemplate *activity);
    static void update(ActivityTemplate *activity);
    static void remove(ActivityTemplate *activity);
    static void remove(int id);
	static ActivityTemplate* get(int id);
	static QList<ActivityTemplate *> getAll();
};
#endif
