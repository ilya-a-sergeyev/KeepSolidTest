//
// Created by ilyas on 18.07.15.
//

#ifndef KEEPSOLIDTEST_MESSAGEDISPATCHER_H
#define KEEPSOLIDTEST_MESSAGEDISPATCHER_H


#include <map>
#include <stdexcept>

#include <boost/noncopyable.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include "../../../.clion10/system/cmake/generated/bc4a843f/bc4a843f/Debug/message.pb.h"

class MessageProcessingError: public std::runtime_error
{
public:
	MessageProcessingError(const std::string & e): std::runtime_error(e)
	{
	}
};

class MessageProcessorBase: private boost::noncopyable
{
public:

	virtual ~MessageProcessorBase()
	{
	}

	virtual std::string id() const = 0;

	virtual rpc::Request processRequest(const rpc::Request & query) = 0;
};

typedef boost::shared_ptr<MessageProcessorBase> MessageProcessorBasePtr;

class MessageDispatcher
{
public:
	MessageDispatcher();

	void addProcessor(MessageProcessorBasePtr processor);

	rpc::Request dispatch(const rpc::Request & query);

	typedef std::map<std::string, MessageProcessorBasePtr> DispatcherImplType;

	const DispatcherImplType & impl() const;

private:
	DispatcherImplType mImpl;
};

#endif //KEEPSOLIDTEST_MESSAGEDISPATCHER_H
