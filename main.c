#include <stdio.h>
#include "list.h"

int main() {
	list_t *list_test = create_list();

	/*
	* 1 1 0.95
		1 2 0.04
		1 3 0.01
	 */
	list_add_cell(list_test, 1, 0.95f);
	list_add_cell(list_test, 2, 0.04f);
	list_add_cell(list_test, 3, 0.01f);

	list_print(list_test);

	list_destroy(list_test);
	return 0;
}
