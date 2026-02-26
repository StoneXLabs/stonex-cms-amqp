#pragma once

#include <string>

#include <cms/Queue.h>
#include <cms/TemporaryQueue.h>
#include <cms/Topic.h>
#include <cms/TemporaryTopic.h>

namespace internal
{
	struct Destination
	{
		bool valid{false};
		cms::Destination::DestinationType type = cms::Destination::QUEUE;
		std::string address{};
	};
}