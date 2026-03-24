

#include "samples.h"
#include <log4cxx/xml/domconfigurator.h>


int main(int argc, char* argv[])
{
	log4cxx::xml::DOMConfigurator::configure("../../stonex-cms-amqp/Test/log4cxx.xml");

	publishSubscribe(10000, Destination{ .name = "TESTADDRESS",.type = cms::Destination::TOPIC }, { Destination{.name = "TESTADDRESS::TESTADDRESS/STONEX1",.type = cms::Destination::TOPIC }, Destination{.name = "TESTADDRESS::TESTADDRESS/STONEX2",.type = cms::Destination::TOPIC } }, 0);
	publishSubscribeText(10000, Destination{ .name = "TESTADDRESS",.type = cms::Destination::TOPIC }, { Destination{.name = "TESTADDRESS::TESTADDRESS/STONEX1",.type = cms::Destination::TOPIC }, Destination{.name = "TESTADDRESS::TESTADDRESS/STONEX2",.type = cms::Destination::TOPIC } }, 0);
	publishSubscribe(10000, Destination{ .name = "TESTADDRESS",.type = cms::Destination::TOPIC }, { Destination{.name = "TESTADDRESS::TESTADDRESS/STONEX1",.type = cms::Destination::TOPIC }, Destination{.name = "TESTADDRESS::TESTADDRESS/STONEX2",.type = cms::Destination::TOPIC }, Destination{.name = "TESTADDRESS::TESTADDRESS/STONEX2",.type = cms::Destination::TOPIC } }, 0);
	publishSubscribeText(10000, Destination{ .name = "TESTADDRESS",.type = cms::Destination::TOPIC }, { Destination{.name = "TESTADDRESS::TESTADDRESS/STONEX1",.type = cms::Destination::TOPIC }, Destination{.name = "TESTADDRESS::TESTADDRESS/STONEX2",.type = cms::Destination::TOPIC }, Destination{.name = "TESTADDRESS::TESTADDRESS/STONEX2",.type = cms::Destination::TOPIC } }, 0);
	publishSubscribe(10000, Destination{ .name = "TESTADDRESS",.type = cms::Destination::TOPIC }, { Destination{.name = "TESTADDRESS::TESTADDRESS/STONEX3",.type = cms::Destination::TOPIC }, Destination{.name = "TESTADDRESS::TESTADDRESS/STONEX3",.type = cms::Destination::TOPIC } }, 5);
	publishSubscribeText(10000, Destination{ .name = "TESTADDRESS",.type = cms::Destination::TOPIC }, { Destination{.name = "TESTADDRESS::TESTADDRESS/STONEX3",.type = cms::Destination::TOPIC }, Destination{.name = "TESTADDRESS::TESTADDRESS/STONEX3",.type = cms::Destination::TOPIC } }, 5);	
	publishSubscribe(10000, Destination{ .name = "TESTQUEUE",.type = cms::Destination::QUEUE }, { Destination{.name = "TESTQUEUE::STONEX3",.type = cms::Destination::QUEUE }, Destination{.name = "TESTQUEUE::STONEX3",.type = cms::Destination::QUEUE } }, 0);

}