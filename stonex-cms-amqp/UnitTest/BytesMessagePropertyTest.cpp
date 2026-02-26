#include <gtest/gtest.h>
#include <BytesMessage.h>

#include "Queue.h"
#include "Topic.h"
#include "TemporaryQueue.h"
#include "TemporaryTopic.h"

using stonex::amqp::BytesMessage;

TEST(BytesMessage, set_and_get_boolean_property) {
    BytesMessage msg;
    msg.setBooleanProperty("flag", true);
    EXPECT_TRUE(msg.getBooleanProperty("flag"));
}

TEST(BytesMessage, set_and_get_byte_property) {
    BytesMessage msg;
    msg.setByteProperty("byte", 42);
    EXPECT_EQ(msg.getByteProperty("byte"), 42);
}

TEST(BytesMessage, set_and_get_double_property) {
    BytesMessage msg;
    msg.setDoubleProperty("pi", 3.14);
    EXPECT_DOUBLE_EQ(msg.getDoubleProperty("pi"), 3.14);
}

TEST(BytesMessage, set_and_get_float_property) {
    BytesMessage msg;
    msg.setFloatProperty("f", 2.71f);
    EXPECT_FLOAT_EQ(msg.getFloatProperty("f"), 2.71f);
}

TEST(BytesMessage, set_and_get_int_property) {
    BytesMessage msg;
    msg.setIntProperty("i", 123);
    EXPECT_EQ(msg.getIntProperty("i"), 123);
}

TEST(BytesMessage, set_and_get_long_property) {
    BytesMessage msg;
    msg.setLongProperty("l", 123456789LL);
    EXPECT_EQ(msg.getLongProperty("l"), 123456789LL);
}

TEST(BytesMessage, set_and_get_short_property) {
    BytesMessage msg;
    msg.setShortProperty("s", 321);
    EXPECT_EQ(msg.getShortProperty("s"), 321);
}

TEST(BytesMessage, set_and_get_string_property) {
    BytesMessage msg;
    msg.setStringProperty("str", "test");
    EXPECT_EQ(msg.getStringProperty("str"), "test");
}

TEST(BytesMessage, property_exists) {
    BytesMessage msg;
    msg.setIntProperty("exists", 1);
    EXPECT_TRUE(msg.propertyExists("exists"));
    EXPECT_FALSE(msg.propertyExists("not_exists"));
}

TEST(BytesMessage, get_property_names) {
    BytesMessage msg;
    msg.setIntProperty("a", 1);
    msg.setStringProperty("b", "b");
    auto names = msg.getPropertyNames();
    EXPECT_NE(std::find(names.begin(), names.end(), "a"), names.end());
    EXPECT_NE(std::find(names.begin(), names.end(), "b"), names.end());
}


TEST(BytesMessage, clear_properties) {
    BytesMessage msg;
    msg.setIntProperty("x", 1);
    msg.clearProperties();
    EXPECT_FALSE(msg.propertyExists("x"));
}

TEST(BytesMessage, property_value_type_boolean) {
    BytesMessage msg;
    msg.setBooleanProperty("bool", true);
    EXPECT_EQ(msg.getPropertyValueType("bool"), cms::BytesMessage::BOOLEAN_TYPE);
}

TEST(BytesMessage, property_value_type_byte) {
    BytesMessage msg;
    msg.setByteProperty("byte", 42);
    EXPECT_EQ(msg.getPropertyValueType("byte"), cms::BytesMessage::BYTE_TYPE);
}

TEST(BytesMessage, property_value_type_short) {
    BytesMessage msg;
    msg.setShortProperty("short", 123);
    EXPECT_EQ(msg.getPropertyValueType("short"), cms::BytesMessage::SHORT_TYPE);
}

TEST(BytesMessage, property_value_type_int) {
    BytesMessage msg;
    msg.setIntProperty("int", 456);
    EXPECT_EQ(msg.getPropertyValueType("int"), cms::BytesMessage::INTEGER_TYPE);
}

TEST(BytesMessage, property_value_type_long) {
    BytesMessage msg;
    msg.setLongProperty("long", 789LL);
    EXPECT_EQ(msg.getPropertyValueType("long"), cms::BytesMessage::LONG_TYPE);
}

TEST(BytesMessage, property_value_type_float) {
    BytesMessage msg;
    msg.setFloatProperty("float", 3.14f);
    EXPECT_EQ(msg.getPropertyValueType("float"), cms::BytesMessage::FLOAT_TYPE);
}

TEST(BytesMessage, property_value_type_double) {
    BytesMessage msg;
    msg.setDoubleProperty("double", 2.718);
    EXPECT_EQ(msg.getPropertyValueType("double"), cms::BytesMessage::DOUBLE_TYPE);
}

TEST(BytesMessage, property_value_type_string) {
    BytesMessage msg;
    msg.setStringProperty("string", "test");
    EXPECT_EQ(msg.getPropertyValueType("string"), cms::BytesMessage::STRING_TYPE);
}

TEST(BytesMessage, DISABLED_property_value_type_unknown) {
    BytesMessage msg;
    EXPECT_EQ(msg.getPropertyValueType("not_set"), cms::BytesMessage::UNKNOWN_TYPE);
}

