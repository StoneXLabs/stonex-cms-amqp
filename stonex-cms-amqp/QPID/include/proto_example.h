
#include <proton/container.hpp>
#include <proton/messaging_handler.hpp>
#include <proton/message.hpp>
#include <proton/message_id.hpp>
#include <proton/receiver.hpp>
#include <proton/sender.hpp>
#include <proton/work_queue.hpp>
#include <proton/connection_options.hpp>
#include <proton/target_options.hpp>
#include <proton/sender_options.hpp>
#include <proton/receiver_options.hpp>

#include <mutex>
#include <iostream>
#include <thread>
#include <spdlog/spdlog.h>
#include <spdlog_setup/conf.h>

class ProtonHandler : public proton::messaging_handler
{
public:
ProtonHandler();
void on_container_start(proton::container& cont) override;
void on_connection_open(proton::connection& conn) override;
void on_sender_open(proton::sender& s) override ;
void on_receiver_open(proton::receiver& r) override;
void on_message(proton::delivery& dlv, proton::message& msg) override;
void on_error(const proton::error_condition& e);
void send(const proton::message& body);

private:

proton::work_queue* work_queue{ nullptr };
std::mutex mutex;
proton::sender sender;
proton::receiver receiver;
std::shared_ptr<spdlog::logger> logger;

};