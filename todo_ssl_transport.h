//
// Created by ilyas on 19.07.15.
//

#ifndef KEEPSOLIDTEST_TODO_SSL_TRANSPORT_H
#define KEEPSOLIDTEST_TODO_SSL_TRANSPORT_H

#include "todo_checklist.h"

//#include "message.pb.h"
#include "../../../.clion10/system/cmake/generated/bc4a843f/bc4a843f/Debug/message.pb.h"

class SslToDoTransport:public ToDoTransport {
protected:
	std::string session_id;
	rpc::WorkGroupsListRequest workgroupsListRequest;
	rpc::Request request;
	rpc::Response response;
	std::string msg;
public:
	SslToDoTransport(std::string sess_id);
	virtual rpc::Response & GetResponse();
	virtual ~SslToDoTransport();
};


#endif //KEEPSOLIDTEST_TODO_SSL_TRANSPORT_H
