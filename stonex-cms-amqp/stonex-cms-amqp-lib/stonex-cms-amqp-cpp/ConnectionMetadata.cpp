#include "ConnectionMetadata.h"
#include <proton/version.h>


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
    return std::to_string(PN_VERSION_MAJOR)+"."+std::to_string(PN_VERSION_MINOR)+"."+std::to_string(PN_VERSION_POINT);
}

int stonex::amqp::ConnectionMetaData::getProviderMajorVersion() const
{
    return PN_VERSION_MAJOR;
}

int stonex::amqp::ConnectionMetaData::getProviderMinorVersion() const
{
    return PN_VERSION_MINOR;
}

int stonex::amqp::ConnectionMetaData::getProviderPatchVersion() const
{
    return PN_VERSION_POINT;
}

std::vector<std::string> stonex::amqp::ConnectionMetaData::getCMSXPropertyNames() const
{
    return {};
}