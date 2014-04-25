#include  <gtest/gtest.h>
#include "model/activitytemplate.h"

TEST(ActivityTemplateTest, ConstructorTest1) {
    ActivityTemplate *at = new ActivityTemplate("name", 1);
    ASSERT_EQ(-1, at->getID());
    ASSERT_EQ(1, at->getDuration());
    ASSERT_EQ("name", at->getName());
}

TEST(ActivityTemplateTest, ConstructorTest2) {
    ActivityTemplate *at = new ActivityTemplate(55, "name", 1);
    ASSERT_EQ(55, at->getID());
    ASSERT_EQ(1, at->getDuration());
    ASSERT_EQ("name", at->getName());
}

TEST(ActivityTemplateTest, GetSetDurationTest) {
    ActivityTemplate *at = new ActivityTemplate("name", 1);
    ASSERT_EQ(1, at->getDuration());
    at->setDuration(2);
    ASSERT_EQ(2, at->getDuration());
}

TEST(ActivityTemplateTest, GetSetNameTest) {
    ActivityTemplate *at = new ActivityTemplate("name", 1);
    ASSERT_EQ("name", at->getName());
    at->setName("name2");
    ASSERT_EQ("name2", at->getName());
}

TEST(ActivityTemplateTest, GetSetIdTest) {
    ActivityTemplate *at = new ActivityTemplate(25, "name", 1);
    ASSERT_EQ(25, at->getID());
    at->setID(26);
    ASSERT_EQ(26, at->getID());
}

TEST(ActivityTemplateTest, AddGetRequirementTest) {
    ActivityTemplate *at = new ActivityTemplate("name", 1);
    at->addRequirement("req", 2);
    QMap<QString, int> req = at->getRequirements();
    ASSERT_TRUE(req.contains("req"));
}

TEST(ActivityTemplateTest, AddRequirementsTest) {
    ActivityTemplate *at = new ActivityTemplate("name", 1);

    QMap<QString, int> req;
    req.insert("a", 1);
    req.insert("b", 1);
    at->setRequirements(req);

    ASSERT_TRUE(req.contains("a"));
    ASSERT_TRUE(req.contains("b"));
}

TEST(ActivityTemplateTest, RemoveRequirementTest) {
    ActivityTemplate *at = new ActivityTemplate("name", 1);
    at->addRequirement("req", 2);
    at->removeRequirement("req");
    QMap<QString, int> req = at->getRequirements();
    ASSERT_FALSE(req.contains("req"));
}

TEST(ActivityTemplateTest, IsCompatibleWithTest1) {
    ActivityTemplate *at = new ActivityTemplate("name", 1);
    at->addRequirement("a", 1);

    Instance *instance = new Instance();
    instance->addResource(1, "a");

    ASSERT_TRUE(at->isCompatibleWith(instance));
}

TEST(ActivityTemplateTest, IsCompatibleWithTest2) {
    ActivityTemplate *at = new ActivityTemplate("name", 1);
    at->addRequirement("a", 1);
    at->addRequirement("b", 1);

    Instance *instance = new Instance();
    instance->addResource(1, "a");
    instance->addResource(1, "b");

    ASSERT_TRUE(at->isCompatibleWith(instance));
}

TEST(ActivityTemplateTest, IsCompatibleWithTest3) {
    ActivityTemplate *at = new ActivityTemplate("name", 1);

    Instance *instance = new Instance();
    instance->addResource(1, "a");
    instance->addResource(1, "b");

    ASSERT_TRUE(at->isCompatibleWith(instance));
}

TEST(ActivityTemplateTest, IsCompatibleWithTest4) {
    ActivityTemplate *at = new ActivityTemplate("name", 1);
    at->addRequirement("a", 1);
    at->addRequirement("b", 1);

    Instance *instance = new Instance();

    ASSERT_FALSE(at->isCompatibleWith(instance));
}

TEST(ActivityTemplateTest, GetResourceNamesTest) {
    ActivityTemplate *at = new ActivityTemplate("name", 1);
    at->addRequirement("a", 1);
    at->addRequirement("b", 1);
    ASSERT_EQ("a, b", at->getResourceNames());
}
