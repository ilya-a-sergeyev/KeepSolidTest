//
// Created by ilyas on 19.07.15.
//

#ifndef KEEPSOLIDTEST_TODO_SSL_TRANSPORT_H
#define KEEPSOLIDTEST_TODO_SSL_TRANSPORT_H

#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <openssl/err.h>

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
	uint64_t SequenceN;

	SSL_CTX *ctx;
	int server;
	SSL *ssl;
	bool isConnected;
public:
	SslToDoTransport(std::string sess_id);
	virtual ~SslToDoTransport();
	virtual bool Connect();
	virtual void Disconnect();
	virtual rpc::Response & GetResponse();
};


#endif //KEEPSOLIDTEST_TODO_SSL_TRANSPORT_H
