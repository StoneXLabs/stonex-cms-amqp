

#include "samples.h"
#include <log4cxx/xml/domconfigurator.h>


int main(int argc, char* argv[])
{
	log4cxx::xml::DOMConfigurator::configure("../../stonex-cms-amqp/Test/log4cxx.xml");

	publishSubscribe(10000, Destination{ "TESTADDRESS", cms::Destination::TOPIC }, { Destination{"TESTADDRESS::TESTADDRESS/STONEX1", cms::Destination::TOPIC }, Destination{"TESTADDRESS::TESTADDRESS/STONEX2", cms::Destination::TOPIC } }, 0);
	publishSubscribeText(10000, Destination{ "TESTADDRESS", cms::Destination::TOPIC }, { Destination{"TESTADDRESS::TESTADDRESS/STONEX1", cms::Destination::TOPIC }, Destination{"TESTADDRESS::TESTADDRESS/STONEX2", cms::Destination::TOPIC } }, 0);
	publishSubscribe(10000, Destination{ "TESTADDRESS", cms::Destination::TOPIC }, { Destination{"TESTADDRESS::TESTADDRESS/STONEX1", cms::Destination::TOPIC }, Destination{"TESTADDRESS::TESTADDRESS/STONEX2", cms::Destination::TOPIC }, Destination{"TESTADDRESS::TESTADDRESS/STONEX2", cms::Destination::TOPIC } }, 0);
	publishSubscribeText(10000, Destination{ "TESTADDRESS", cms::Destination::TOPIC }, { Destination{"TESTADDRESS::TESTADDRESS/STONEX1", cms::Destination::TOPIC }, Destination{"TESTADDRESS::TESTADDRESS/STONEX2", cms::Destination::TOPIC }, Destination{"TESTADDRESS::TESTADDRESS/STONEX2", cms::Destination::TOPIC } }, 0);
	publishSubscribe(10000, Destination{ "TESTADDRESS", cms::Destination::TOPIC }, { Destination{"TESTADDRESS::TESTADDRESS/STONEX3", cms::Destination::TOPIC }, Destination{"TESTADDRESS::TESTADDRESS/STONEX3", cms::Destination::TOPIC } }, 5);
	publishSubscribeText(10000, Destination{ "TESTADDRESS", cms::Destination::TOPIC }, { Destination{"TESTADDRESS::TESTADDRESS/STONEX3", cms::Destination::TOPIC }, Destination{"TESTADDRESS::TESTADDRESS/STONEX3", cms::Destination::TOPIC } }, 5);	
	publishSubscribe(10000, Destination{ "TESTQUEUE", cms::Destination::QUEUE }, { Destination{"TESTQUEUE::STONEX3", cms::Destination::QUEUE }, Destination{"TESTQUEUE::STONEX3", cms::Destination::QUEUE } }, 0);

}