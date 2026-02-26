#include <gtest/gtest.h>
#include <Message.h>

#include "Queue.h"
#include "Topic.h"
#include "TemporaryQueue.h"
#include "TemporaryTopic.h"

using stonex::amqp::Message;

TEST(MessageTest, set_and_get_boolean_property) {
    Message msg;
    msg.setBooleanProperty("flag", true);
    EXPECT_TRUE(msg.getBooleanProperty("flag"));
}

TEST(MessageTest, set_and_get_byte_property) {
    Message msg;
    msg.setByteProperty("byte", 42);
    EXPECT_EQ(msg.getByteProperty("byte"), 42);
}

TEST(MessageTest, set_and_get_double_property) {
    Message msg;
    msg.setDoubleProperty("pi", 3.14);
    EXPECT_DOUBLE_EQ(msg.getDoubleProperty("pi"), 3.14);
}

TEST(MessageTest, set_and_get_float_property) {
    Message msg;
    msg.setFloatProperty("f", 2.71f);
    EXPECT_FLOAT_EQ(msg.getFloatProperty("f"), 2.71f);
}

TEST(MessageTest, set_and_get_int_property) {
    Message msg;
    msg.setIntProperty("i", 123);
    EXPECT_EQ(msg.getIntProperty("i"), 123);
}

TEST(MessageTest, set_and_get_long_property) {
    Message msg;
    msg.setLongProperty("l", 123456789LL);
    EXPECT_EQ(msg.getLongProperty("l"), 123456789LL);
}

TEST(MessageTest, set_and_get_short_property) {
    Message msg;
    msg.setShortProperty("s", 321);
    EXPECT_EQ(msg.getShortProperty("s"), 321);
}

TEST(MessageTest, set_and_get_string_property) {
    Message msg;
    msg.setStringProperty("str", "test");
    EXPECT_EQ(msg.getStringProperty("str"), "test");
}

TEST(MessageTest, property_exists) {
    Message msg;
    msg.setIntProperty("exists", 1);
    EXPECT_TRUE(msg.propertyExists("exists"));
    EXPECT_FALSE(msg.propertyExists("not_exists"));
}

TEST(MessageTest, get_property_names) {
    Message msg;
    msg.setIntProperty("a", 1);
    msg.setStringProperty("b", "b");
    auto names = msg.getPropertyNames();
    EXPECT_NE(std::find(names.begin(), names.end(), "a"), names.end());
    EXPECT_NE(std::find(names.begin(), names.end(), "b"), names.end());
}


TEST(MessageTest, clear_properties) {
    Message msg;
    msg.setIntProperty("x", 1);
    msg.clearProperties();
    EXPECT_FALSE(msg.propertyExists("x"));
}

TEST(MessageTest, property_value_type_boolean) {
    Message msg;
    msg.setBooleanProperty("bool", true);
    EXPECT_EQ(msg.getPropertyValueType("bool"), cms::Message::BOOLEAN_TYPE);
}

TEST(MessageTest, property_value_type_byte) {
    Message msg;
    msg.setByteProperty("byte", 42);
    EXPECT_EQ(msg.getPropertyValueType("byte"), cms::Message::BYTE_TYPE);
}

TEST(MessageTest, property_value_type_short) {
    Message msg;
    msg.setShortProperty("short", 123);
    EXPECT_EQ(msg.getPropertyValueType("short"), cms::Message::SHORT_TYPE);
}

TEST(MessageTest, property_value_type_int) {
    Message msg;
    msg.setIntProperty("int", 456);
    EXPECT_EQ(msg.getPropertyValueType("int"), cms::Message::INTEGER_TYPE);
}

TEST(MessageTest, property_value_type_long) {
    Message msg;
    msg.setLongProperty("long", 789LL);
    EXPECT_EQ(msg.getPropertyValueType("long"), cms::Message::LONG_TYPE);
}

TEST(MessageTest, property_value_type_float) {
    Message msg;
    msg.setFloatProperty("float", 3.14f);
    EXPECT_EQ(msg.getPropertyValueType("float"), cms::Message::FLOAT_TYPE);
}

TEST(MessageTest, property_value_type_double) {
    Message msg;
    msg.setDoubleProperty("double", 2.718);
    EXPECT_EQ(msg.getPropertyValueType("double"), cms::Message::DOUBLE_TYPE);
}

TEST(MessageTest, property_value_type_string) {
    Message msg;
    msg.setStringProperty("string", "test");
    EXPECT_EQ(msg.getPropertyValueType("string"), cms::Message::STRING_TYPE);
}

TEST(MessageTest, DISABLED_property_value_type_unknown) {
    Message msg;
    EXPECT_EQ(msg.getPropertyValueType("not_set"), cms::Message::UNKNOWN_TYPE);
}

