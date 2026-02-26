#include <gtest/gtest.h>
#include <TextMessage.h>

#include "Queue.h"
#include "Topic.h"
#include "TemporaryQueue.h"
#include "TemporaryTopic.h"

using stonex::amqp::TextMessage;

TEST(TextMessage, set_and_get_boolean_property) {
    TextMessage msg;
    msg.setBooleanProperty("flag", true);
    EXPECT_TRUE(msg.getBooleanProperty("flag"));
}

TEST(TextMessage, set_and_get_byte_property) {
    TextMessage msg;
    msg.setByteProperty("byte", 42);
    EXPECT_EQ(msg.getByteProperty("byte"), 42);
}

TEST(TextMessage, set_and_get_double_property) {
    TextMessage msg;
    msg.setDoubleProperty("pi", 3.14);
    EXPECT_DOUBLE_EQ(msg.getDoubleProperty("pi"), 3.14);
}

TEST(TextMessage, set_and_get_float_property) {
    TextMessage msg;
    msg.setFloatProperty("f", 2.71f);
    EXPECT_FLOAT_EQ(msg.getFloatProperty("f"), 2.71f);
}

TEST(TextMessage, set_and_get_int_property) {
    TextMessage msg;
    msg.setIntProperty("i", 123);
    EXPECT_EQ(msg.getIntProperty("i"), 123);
}

TEST(TextMessage, set_and_get_long_property) {
    TextMessage msg;
    msg.setLongProperty("l", 123456789LL);
    EXPECT_EQ(msg.getLongProperty("l"), 123456789LL);
}

TEST(TextMessage, set_and_get_short_property) {
    TextMessage msg;
    msg.setShortProperty("s", 321);
    EXPECT_EQ(msg.getShortProperty("s"), 321);
}

TEST(TextMessage, set_and_get_string_property) {
    TextMessage msg;
    msg.setStringProperty("str", "test");
    EXPECT_EQ(msg.getStringProperty("str"), "test");
}

TEST(TextMessage, property_exists) {
    TextMessage msg;
    msg.setIntProperty("exists", 1);
    EXPECT_TRUE(msg.propertyExists("exists"));
    EXPECT_FALSE(msg.propertyExists("not_exists"));
}

TEST(TextMessage, get_property_names) {
    TextMessage msg;
    msg.setIntProperty("a", 1);
    msg.setStringProperty("b", "b");
    auto names = msg.getPropertyNames();
    EXPECT_NE(std::find(names.begin(), names.end(), "a"), names.end());
    EXPECT_NE(std::find(names.begin(), names.end(), "b"), names.end());
}


TEST(TextMessage, clear_properties) {
    TextMessage msg;
    msg.setIntProperty("x", 1);
    msg.clearProperties();
    EXPECT_FALSE(msg.propertyExists("x"));
}

TEST(TextMessage, property_value_type_boolean) {
    TextMessage msg;
    msg.setBooleanProperty("bool", true);
    EXPECT_EQ(msg.getPropertyValueType("bool"), cms::TextMessage::BOOLEAN_TYPE);
}

TEST(TextMessage, property_value_type_byte) {
    TextMessage msg;
    msg.setByteProperty("byte", 42);
    EXPECT_EQ(msg.getPropertyValueType("byte"), cms::TextMessage::BYTE_TYPE);
}

TEST(TextMessage, property_value_type_short) {
    TextMessage msg;
    msg.setShortProperty("short", 123);
    EXPECT_EQ(msg.getPropertyValueType("short"), cms::TextMessage::SHORT_TYPE);
}

TEST(TextMessage, property_value_type_int) {
    TextMessage msg;
    msg.setIntProperty("int", 456);
    EXPECT_EQ(msg.getPropertyValueType("int"), cms::TextMessage::INTEGER_TYPE);
}

TEST(TextMessage, property_value_type_long) {
    TextMessage msg;
    msg.setLongProperty("long", 789LL);
    EXPECT_EQ(msg.getPropertyValueType("long"), cms::TextMessage::LONG_TYPE);
}

TEST(TextMessage, property_value_type_float) {
    TextMessage msg;
    msg.setFloatProperty("float", 3.14f);
    EXPECT_EQ(msg.getPropertyValueType("float"), cms::TextMessage::FLOAT_TYPE);
}

TEST(TextMessage, property_value_type_double) {
    TextMessage msg;
    msg.setDoubleProperty("double", 2.718);
    EXPECT_EQ(msg.getPropertyValueType("double"), cms::TextMessage::DOUBLE_TYPE);
}

TEST(TextMessage, property_value_type_string) {
    TextMessage msg;
    msg.setStringProperty("string", "test");
    EXPECT_EQ(msg.getPropertyValueType("string"), cms::TextMessage::STRING_TYPE);
}

