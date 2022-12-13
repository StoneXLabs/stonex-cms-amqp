#pragma once

#include "cms/ExceptionListener.h"
#include <gmock/gmock.h>

class ExceptionListenerMock : public cms::ExceptionListener
{
public:
    MOCK_METHOD(void, onException, (const cms::CMSException& ex), (override));

};

