

#include "samples.h"
#include <log4cxx/basicconfigurator.h>


int main(int argc, char* argv[])
{

	log4cxx::BasicConfigurator::configure();
	//Publish to MULTICAST 10000 messages
	//2 consumers TESTADDRESS::TESTADDRESS/STONEX1 and TESTADDRESS::TESTADDRESS/STONEX2
	//expected each consumer receives 10000 messages

//	publishSubscribe(10000, Destination{ .name = "TESTADDRESS",.type = cms::Destination::TOPIC }, { Destination{.name = "TESTADDRESS::TESTADDRESS/STONEX1",.type = cms::Destination::TOPIC }, Destination{.name = "TESTADDRESS::TESTADDRESS/STONEX2",.type = cms::Destination::TOPIC } }, 0);

//	publishSubscribeText(10000, Destination{ .name = "TESTADDRESS",.type = cms::Destination::TOPIC }, { Destination{.name = "TESTADDRESS::TESTADDRESS/STONEX1",.type = cms::Destination::TOPIC }, Destination{.name = "TESTADDRESS::TESTADDRESS/STONEX2",.type = cms::Destination::TOPIC } }, 0);

	//Publish to MULTICAST 10000 messages
	//3 consumers TESTADDRESS::TESTADDRESS/STONEX1 and TESTADDRESS::TESTADDRESS/STONEX2 (2 consumers)
	//expected messages
	// consumer 1 10000
	// consumer 2 5000
	// consumer 3 5000

//	publishSubscribe(10000, Destination{ .name = "TESTADDRESS",.type = cms::Destination::TOPIC }, { Destination{.name = "TESTADDRESS::TESTADDRESS/STONEX1",.type = cms::Destination::TOPIC }, Destination{.name = "TESTADDRESS::TESTADDRESS/STONEX2",.type = cms::Destination::TOPIC }, Destination{.name = "TESTADDRESS::TESTADDRESS/STONEX2",.type = cms::Destination::TOPIC } }, 0);

//	publishSubscribeText(10000, Destination{ .name = "TESTADDRESS",.type = cms::Destination::TOPIC }, { Destination{.name = "TESTADDRESS::TESTADDRESS/STONEX1",.type = cms::Destination::TOPIC }, Destination{.name = "TESTADDRESS::TESTADDRESS/STONEX2",.type = cms::Destination::TOPIC }, Destination{.name = "TESTADDRESS::TESTADDRESS/STONEX2",.type = cms::Destination::TOPIC } }, 0);
	
	//Publish to MULTICAST 10000 messages
	//2 consumers TESTADDRESS::TESTADDRESS/STONEX1 and TESTADDRESS::TESTADDRESS/STONEX2
	//expected each consumer receives 10000 messages

//	publishSubscribe(10000, Destination{ .name = "TESTADDRESS",.type = cms::Destination::TOPIC }, { Destination{.name = "TESTADDRESS::TESTADDRESS/STONEX3",.type = cms::Destination::TOPIC }, Destination{.name = "TESTADDRESS::TESTADDRESS/STONEX3",.type = cms::Destination::TOPIC } }, 5);

	publishSubscribeText(10000, Destination{ .name = "TESTADDRESS",.type = cms::Destination::TOPIC }, { Destination{.name = "TESTADDRESS::TESTADDRESS/STONEX3",.type = cms::Destination::TOPIC }, Destination{.name = "TESTADDRESS::TESTADDRESS/STONEX3",.type = cms::Destination::TOPIC } }, 5);


	
	publishSubscribe(10000, Destination{ .name = "TESTQUEUE",.type = cms::Destination::QUEUE }, { Destination{.name = "TESTQUEUE::STONEX3",.type = cms::Destination::QUEUE }, Destination{.name = "TESTQUEUE::STONEX3",.type = cms::Destination::QUEUE } }, 0);
	/*
    publishSubscribeGrouping(10);
    */
// 	publishSubscribeTemporaryTopic();
}