TEST(TextMessage, DISABLED_property_value_type_unknown) {
    TextMessage msg;
    EXPECT_EQ(msg.getPropertyValueType("not_set"), cms::TextMessage::UNKNOWN_TYPE);
}

TEST(TextMessageTest, CorrelationID) {
    TextMessage msg;
    std::string id = "corr-id";
    msg.setCMSCorrelationID(id);
    EXPECT_EQ(msg.getCMSCorrelationID(), id);
}

TEST(TextMessageTest, DeliveryMode) {
    TextMessage msg;
    int mode = 1;
    msg.setCMSDeliveryMode(mode);
    EXPECT_EQ(msg.getCMSDeliveryMode(), mode);
}

TEST(TextMessageTest, Expiration) {
    TextMessage msg;
    long long exp = 123456789LL;
    msg.setCMSExpiration(exp);
    EXPECT_EQ(msg.getCMSExpiration(), exp);
}

TEST(TextMessageTest, TextMessageID) {
    TextMessage msg;
    std::string id = "msg-id";
    msg.setCMSMessageID(id);
    EXPECT_EQ(msg.getCMSMessageID(), id);
}

TEST(TextMessageTest, Priority) {
    TextMessage msg;
    int priority = 5;
    msg.setCMSPriority(priority);
    EXPECT_EQ(msg.getCMSPriority(), priority);
}

TEST(TextMessageTest, Redelivered) {
    TextMessage msg;
    msg.setCMSRedelivered(true);
    EXPECT_TRUE(msg.getCMSRedelivered());
    msg.setCMSRedelivered(false);
    EXPECT_FALSE(msg.getCMSRedelivered());
}

TEST(TextMessageTest, Timestamp) {
    TextMessage msg;
    long long ts = 987654321LL;
    msg.setCMSTimestamp(ts);
    EXPECT_EQ(msg.getCMSTimestamp(), ts);
}

TEST(TextMessageTest, Type) {
    TextMessage msg;
    std::string type = "type";
    msg.setCMSType(type);
    EXPECT_EQ(msg.getCMSType(), type);
}

TEST(TextMessageTest, Destination_Queue) {
    TextMessage msg;
    cms::Destination* dest = new stonex::amqp::Queue("queue");
    msg.setCMSDestination(dest);
    EXPECT_TRUE(msg.getCMSDestination()->equals(*dest));
}

TEST(TextMessageTest, Destination_Topic) {
    TextMessage msg;
    cms::Destination* dest = new stonex::amqp::Topic("queue");
    msg.setCMSDestination(dest);
    EXPECT_TRUE(msg.getCMSDestination()->equals(*dest));
}

TEST(TextMessageTest, Destination_TempQueue) {
    TextMessage msg;
    cms::Destination* dest = new stonex::amqp::TemporaryQueue("queue");
    msg.setCMSDestination(dest);
    EXPECT_TRUE(msg.getCMSDestination()->equals(*dest));
}

TEST(TextMessageTest, Destination_TempTopic) {
    TextMessage msg;
    cms::Destination* dest = new stonex::amqp::TemporaryTopic("queue");
    msg.setCMSDestination(dest);
    EXPECT_TRUE(msg.getCMSDestination()->equals(*dest));
}

TEST(TextMessageTest, ReplyTo_Queue) {
    TextMessage msg;
    cms::Destination* dest = new stonex::amqp::Queue("queue");
    msg.setCMSReplyTo(dest);
    EXPECT_TRUE(msg.getCMSReplyTo()->equals(*dest));
}

TEST(TextMessageTest, ReplyTo_Toic) {
    TextMessage msg;
    cms::Destination* dest = new stonex::amqp::Topic("queue");
    msg.setCMSReplyTo(dest);
    EXPECT_TRUE(msg.getCMSReplyTo()->equals(*dest));
}

TEST(TextMessageTest, ReplyTo_TempQueue) {
    TextMessage msg;
    cms::Destination* dest = new stonex::amqp::TemporaryQueue("queue");
    msg.setCMSReplyTo(dest);
    EXPECT_TRUE(msg.getCMSReplyTo()->equals(*dest));
}

TEST(TextMessageTest, ReplyTo_TempTopic) {
    TextMessage msg;
    cms::Destination* dest = new stonex::amqp::TemporaryTopic("queue");
    msg.setCMSReplyTo(dest);
    EXPECT_TRUE(msg.getCMSReplyTo()->equals(*dest));
}

TEST(TextMessageTest, message_content_text) {
    TextMessage msg;
    msg.setText("Hello, world!");
    EXPECT_EQ(msg.getText(), "Hello, world!");
}

TEST(TextMessage_UT, message_content_text_constructor) {
    TextMessage msg("Hello, world!");
    EXPECT_EQ(msg.getText(), "Hello, world!");
}