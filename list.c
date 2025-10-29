#include "list.h"

#include <stdio.h>
#include <stdlib.h>

arrete_t *create_cell(int sommet_arrive, float probabilite) {
	arrete_t *cellule;
	cellule = (arrete_t *) malloc(sizeof(arrete_t));
	if (NULL == cellule) {
		perror("malloc(cellule)");
		return NULL;
	}

	cellule->sommet_arrive = sommet_arrive;
	cellule->probabilite = probabilite;
	cellule->sommet_suivante = NULL;

	return cellule;
}

list_t *create_list() {
	list_t *list;
	list = (list_t *) malloc(sizeof(list_t));
	if (NULL == list) {
		perror("malloc(list)");
		return NULL;
	}
	list->head = NULL;
	list->tail = NULL;

	return list;
}

void list_add_cell(list_t *list, /*arrete_t *cellule*/ int sommet_arrive, float probabilite) {
	if (list == NULL) return;

	arrete_t *new_cell = create_cell(sommet_arrive, probabilite);
	if (new_cell == NULL) return;

	new_cell->sommet_suivante = NULL;

	// si la liste est vide.
	if (list->head == NULL) {
		list->head = new_cell;
		list->tail = new_cell;
	} else {
		list->tail->sommet_suivante = new_cell;
		list->tail = new_cell;
	}
}

void list_print(const list_t *list) {
	if (list == NULL) return;

	arrete_t *current = list->head;
	printf("Liste pour le sommet %d:[head @] ", current->sommet_arrive);
	while (current != NULL) {
		/*
		* Liste pour le sommet 1:[head @] -> (3, 0.01) @-> (2, 0.04) @-> (1, 0.95)
			Liste pour le sommet 2:[head @] -> (4, 0.05) @-> (3, 0.05) @-> (2, 0.90)
			Liste pour le sommet 3:[head @] -> (4, 0.20) @-> (3, 0.80)
			Liste pour le sommet 4:[head @] -> (1, 1.00)
		 */
		printf("-> (%d, %.3f) ", current->sommet_arrive, current->probabilite);
		current = current->sommet_suivante;
	}
}

void list_destroy(list_t *list) {
	if (list == NULL) return;
	arrete_t *current = list->head;
	while (current != NULL) {
		arrete_t *next = current->sommet_suivante;
		free(current);
		current = next;
	}
}
