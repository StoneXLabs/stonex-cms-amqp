#pragma once

#include "ProtonCppLibrary.h"
#include <gmock/gmock.h>

#include "proton/connection.hpp"

class ProtonCppLibraryMock : public cms::ProtonCppLibrary
{
public:
	MOCK_METHOD(void, on_connection_error, (proton::connection &conn));
};