TEST(MessageTest, CorrelationID) {
    Message msg;
    std::string id = "corr-id";
    msg.setCMSCorrelationID(id);
    EXPECT_EQ(msg.getCMSCorrelationID(), id);
}

TEST(MessageTest, DeliveryMode) {
    Message msg;
    int mode = 1;
    msg.setCMSDeliveryMode(mode);
    EXPECT_EQ(msg.getCMSDeliveryMode(), mode);
}

TEST(MessageTest, Expiration) {
    Message msg;
    long long exp = 123456789LL;
    msg.setCMSExpiration(exp);
    EXPECT_EQ(msg.getCMSExpiration(), exp);
}

TEST(MessageTest, MessageID) {
    Message msg;
    std::string id = "msg-id";
    msg.setCMSMessageID(id);
    EXPECT_EQ(msg.getCMSMessageID(), id);
}

TEST(MessageTest, Priority) {
    Message msg;
    int priority = 5;
    msg.setCMSPriority(priority);
    EXPECT_EQ(msg.getCMSPriority(), priority);
}

TEST(MessageTest, Redelivered) {
    Message msg;
    msg.setCMSRedelivered(true);
    EXPECT_TRUE(msg.getCMSRedelivered());
    msg.setCMSRedelivered(false);
    EXPECT_FALSE(msg.getCMSRedelivered());
}

TEST(MessageTest, Timestamp) {
    Message msg;
    long long ts = 987654321LL;
    msg.setCMSTimestamp(ts);
    EXPECT_EQ(msg.getCMSTimestamp(), ts);
}

TEST(MessageTest, Type) {
    Message msg;
    std::string type = "type";
    msg.setCMSType(type);
    EXPECT_EQ(msg.getCMSType(), type);
}

TEST(MessageTest, Destination_Queue) {
    Message msg;
    cms::Destination* dest = new stonex::amqp::Queue("queue");
    msg.setCMSDestination(dest);
    EXPECT_TRUE(msg.getCMSDestination()->equals(*dest));
}

TEST(MessageTest, Destination_Topic) {
    Message msg;
    cms::Destination* dest = new stonex::amqp::Topic("queue");
    msg.setCMSDestination(dest);
    EXPECT_TRUE(msg.getCMSDestination()->equals(*dest));
}

TEST(MessageTest, Destination_TempQueue) {
    Message msg;
    cms::Destination* dest = new stonex::amqp::TemporaryQueue("queue");
    msg.setCMSDestination(dest);
    EXPECT_TRUE(msg.getCMSDestination()->equals(*dest));
}

TEST(MessageTest, Destination_TempTopic) {
    Message msg;
    cms::Destination* dest = new stonex::amqp::TemporaryTopic("queue");
    msg.setCMSDestination(dest);
    EXPECT_TRUE(msg.getCMSDestination()->equals(*dest));
}

TEST(MessageTest, ReplyTo_Queue) {
    Message msg;
    cms::Destination* dest = new stonex::amqp::Queue("queue");
    msg.setCMSReplyTo(dest);
    EXPECT_TRUE(msg.getCMSReplyTo()->equals(*dest));
}

TEST(MessageTest, ReplyTo_Toic) {
    Message msg;
    cms::Destination* dest = new stonex::amqp::Topic("queue");
    msg.setCMSReplyTo(dest);
    EXPECT_TRUE(msg.getCMSReplyTo()->equals(*dest));
}

TEST(MessageTest, ReplyTo_TempQueue) {
    Message msg;
    cms::Destination* dest = new stonex::amqp::TemporaryQueue("queue");
    msg.setCMSReplyTo(dest);
    EXPECT_TRUE(msg.getCMSReplyTo()->equals(*dest));
}

TEST(MessageTest, ReplyTo_TempTopic) {
    Message msg;
    cms::Destination* dest = new stonex::amqp::TemporaryTopic("queue");
    msg.setCMSReplyTo(dest);
    EXPECT_TRUE(msg.getCMSReplyTo()->equals(*dest));
}

TEST(MessageTest, DefaultGetters) {
    Message msg;
    EXPECT_EQ(msg.getCMSCorrelationID(), "");
    EXPECT_EQ(msg.getCMSDeliveryMode(), 0);
    EXPECT_EQ(msg.getCMSDestination(), nullptr);
    EXPECT_EQ(msg.getCMSExpiration(), 0);
//    EXPECT_EQ(msg.getCMSMessageID(), "");
    EXPECT_EQ(msg.getCMSPriority(), 0);
    EXPECT_FALSE(msg.getCMSRedelivered());
    EXPECT_EQ(msg.getCMSReplyTo(), nullptr);
    EXPECT_EQ(msg.getCMSTimestamp(), 0);
    EXPECT_EQ(msg.getCMSType(), "");
}