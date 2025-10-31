#ifndef INC_2526_TI301_PJT_VISUALIZATION_H
#define INC_2526_TI301_PJT_VISUALIZATION_H

#include "list.h"
#include "tarjan.h"

typedef enum {
    LAYOUT_ELK,
    LAYOUT_DAGRE,
    LAYOUT_CIRCO
} layout_type;

typedef enum {
    FILTER_NONE,
    FILTER_HIGH_PROB,    // Seulement probabilités > seuil
    FILTER_TOP_EDGES,    // Seulement X arêtes par sommet
    FILTER_SCC_ONLY      // Seulement arêtes dans les SCC
} filter_type;

// Fonctions de visualisation améliorées
void generate_advanced_mermaid_graph(const liste_adjacence *graph, const char *filename,
                                     layout_type layout, filter_type filter, float threshold);
void generate_simplified_graph(const liste_adjacence *graph, const char *filename,
                               float min_probability, int max_edges_per_vertex);
void generate_clustered_graph(const liste_adjacence *graph, const t_partition *partition,
                              const char *filename);
void generate_interactive_html(const liste_adjacence *graph, const t_partition *partition,
                               const char *filename);

// Utilitaires
void print_graph_statistics(const liste_adjacence *graph);

#endif //INC_2526_TI301_PJT_VISUALIZATION_H
