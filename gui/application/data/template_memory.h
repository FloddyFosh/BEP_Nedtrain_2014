#ifndef TEMPLATEHOLDER_H
#define TEMPLATEHOLDER_H

#include "data/i_template_gateway.h"

/**
 * This class implements the ITemplateGateway and maintains all templates
 * in memory. It is used by the testsuite to store templates easily without
 * a real database.
 */
class TemplateInMemory : public ITemplateGateway {

private:
	QMap<int, ActivityTemplate *> templates;
public:
	void save(ActivityTemplate *activity);
    void update(ActivityTemplate *activity);
    void remove(ActivityTemplate *activity);
    void remove(int id);
    ActivityTemplate* get(int id);
	QList<ActivityTemplate *> getAll();
};

#endif // TEMPLATEHOLDER_H
