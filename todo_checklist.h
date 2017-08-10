//
// Created by ilyas on 19.07.15.
//

#ifndef KEEPSOLIDTEST_TO_DO_CHECKLIST_H
#define KEEPSOLIDTEST_TO_DO_CHECKLIST_H

#include "message.pb.h"

class ToDoAuthentificator {
public:
	virtual std::string GetSessionID()=0;
};

class ToDoTransport {
public:
	virtual bool Connect()=0;
	virtual void Disconnect()=0;
	virtual rpc::Response & GetResponse()=0;
};

class ToDoCheckList {
protected:
	ToDoAuthentificator *todoauth;
	ToDoTransport *todotransp;
	std::string session_id;
	void DoShowTaskList(rpc::Response &response);
	template <typename AliasList, typename TaskList> void ProcessTasks(AliasList &tasks, TaskList wgList);
public:
	ToDoCheckList();
	~ToDoCheckList();
	void ShowCheckList();
};

#endif //KEEPSOLIDTEST_TO_DO_CHECKLIST_H
