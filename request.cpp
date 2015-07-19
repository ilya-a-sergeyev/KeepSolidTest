//
// Created by ilyas on 19.07.15.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <openssl/err.h>

#include <assert.h>
#include <stdint.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

//#include "message.pb.h"

#include "request.h"
#include "../../../.clion10/system/cmake/generated/bc4a843f/bc4a843f/Debug/message.pb.h"

using namespace std;

int OpenConnection(const char *hostname, int port)
{
	int sd;
	struct hostent *host;
	struct sockaddr_in addr;

	if ( (host = gethostbyname(hostname)) == NULL )
	{
		perror(hostname);
		abort();
	}
	sd = socket(PF_INET, SOCK_STREAM, 0);
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = *(long*)(host->h_addr);
	if ( connect(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 )
	{
		close(sd);
		perror(hostname);
		abort();
	}
	return sd;
}

SSL_CTX* InitCTX(void)
{
	SSL_METHOD *method;
	SSL_CTX *ctx;

	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();

	method = (SSL_METHOD *)TLSv1_client_method();

	ctx = SSL_CTX_new(method);
	if ( ctx == NULL )
	{
		ERR_print_errors_fp(stderr);
		abort();
	}
	return ctx;
}


void prepare_workgroups_list_request(const char *session_id)
{
	rpc::WorkGroupsListRequest workgroupsListRequest;
	workgroupsListRequest.set_session_id(session_id);
	workgroupsListRequest.set_filter(rpc::Filter:: ALL_ENTRIES);

	rpc::Request request;
	request.set_service_type(1);
	request.set_is_debug(true);
	request.set_message_type(RPC_WORKGROUPS_LIST);
	request.set_allocated_workgroups_list(&workgroupsListRequest);

	std::string msg;
	request.SerializeToString(&msg);

	rpc::WorkGroupsListRequest ctrl;
	ctrl.ParseFromString(msg);
	ctrl.PrintDebugString();
}

struct request_hdr {
	uint32_t	size;
	uint32_t	flags;
};

int send_workgroups_list_request(const char *session_id)
{
	SSL_CTX *ctx;
	int server;
	SSL *ssl;
	int ret;

	rpc::WorkGroupsListRequest workgroupsListRequest;
	workgroupsListRequest.set_session_id(session_id);
	workgroupsListRequest.set_filter(rpc::Filter:: ALL_ENTRIES);

	rpc::Request request;
	request.set_service_type(1);
	request.set_is_debug(true);
	request.set_message_type(RPC_WORKGROUPS_LIST);
	request.set_allocated_workgroups_list(&workgroupsListRequest);

	std::string msg;
	request.SerializeToString(&msg);

	//rpc::Request ctrl;
	//ctrl.ParseFromString(msg);
	//ctrl.PrintDebugString();

	SSL_load_error_strings();
	SSL_library_init();

	ctx = InitCTX();
	server = OpenConnection("rpc.v1.keepsolid.com", 443);
	ssl = SSL_new(ctx);

	SSL_set_fd(ssl, server);

	ret = SSL_connect(ssl);
	if (ret != 1) {
		cout << "Connection error " << SSL_get_error(ssl, ret) << "\n";
	}
	else {
		cout << "Connected with " << SSL_get_cipher(ssl) << " encryption\n";

		uint64_t seq = 0;
		request_hdr rq_hdr;
		rq_hdr.size = msg.length();
		rq_hdr.flags = 0;

		SSL_write(ssl, &seq, sizeof(seq));
		SSL_write(ssl, &rq_hdr, sizeof(rq_hdr));
		SSL_write(ssl, msg.c_str(), msg.length());

		cout << "Sent request (" << sizeof(seq)+sizeof(rq_hdr)+msg.length() << " bytes)\n";

		unsigned char hdr_buf[8];
		int bytes = SSL_read(ssl, &hdr_buf, sizeof(hdr_buf));

		if (bytes == 8) {
			uint64_t *pseq = (uint64_t *)&hdr_buf;
			cout << "seq = " << *pseq;
			bytes = SSL_read(ssl, hdr_buf, sizeof(hdr_buf));
			if (bytes == 8) {
				request_hdr *rep_hdr = (request_hdr *)&hdr_buf;
				cout << " size = " << rep_hdr->size << "\n";
				// ToDo - тут хорошо бы предусмотреть контроль корректности значений
				if (rep_hdr->size>0) {
					unsigned char *reply_buf= new unsigned char[rep_hdr->size];
					if (reply_buf != NULL) {
						bytes = SSL_read(ssl, reply_buf, rep_hdr->size);
						cout << "Got " << bytes << " bytes\n";

						rpc::Response response;
						response.ParseFromArray(reply_buf, rep_hdr->size);
						response.PrintDebugString();

						delete reply_buf;
					}
					else
						cout << "No mem\n";
				}
			}

		}

		SSL_free(ssl);
	}
	close(server);
	SSL_CTX_free(ctx);

	return 0;


	return 0;
}