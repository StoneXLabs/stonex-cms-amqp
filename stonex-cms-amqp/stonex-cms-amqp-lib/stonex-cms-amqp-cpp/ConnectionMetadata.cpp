#include "ConnectionMetadata.h"


std::string stonex::amqp::ConnectionMetaData::getCMSVersion() const
{
    return "3.2";
}

int stonex::amqp::ConnectionMetaData::getCMSMajorVersion() const
{
    return 3;
}

int stonex::amqp::ConnectionMetaData::getCMSMinorVersion() const
{
    return 2;
}

std::string stonex::amqp::ConnectionMetaData::getCMSProviderName() const
{
    return "qpid-proton";
}

std::string stonex::amqp::ConnectionMetaData::getProviderVersion() const
{
    return {};
}

int stonex::amqp::ConnectionMetaData::getProviderMajorVersion() const
{
    return {};
}

int stonex::amqp::ConnectionMetaData::getProviderMinorVersion() const
{
    return {};
}

int stonex::amqp::ConnectionMetaData::getProviderPatchVersion() const
{
    return {};
}

std::vector<std::string> stonex::amqp::ConnectionMetaData::getCMSXPropertyNames() const
{
    return {};
}