TEST(BytesMessageTest, CorrelationID) {
    BytesMessage msg;
    std::string id = "corr-id";
    msg.setCMSCorrelationID(id);
    EXPECT_EQ(msg.getCMSCorrelationID(), id);
}

TEST(BytesMessageTest, DeliveryMode) {
    BytesMessage msg;
    int mode = 1;
    msg.setCMSDeliveryMode(mode);
    EXPECT_EQ(msg.getCMSDeliveryMode(), mode);
}

TEST(BytesMessageTest, Expiration) {
    BytesMessage msg;
    long long exp = 123456789LL;
    msg.setCMSExpiration(exp);
    EXPECT_EQ(msg.getCMSExpiration(), exp);
}

TEST(BytesMessageTest, BytesMessageID) {
    BytesMessage msg;
    std::string id = "msg-id";
    msg.setCMSMessageID(id);
    EXPECT_EQ(msg.getCMSMessageID(), id);
}

TEST(BytesMessageTest, Priority) {
    BytesMessage msg;
    int priority = 5;
    msg.setCMSPriority(priority);
    EXPECT_EQ(msg.getCMSPriority(), priority);
}

TEST(BytesMessageTest, Redelivered) {
    BytesMessage msg;
    msg.setCMSRedelivered(true);
    EXPECT_TRUE(msg.getCMSRedelivered());
    msg.setCMSRedelivered(false);
    EXPECT_FALSE(msg.getCMSRedelivered());
}

TEST(BytesMessageTest, Timestamp) {
    BytesMessage msg;
    long long ts = 987654321LL;
    msg.setCMSTimestamp(ts);
    EXPECT_EQ(msg.getCMSTimestamp(), ts);
}

TEST(BytesMessageTest, Type) {
    BytesMessage msg;
    std::string type = "type";
    msg.setCMSType(type);
    EXPECT_EQ(msg.getCMSType(), type);
}

TEST(BytesMessageTest, Destination_Queue) {
    BytesMessage msg;
    cms::Destination* dest = new stonex::amqp::Queue("queue");
    msg.setCMSDestination(dest);
    EXPECT_TRUE(msg.getCMSDestination()->equals(*dest));
}

TEST(BytesMessageTest, Destination_Topic) {
    BytesMessage msg;
    cms::Destination* dest = new stonex::amqp::Topic("queue");
    msg.setCMSDestination(dest);
    EXPECT_TRUE(msg.getCMSDestination()->equals(*dest));
}

TEST(BytesMessageTest, Destination_TempQueue) {
    BytesMessage msg;
    cms::Destination* dest = new stonex::amqp::TemporaryQueue("queue");
    msg.setCMSDestination(dest);
    EXPECT_TRUE(msg.getCMSDestination()->equals(*dest));
}

TEST(BytesMessageTest, Destination_TempTopic) {
    BytesMessage msg;
    cms::Destination* dest = new stonex::amqp::TemporaryTopic("queue");
    msg.setCMSDestination(dest);
    EXPECT_TRUE(msg.getCMSDestination()->equals(*dest));
}

TEST(BytesMessageTest, ReplyTo_Queue) {
    BytesMessage msg;
    cms::Destination* dest = new stonex::amqp::Queue("queue");
    msg.setCMSReplyTo(dest);
    EXPECT_TRUE(msg.getCMSReplyTo()->equals(*dest));
}

TEST(BytesMessageTest, ReplyTo_Toic) {
    BytesMessage msg;
    cms::Destination* dest = new stonex::amqp::Topic("queue");
    msg.setCMSReplyTo(dest);
    EXPECT_TRUE(msg.getCMSReplyTo()->equals(*dest));
}

TEST(BytesMessageTest, ReplyTo_TempQueue) {
    BytesMessage msg;
    cms::Destination* dest = new stonex::amqp::TemporaryQueue("queue");
    msg.setCMSReplyTo(dest);
    EXPECT_TRUE(msg.getCMSReplyTo()->equals(*dest));
}

TEST(BytesMessageTest, ReplyTo_TempTopic) {
    BytesMessage msg;
    cms::Destination* dest = new stonex::amqp::TemporaryTopic("queue");
    msg.setCMSReplyTo(dest);
    EXPECT_TRUE(msg.getCMSReplyTo()->equals(*dest));
}

TEST(BytesMessageTest, ReadWrite) {
    BytesMessage msg;
    msg.writeBoolean(true);
    msg.writeByte(0xF3);
    msg.writeChar('G');
    msg.writeFloat(-1234.56);
    msg.writeDouble(-1234.56);
    msg.writeShort(-1234);
    msg.writeUnsignedShort(1234);
    msg.writeInt(1234);
    msg.writeLong(12345678);
    msg.writeString("Test");

    EXPECT_EQ(msg.readBoolean(),true);
    EXPECT_EQ(msg.readByte(),0xF3);
    EXPECT_EQ(msg.readChar(),'G');
    EXPECT_EQ(msg.readFloat(),-1234.56);
    EXPECT_EQ(msg.readDouble(),-1234.56);
    EXPECT_EQ(msg.readShort(),-1234);
    EXPECT_EQ(msg.readUnsignedShort(),1234);
    EXPECT_EQ(msg.readInt(),1234);
    EXPECT_EQ(msg.readLong(),12345678);
    EXPECT_EQ(msg.readString(),"Test");

    EXPECT_THROW(msg.readBoolean(), cms::CMSException);
}