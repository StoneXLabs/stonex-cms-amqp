#pragma once
#include <vector>
#include <cms/Destination.h>

struct Destination
{
	std::string name;
	cms::Destination::DestinationType type;
	std::string selector{};
};

void publishSubscribe(int messageCount, Destination producer, std::vector<Destination> consumers, int groupCount);
void publishSubscribeText(int messageCount, Destination producer, std::vector<Destination> consumers, int groupCount);
//void pointToPoint(int messageCount = 2);
//void publishSubscribeGrouping(int messageCount = 1);
//void publishSubscribeTemporaryTopic(int messageCount = 1);