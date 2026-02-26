#pragma once

#include <cms/ConnectionMetaData.h>

namespace stonex::amqp
{
class ConnectionMetaData : public cms::ConnectionMetaData
{
public:
    ~ConnectionMetaData() = default;
    std::string getCMSVersion() const override;
    int getCMSMajorVersion() const override;
    int getCMSMinorVersion() const override;
    std::string getCMSProviderName() const override;
    std::string getProviderVersion() const override;
    int getProviderMajorVersion() const override;
    int getProviderMinorVersion() const override;
    int getProviderPatchVersion() const override;
    std::vector<std::string> getCMSXPropertyNames() const override;
};
}
