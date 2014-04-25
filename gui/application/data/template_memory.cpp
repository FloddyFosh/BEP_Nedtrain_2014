#include "template_memory.h"

void TemplateInMemory::save(ActivityTemplate *activity) {
	templates[activity->getID()] = activity;
}

/**
 * Precondition: id hasn't changed
 */
void TemplateInMemory::update(ActivityTemplate *activity) {
    templates[activity->getID()] = activity;
}

void TemplateInMemory::remove(ActivityTemplate *activity) {
    templates.remove(activity->getID());
};

void TemplateInMemory::remove(int id) {
    templates.remove(id);
};

ActivityTemplate* TemplateInMemory::get(int id) {
	return templates[id];
};

QList<ActivityTemplate *> TemplateInMemory::getAll() {
    QList<ActivityTemplate *> list;
    QMapIterator<int, ActivityTemplate *> i(templates);
    while (i.hasNext()) {
        i.next();
        list.append(i.value());
    }
	return list;
}
