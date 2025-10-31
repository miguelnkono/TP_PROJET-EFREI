#include <stdio.h>
#include "list.h"
#include "utils.h"
#include "markov.h"
#include "tarjan.h"
#include "hasse.h"
#include "visualization.h"  // Ajout de la nouvelle inclusion

const char *file_name = "/home/gost/CLionProjects/TP_PROJET-EFREI/data/exemple";

int main() {
	printf("=== CHARGEMENT DU GRAPHE ===\n");
	liste_adjacence *graphe = readGraph(file_name);

	printf("\n=== STATISTIQUES DU GRAPHE ===\n");
	print_graph_statistics(graphe);

	printf("\n=== VERIFICATION MARKOV ===\n");
	verify_markov_property(graphe);

	// Génération des différentes visualisations
	printf("\n=== GENERATION DES VISUALISATIONS ===\n");

	// 1. Graphe complet standard
	generate_mermaid_graph(graphe, "graphe_complet.mmd");

	// 2. Graphe avec layout alternatif
	generate_advanced_mermaid_graph(graphe, "graphe_dagre.mmd", LAYOUT_DAGRE, FILTER_NONE, 0.0);

	// 3. Graphe simplifié (seulement probabilités élevées)
	generate_simplified_graph(graphe, "graphe_simplifie.mmd", 0.3, 3);

	// 4. Graphe filtré (seulement probabilités > 0.5)
	generate_advanced_mermaid_graph(graphe, "graphe_filtre.mmd", LAYOUT_ELK, FILTER_HIGH_PROB, 0.5);

	printf("\n=== ALGORITHME DE TARJAN ===\n");
	t_partition *partition = tarjan(graphe);
	if (partition != NULL) {
		print_partition(partition);

		// 5. Graphe clusterisé par composantes
		generate_clustered_graph(graphe, partition, "graphe_clusterise.mmd");

		printf("\n=== GENERATION DIAGRAMME DE HASSE ===\n");
		generate_mermaid_hasse(partition, graphe, "hasse_mermaid.mmd");

		printf("\n=== ANALYSE DES CARACTERISTIQUES ===\n");
		analyze_graph_characteristics(partition, graphe);

		// 6. Visualisation HTML interactive
		generate_interactive_html(graphe, partition, "visualisation_interactive.html");

		printf("\n=== NETTOYAGE MEMOIRE ===\n");
		free_partition(partition);
	} else {
		printf("Erreur lors de l'exécution de l'algorithme de Tarjan\n");
	}

	graph_free(graphe);

	printf("\n=== VISUALISATIONS GENEREES ===\n");
	printf("1. graphe_complet.mmd - Graphe complet standard\n");
	printf("2. graphe_dagre.mmd - Layout alternatif DAGRE\n");
	printf("3. graphe_simplifie.mmd - Version simplifiée\n");
	printf("4. graphe_filtre.mmd - Filtrage probabilités > 0.5\n");
	printf("5. graphe_clusterise.mmd - Groupé par composantes\n");
	printf("6. visualisation_interactive.html - Version web interactive\n");
	printf("7. hasse_mermaid.mmd - Diagramme de Hasse\n");

	return 0;
}