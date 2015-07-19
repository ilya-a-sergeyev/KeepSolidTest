#include "todo_checklist.h"
#include "todo_post_auth.h"
#include "todo_ssl_transport.h"

int main(int ac, char *av[])
{
	ToDoCheckList *todocl = new ToDoCheckList();

	todocl->ShowCheckList();

	delete todocl;

	return 0;
}
