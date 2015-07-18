//
// Created by ilyas on 18.07.15.
//

#ifndef KEEPSOLIDTEST_PROTOMESSAGEPROCESSOR_H
#define KEEPSOLIDTEST_PROTOMESSAGEPROCESSOR_H


#include "MessageDispatcher.h"

template <typename ProtoQueryT, typename ProtoAnswerT>
class ProtoMessageProcessor: public MessageProcessorBase
{
public:
	virtual rpc::Request process(const rpc::Request & query)
	{
		ProtoQueryT underlyingQuery;
		if (!underlyingQuery.ParseFromString(query.data()))
		{
			throw MessageProcessingError("Failed to parse query: " +
						     query.ShortDebugString());
		}

		ProtoAnswerT underlyingAnswer = doProcessing(underlyingQuery);

		rpc::Request a;
		a.set_id(query.id());

		if (!underlyingAnswer.SerializeToString(a.mutable_data()))
		{
			throw MessageProcessingError("Failed to prepare answer: " +
						     underlyingAnswer.ShortDebugString());
		}
		return a;
	}

private:
	virtual ProtoAnswerT doProcessing(const ProtoQueryT & query) = 0;
};



#endif //KEEPSOLIDTEST_PROTOMESSAGEPROCESSOR_H
