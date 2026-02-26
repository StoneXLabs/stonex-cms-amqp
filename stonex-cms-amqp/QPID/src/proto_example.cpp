#include "proto_example.h"
#include <proton/types.hpp>


ProtonHandler::ProtonHandler() {
	logger = spdlog::get("ProtoClient");
}

void ProtonHandler::on_container_start(proton::container& cont) {
	proton::connection_options co;
	co.user("CMSTEST");
	co.password("CMSTEST");
	co.sasl_allow_insecure_mechs(true);
	co.sasl_allowed_mechs("PLAIN");
	co.desired_capabilities({ "ANONYMOUS-RELAY" });
	co.handler(*this);
	cont.connect("localhost:5672", co);
}

void ProtonHandler::on_connection_open(proton::connection& conn) {

	proton::target_options topts;
	topts.capabilities(std::vector<proton::symbol>({"topic"}));

	proton::sender_options sopts;
	sopts.target(topts);
	sopts.handler(*this);


	conn.open_sender("TESTADDRESS", sopts);

	proton::receiver_options ro;
	ro.handler(*this);
	conn.open_receiver("TESTADDRESS::PROTON_QUEUE", ro);
}


void ProtonHandler::on_sender_open(proton::sender& s) {
	// sender_ and work_queue_ must be set atomically
	std::unique_lock lk(mutex);
	sender = s;
	if (!work_queue)
		work_queue = &s.work_queue();
}

void ProtonHandler::on_receiver_open(proton::receiver& r) {
	std::unique_lock lk(mutex);
	receiver = r;
	if (!work_queue)
		work_queue = &r.work_queue();
}

void ProtonHandler::on_message(proton::delivery& dlv, proton::message& msg) {
	std::unique_lock lk(mutex);
	try
	{
		logger->info(
			"Received message: {}\nCorrelationId: {}\nMessageId: {}\nto: {}\nreplyTo: {}\n"
			"subject: {}\ncontent_type: {}\ncontent_encoding: {}\nexpiry_time: {}\ncreation_time: {}\n"
			"inferred: {}\ndurable: {}\nttl: {}\npriority: {}\nfirst_acquirer: {}\ndelivery_count: {}\ngroup_id: {}\n"
			"reply_to_group_id: {}\ngroup_sequence: {}",
			"msg.body()", msg.correlation_id().empty() ? "" : proton::get<std::string>(msg.correlation_id()), msg.id().empty() ? "" : proton::get<std::string>(msg.id()), msg.to(), msg.reply_to(), msg.subject(),
			msg.content_type(), msg.content_encoding(), msg.expiry_time().milliseconds(), msg.creation_time().milliseconds(),
			msg.inferred(), msg.durable(), msg.ttl().milliseconds(), msg.priority(), msg.first_acquirer(),
			msg.delivery_count(), msg.group_id(), msg.reply_to_group_id(), msg.group_sequence()
		);


		std::map<std::string, proton::scalar> properties;
		proton::get(msg.properties(), properties);
		for (const auto& [key, value] : properties)
		{
			logger->info("{} : {}", key, "value");
		}

		proton::get(msg.message_annotations(), properties);
		for (const auto& [key, value] : properties)
		{
			logger->info("{} : {}", key, "value");
		}

		proton::get(msg.delivery_annotations(), properties);
		for (const auto& [key, value] : properties)
		{
			logger->info("{} : {}", key, "value");
		}
	}
	catch (const std::exception& e)
	{
		logger->error("Error processing message: {}", e.what());
	}

	logger->info("{}", proton::get<std::string>(msg.correlation_id()));
}

void ProtonHandler::on_error(const proton::error_condition& e) {
	std::cerr << "unexpected error: " << e << std::endl;
}


void ProtonHandler::send(const proton::message& body) {
	std::unique_lock lk(mutex);
	work_queue->add([body,this]() { sender.send(proton::message(body)); });
}