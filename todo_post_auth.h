//
// Created by ilyas on 19.07.15.
//

#ifndef KEEPSOLIDTEST_TODO_POST_AUTH_H
#define KEEPSOLIDTEST_TODO_POST_AUTH_H

#include "todo_checklist.h"

//#include "message.pb.h"
#include "../../../.clion10/system/cmake/generated/bc4a843f/bc4a843f/Debug/message.pb.h"

class PostToDoAuthentificator:public ToDoAuthentificator {
protected:
	std::string session_id;
	std::string MakePostParameters(std::string login, std::string passwd);
public:
	PostToDoAuthentificator(std::string username,std::string  userpasswd);
	virtual std::string GetSessionID();
	virtual ~PostToDoAuthentificator();
};

#endif //KEEPSOLIDTEST_TODO_POST_AUTH_H
