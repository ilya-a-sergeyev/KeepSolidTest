//
// Created by ilyas on 19.07.15.
//

#ifndef KEEPSOLIDTEST_TODO_POST_AUTH_H
#define KEEPSOLIDTEST_TODO_POST_AUTH_H

#include <string>
#include "todo_checklist.h"

class PostToDoAuthentificator:public ToDoAuthentificator {
protected:
	std::string session_id;
public:
	PostToDoAuthentificator();
	virtual std::string GetSessionID();
	virtual ~PostToDoAuthentificator();
};

#endif //KEEPSOLIDTEST_TODO_POST_AUTH_H
