#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <assert.h>

#include <jsoncpp/json/json.h>

#include <curl/curl.h>
#include <openssl/ssl.h>

//#include "message.pb.h"
#include "../../../.clion10/system/cmake/generated/bc4a843f/bc4a843f/Debug/message.pb.h"
#include "request.h"

using namespace std;

size_t calcDecodeLength(const char* b64input)
{
	size_t len = strlen(b64input),
		padding = 0;

	if (b64input[len-1] == '=' && b64input[len-2] == '=')
		padding = 2;
	else if (b64input[len-1] == '=')
		padding = 1;

	return (len*3)/4 - padding;
}

int Base64Decode(char* b64message, unsigned char** buffer, size_t* length)
{
	BIO *bio, *b64;

	int decodeLen = calcDecodeLength(b64message);
	*buffer = (unsigned char*)malloc(decodeLen + 1);
	(*buffer)[decodeLen] = '\0';

	bio = BIO_new_mem_buf(b64message, -1);
	b64 = BIO_new(BIO_f_base64());
	bio = BIO_push(b64, bio);

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
	*length = BIO_read(bio, *buffer, strlen(b64message));
	assert(*length == decodeLen);
	BIO_free_all(bio);

	return (0);
}

int Base64Encode(const char* str, char** b64text, unsigned int *b64length)
{
	BIO *bio, *b64;
	BUF_MEM *bufferPtr;

	*b64text=0;
	*b64length=0;
	size_t buf_length = strlen(str);

	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new(BIO_s_mem());
	bio = BIO_push(b64, bio);

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
	BIO_write(bio, str, buf_length);
	BIO_flush(bio);
	BIO_get_mem_ptr(bio, &bufferPtr);
	BIO_set_close(bio, BIO_NOCLOSE);
	BIO_free_all(bio);

	*b64text=(*bufferPtr).data;
	*b64length=(*bufferPtr).length;

	return (0);
}

string Base64Encode(string &ssrc)
{
	char *buf = new char[256];
	unsigned int b64length=0;
	size_t backlength=0;
	memset(buf, 0, 256);
	Base64Encode(ssrc.c_str(), &buf, &b64length);
	buf[b64length]=0;

	// verification
	//char *back = new char[256];
	//Base64Decode(buf, (unsigned char **)&back, &backlength);
	//back[backlength]=0;
	//cout << back << "\n";
	//delete back;

	string ret(buf);
	delete buf;
	return ret;
}

string get_locale()
{
	locale l("");
	string s_locale = l.name();

	std::size_t pch = s_locale.find("LC_CTYPE=", 0);

	if (pch == string::npos)
		return "en_US";

	// ToDo - detect wrong locale strings
	return s_locale.substr(pch+9, 5);

}

string get_timezone()
{
	extern long timezone;
	char buf[16];
	tzset();
	int min_off = (-timezone/60)%60;
	int hour_off = -timezone/60/60;
	sprintf(buf, "%+03i%02u", hour_off, min_off);

	string rez(buf);
	return rez;
}


string make_post_data(string login="pink007@mailinator.com", string passwd="123456")
{
	string s_action = "login";
	string s_service = "com.braininstock.ToDoChecklist";
	string s_login =  login;
	string s_password = passwd;
	string s_devicename = "FLY12345";
	string s_deviceid = "FLY54321";
	string s_platform = "Android";
	string s_platformversion = "4.4";
	string s_appversion = "0.1";
	string s_locale = get_locale();
	string s_timezone = get_timezone();

//  JSON version
//	string data = "{ \"action\":\"" + Base64Encode(s_action) + "\",";
//	data += "\"service\":\"" + Base64Encode(s_service) + "\",";
//	data += "\"login\":\"" + Base64Encode(s_login) + "\",";
//	data += "\"password\":\""+ Base64Encode(s_password) + "\",";
//	data += "\"deviceid\":\"" +Base64Encode(s_deviceid) + "\",";
//	data += "\"device\":\"" +Base64Encode(s_devicename) + "\",";
//	data += "\"platform\":\""+Base64Encode(s_platform) + "\",";
//	data += "\"platformversion\":\""+Base64Encode(s_platformversion) + "\",";
//	data += "\"appversion\":\""+Base64Encode(s_appversion) + "\",";
//	data += "\"locale\":\""+Base64Encode(s_locale) + "\",";
//	data += "\"timezone\":\""+Base64Encode(s_timezone) + "\"}";

// simple version
	string data = "action=" + Base64Encode(s_action) + "&";
	data += "service=" + Base64Encode(s_service) + "&";
	data += "login=" + Base64Encode(s_login) + "&";
	data += "password="+ Base64Encode(s_password) + "&";
	data += "deviceid=" +Base64Encode(s_deviceid) + "&";
	data += "device=" +Base64Encode(s_devicename) + "&";
	data += "platform="+Base64Encode(s_platform) + "&";
	data += "platformversion="+Base64Encode(s_platformversion) + "&";
	data += "appversion="+Base64Encode(s_appversion) + "&";
	data += "locale="+Base64Encode(s_locale) + "&";
	data += "timezone="+Base64Encode(s_timezone);

	return data;
}

size_t write_to_string(void *ptr, size_t size, size_t count, void *stream)
{
	((string*)stream)->append((char*)ptr, 0, size*count);
	return size*count;
}

int main()
{
	CURL *curl;
	CURLcode res;
	string response;

	string username = "pink007@mailinator.com";
	string userpasswd = "123456";

	//cout << "Enter username: ";
	//cin >> username;
	//cout << "Enter password: ";
	//cin >> userpasswd;

	string data = make_post_data(username, userpasswd);

	struct curl_slist *headers=NULL;

	curl = curl_easy_init();

	if(curl) {
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		curl_easy_setopt(curl, CURLOPT_URL, "https://dev-auth.simplexsolutionsinc.com/");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	 	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1);

		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.length());

		curl_easy_setopt(curl, CURLOPT_POST, 1);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		res = curl_easy_perform(curl);

		if(CURLE_OK == res) {
			char *ct;
			res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
			if((CURLE_OK == res) && ct) {
				//printf("We received Content-Type: %s\n", ct);
				//std::cout << response << "\n";
				Json::Value	parsedFromString;
				Json::Reader	reader;
				Json::StyledWriter styledWriter;
				bool parsingSuccessful = reader.parse(response, parsedFromString);
				if (parsingSuccessful)
				{
					string session_id = parsedFromString["session"].asString();
					cout << "Got session ID: " << session_id  << "\n";
					// ToDo check session_id value
					send_workgroups_list_request(session_id.c_str());
				}

			}
		}

		curl_easy_cleanup(curl);
		curl_slist_free_all(headers);
	}

	return 0;
}
