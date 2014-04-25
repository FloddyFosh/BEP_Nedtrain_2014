#include "template_gateway.h"
#include "template_memory.h"

ITemplateGateway* TemplateGateway::instance = new TemplateInMemory();

void TemplateGateway::save(ActivityTemplate *activity) {
	instance->save(activity);
}

void TemplateGateway::update(ActivityTemplate *activity) {
    instance->update(activity);
}

void TemplateGateway::remove(ActivityTemplate *activity) {
    instance->remove(activity);
}

void TemplateGateway::remove(int id) {
	instance->remove(id);
};

ActivityTemplate* TemplateGateway::get(int id) {
    return instance->get(id);
};

QList<ActivityTemplate *> TemplateGateway::getAll() {
	return instance->getAll();
}
