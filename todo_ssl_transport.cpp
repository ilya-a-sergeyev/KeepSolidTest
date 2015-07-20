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

#include <jsoncpp/json/json.h>

#include "todo_ssl_transport.h"

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


SslToDoTransport::SslToDoTransport(std::string sess_id)
{
	session_id = sess_id;
	SSL_load_error_strings();
	SSL_library_init();
	ctx = InitCTX();
	Connect();
}

SslToDoTransport::~SslToDoTransport()
{
	Disconnect();
}

bool SslToDoTransport::Connect()
{
	int ret;
	server = OpenConnection("rpc.v1.keepsolid.com", 443);
	ssl = SSL_new(ctx);
	SSL_set_fd(ssl, server);
	ret = SSL_connect(ssl);

	if (ret != 1) {
		cout << "Connection error " << SSL_get_error(ssl, ret) << "\n";
		isConnected = false;
	}
	else
		isConnected = true;

	SequenceN = 0;

	return isConnected;
}

void SslToDoTransport::Disconnect()
{
	if (isConnected) {
		close(server);
		SSL_CTX_free(ctx);
		SSL_free(ssl);
		isConnected=false;
	}
}

rpc::Response & SslToDoTransport::GetResponse()
{
	int ret;

	response.clear_workgroups_list();

	if (!isConnected)
		if (!Connect())
			return response;

	workgroupsListRequest.set_session_id(session_id);
	workgroupsListRequest.set_filter(rpc::Filter:: ALL_ENTRIES);

	request.set_service_type(1);
	request.set_is_debug(true);
	request.set_message_type(RPC_WORKGROUPS_LIST);
	request.set_allocated_workgroups_list(&workgroupsListRequest);

	request.SerializeToString(&msg);

	uint64_t seq = SequenceN;
	request_hdr rq_hdr;
	rq_hdr.size = msg.length();
	rq_hdr.flags = 0;

	SSL_write(ssl, &seq, sizeof(seq));
	SSL_write(ssl, &rq_hdr, sizeof(rq_hdr));
	SSL_write(ssl, msg.c_str(), msg.length());

	unsigned char hdr_buf[8];
	int bytes = SSL_read(ssl, &hdr_buf, sizeof(hdr_buf));

	if (bytes == 8) {
		uint64_t *pseq = (uint64_t *)&hdr_buf;
		if (seq+1 != *pseq) {
			cout << "Protocol error: bad sequence number (" << seq << "  " << *pseq + ")\n";
		}
		else {
			SequenceN = *pseq+1;
			bytes = SSL_read(ssl, hdr_buf, sizeof(hdr_buf));
			if (bytes == 8) {
				request_hdr *rep_hdr = (request_hdr *)&hdr_buf;
				// ToDo - тут хорошо бы предусмотреть контроль корректности значений
				if (rep_hdr->size>0) {
					unsigned char *reply_buf= new unsigned char[rep_hdr->size];
					bytes = SSL_read(ssl, reply_buf, rep_hdr->size);

					if (bytes == rep_hdr->size)
						// We did it!
						response.ParseFromArray(reply_buf, rep_hdr->size);

					delete reply_buf;
				}
			}
		}
	}

	return response;
}

