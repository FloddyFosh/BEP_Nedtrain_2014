#include  <gtest/gtest.h>
#include "model/activitytemplate.h"
#include "data/template_gateway.h"
#include "data/template_memory.h"
#include <QString>

TEST(TemplateGatewayTest, saveAndGetAndRemove) {
    TemplateGateway::instance = new TemplateInMemory();
    ActivityTemplate *activity = new ActivityTemplate(1, "Test", 2);

	TemplateGateway::save(activity);
	ASSERT_EQ(activity->getName(), TemplateGateway::get(activity->getID())->getName());

    TemplateGateway::remove(activity->getID());
    ASSERT_EQ(NULL, TemplateGateway::get(activity->getID()));
}
