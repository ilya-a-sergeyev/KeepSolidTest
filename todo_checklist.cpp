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

using namespace std;

#include "todo_checklist.h"
#include "todo_ssl_transport.h"
#include "todo_post_auth.h"

ToDoCheckList::ToDoCheckList()
{
	string username = "pink007@mailinator.com";
	string userpasswd = "123456";

	//cout << "Enter username: ";
	//cin >> username;
	//cout << "Enter password: ";
	//cin >> userpasswd;

	// ToDo - проверку на то что аутентификация не провалена, вынести попытку аутентификации в отдельный метод
	todoauth = new PostToDoAuthentificator(username, userpasswd);
	session_id = todoauth->GetSessionID();
	todotransp = new SslToDoTransport(session_id);
}


ToDoCheckList::~ToDoCheckList()
{
	delete todotransp;
	delete todoauth;
}

//
// ToDo - уменьшить количество уровней иерархии
//
void ToDoCheckList::DoShowTaskList(rpc::Response &response)
{
	// full output
	//response.PrintDebugString();

	rpc::WorkGroupsListResponse workGroupsListResponse = response.workgroups_list();
	int size = workGroupsListResponse.workgroup_info_list_size();
	int i, j, k;

	for (i=0; i<size; i++) {
		if (workGroupsListResponse.workgroup_info_list(i).workgroup_type()==1002) {
			string workgroup_name = workGroupsListResponse.workgroup_info_list(i).workgroup_name();
			bool workgroup_deleted = workGroupsListResponse.workgroup_info_list(i).is_deleted();

			Json::Value parsedTaskList;
			Json::Reader readerTaskList;

			bool parsingSuccessful =
				readerTaskList.parse(workGroupsListResponse.workgroup_info_list(i).workgroup_metadata(),
						     parsedTaskList);

			if (parsingSuccessful) {
				string tasklistTtitle = parsedTaskList["title"].asString();

				if (!workgroup_deleted)
					cout << "Tasklist: " << tasklistTtitle << "\n";

				Json::Value tasks = parsedTaskList["tasks"];
				for (j=0; j<tasks.size(); j++) {
					Json::Value task_alias = tasks[j];

					for (k=0; k<size; k++) {
						if (workGroupsListResponse.workgroup_info_list(k).workgroup_type()==1001) {
							bool task_deleted = workGroupsListResponse.workgroup_info_list(k).is_deleted();

							Json::Value parsedTask;
							Json::Reader reader;

							bool parsingSuccessful =
								reader.parse(workGroupsListResponse.workgroup_info_list(
										     k).workgroup_metadata(),
									     parsedTask);

							if (parsingSuccessful) {

								if (parsedTask["alias"].asString() == task_alias.asString()) {
									string taskTitle = parsedTask["title"].asString();
									bool task_completed = parsedTask["completed"].asBool();


									if (!task_deleted) {
										cout << "\t\tTask: " << taskTitle;
										if (task_completed)
											cout << "(completed)";
										cout << "\n";
									}
								}
							}
							else
								cout << "\t\tTask: " << task_alias.asString() << "\n";
						}
					}
				}
			}
		}
	}
}

void ToDoCheckList::ShowCheckList()
{
	rpc::Response response = todotransp->GetResponse();

	if (response.error_code() == 200)
		DoShowTaskList(response);
}
