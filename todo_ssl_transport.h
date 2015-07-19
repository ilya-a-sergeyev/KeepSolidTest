//
// Created by ilyas on 19.07.15.
//

#ifndef KEEPSOLIDTEST_TODO_SSL_TRANSPORT_H
#define KEEPSOLIDTEST_TODO_SSL_TRANSPORT_H

#include "todo_checklist.h"

class SslToDoTransport:public ToDoTransport {
public:
	SslToDoTransport();
	virtual int GetBuffer(void **buf);
	virtual ~SslToDoTransport();
};


#endif //KEEPSOLIDTEST_TODO_SSL_TRANSPORT_H
