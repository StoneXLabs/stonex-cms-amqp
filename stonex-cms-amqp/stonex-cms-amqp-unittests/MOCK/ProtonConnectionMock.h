#pragma once
#include <gmock/gmock.h>
#include "proton/connection.hpp"
class ProtonConnectionMock : public proton::connection
{
public:
	MOCK_METHOD(proton::error_condition, error,());
};

