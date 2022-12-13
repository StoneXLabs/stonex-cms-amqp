#pragma once

#include "cms/MessageTransformer.h"
#include <gmock/gmock.h>

class MessageTransformerMock : public cms::MessageTransformer
{
public:
    MOCK_METHOD(bool, consumerTransform, (cms::Session* session, cms::MessageConsumer* consumer, cms::Message* message, cms::Message** transformed), (override));
    MOCK_METHOD(bool, producerTransform, (cms::Session* session, cms::MessageProducer* producer, cms::Message* message, cms::Message** transformed), (override));

};

