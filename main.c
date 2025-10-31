#include <stdio.h>
#include "list.h"
#include "utils.h"

const char *file_name = "/home/gost/CLionProjects/TP_PROJET-EFREI/data/exemple1_from_chatGPT.txt";

int main() {
	liste_adjacence *graphe = readGraph(file_name);

	graph_print(graphe);

	graph_free(graphe);

	return 0;
}
