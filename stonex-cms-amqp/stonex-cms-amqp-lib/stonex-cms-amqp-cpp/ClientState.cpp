#include "ClientState.h"

bool cms::amqp::StateMachine::checkState(ClientState state)
{
	return mState == state;
}

cms::amqp::ClientState cms::amqp::StateMachine::getState() const
{
	return mState;
}

void cms::amqp::StateMachine::setState(ClientState state)
{
	if (mLogger)
	{
		mLogger->log(SEVERITY::LOG_INFO, fmt::format("{} last state {} current state {}", __func__, mState, state));
	}
	mState = state;
}

void cms::amqp::StateMachine::setLogger(StonexLoggerPtr logger)
{
	mLogger = logger;
}