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


template <typename WG> void processWorkgroup(WG &workGroupIt, std::string &task_alias)
{
	if (workGroupIt.workgroup_type()!=1001) {
		return;
	}

	Json::Value  parsedTask;
	Json::Reader reader;

	if (!reader.parse(workGroupIt.workgroup_metadata(), parsedTask)) {
		return;
	}

	string taskTitle = parsedTask["title"].asString();
	bool task_completed = parsedTask["completed"].asBool();

	if (task_alias == parsedTask["alias"].asString()) {
		return;
	}

	if (workGroupIt.is_deleted()) {
		cout << "\t\tTask: " << parsedTask["alias"].asString() << " deleted\n";
		return;
	}

	cout << "\t\tTask: " << taskTitle;
	if (task_completed) cout << "(completed)";
	cout << "\n";
}

template <typename AliasList, typename TaskList> void ToDoCheckList::ProcessTasks(AliasList &tasks, TaskList wgList)
{
	for (int j=0; j<tasks.size(); j++) {
		std::string task_alias = tasks[j].asString();
		for (auto wg: wgList) {
			processWorkgroup(wg, task_alias);
		}
	}
}

void ToDoCheckList::DoShowTaskList(rpc::Response &response)
{
	// full output
	//response.PrintDebugString();

	rpc::WorkGroupsListResponse workGroupsListResponse = response.workgroups_list();
	int size = workGroupsListResponse.workgroup_info_list_size();
	int i, j, k;

	for (auto wg: workGroupsListResponse.workgroup_info_list()) {

		if (wg.workgroup_type()==1002) {
			string workgroup_name = wg.workgroup_name();
			bool workgroup_deleted = wg.is_deleted();

			Json::Value parsedTaskList;
			Json::Reader readerTaskList;

			if (!readerTaskList.parse(wg.workgroup_metadata(), parsedTaskList)) {
				continue;
			}

			if (workgroup_deleted) {
				continue;
			}

			string tasklistTtitle = parsedTaskList["title"].asString();
			cout << "Tasklist: " << tasklistTtitle << "\n";

			ProcessTasks(parsedTaskList["tasks"], workGroupsListResponse.workgroup_info_list());
		}
	}
}

void ToDoCheckList::ShowCheckList()
{
	rpc::Response response = todotransp->GetResponse();

	if (response.error_code() == 200)
		DoShowTaskList(response);
}
