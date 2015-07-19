//
// Created by ilyas on 19.07.15.
//

#ifndef KEEPSOLIDTEST_TO_DO_CHECKLIST_H
#define KEEPSOLIDTEST_TO_DO_CHECKLIST_H

#include <string>

class ToDoAuthentificator {
public:
	virtual std::string GetSessionID()=0;
};

class ToDoTransport {
public:
	virtual int GetBuffer(void **buf)=0;
};

class ToDoCheckList {
protected:
	ToDoAuthentificator * authentificator;
	ToDoTransport * transport;
public:
	ToDoCheckList();
	~ToDoCheckList();
};

#endif //KEEPSOLIDTEST_TO_DO_CHECKLIST_H
