#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "visualization.h"

static const char* get_layout_name(layout_type layout) {
    switch (layout) {
        case LAYOUT_ELK: return "elk";
        case LAYOUT_DAGRE: return "dagre";
        case LAYOUT_CIRCO: return "circo";
        default: return "elk";
    }
}

void generate_advanced_mermaid_graph(const liste_adjacence *graph, const char *filename,
                                     layout_type layout, filter_type filter, float threshold) {
    if (graph == NULL || filename == NULL) return;

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erreur d'ouverture du fichier Mermaid");
        return;
    }

    fprintf(file, "---\n");
    fprintf(file, "config:\n");
    fprintf(file, "    layout: %s\n", get_layout_name(layout));
    fprintf(file, "    theme: neo\n");
    fprintf(file, "    look: neo\n");
    fprintf(file, "---\n");
    fprintf(file, "flowchart LR\n");

    // Déclarer tous les sommets
    for (int i = 0; i < graph->taille; i++) {
        fprintf(file, "    N%d((%d))\n", i + 1, i + 1);
    }
    fprintf(file, "\n");

    int total_edges = 0;
    int displayed_edges = 0;

    // Ajouter les arêtes avec filtrage
    for (int i = 0; i < graph->taille; i++) {
        list_t *liste = &graph->liste_arretes[i];
        arrete_t *current = liste->head;

        while (current != NULL) {
            total_edges++;

            int should_display = 1;

            // Appliquer le filtre
            switch (filter) {
                case FILTER_HIGH_PROB:
                    should_display = (current->probabilite >= threshold);
                    break;
                case FILTER_NONE:
                default:
                    should_display = 1;
                    break;
            }

            if (should_display) {
                fprintf(file, "    N%d-->|%.3f|N%d\n",
                        current->sommet_depart,
                        current->probabilite,
                        current->sommet_arrive);
                displayed_edges++;
            }
            current = current->sommet_suivante;
        }
    }

    fclose(file);
    printf("Fichier Mermaid avancé généré: %s\n", filename);
    printf("Arêtes: %d affichées / %d total (filtre: %.2f)\n",
           displayed_edges, total_edges, threshold);
}

void generate_simplified_graph(const liste_adjacence *graph, const char *filename,
                               float min_probability, int max_edges_per_vertex) {
    if (graph == NULL || filename == NULL) return;

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erreur d'ouverture du fichier simplifié");
        return;
    }

    fprintf(file, "---\n");
    fprintf(file, "config:\n");
    fprintf(file, "    layout: circo\n");  // Layout circulaire pour mieux voir la structure
    fprintf(file, "    theme: forest\n");
    fprintf(file, "    look: flat\n");
    fprintf(file, "---\n");
    fprintf(file, "flowchart TD\n");

    // Déclarer tous les sommets
    for (int i = 0; i < graph->taille; i++) {
        fprintf(file, "    N%d[%d]\n", i + 1, i + 1);  // Format carré pour plus de lisibilité
    }
    fprintf(file, "\n");

    int total_displayed = 0;

    // Ajouter seulement les arêtes importantes
    for (int i = 0; i < graph->taille; i++) {
        list_t *liste = &graph->liste_arretes[i];
        arrete_t *current = liste->head;
        int edge_count = 0;

        // Trier les arêtes par probabilité (simplifié)
        while (current != NULL && edge_count < max_edges_per_vertex) {
            if (current->probabilite >= min_probability) {
                // Choisir la couleur en fonction de la probabilité
                const char* color = "black";
                if (current->probabilite > 0.7) color = "green";
                else if (current->probabilite > 0.3) color = "blue";
                else color = "gray";

                fprintf(file, "    N%d-- %s %.2f -->N%d\n",
                        current->sommet_depart,
                        color,
                        current->probabilite,
                        current->sommet_arrive);
                edge_count++;
                total_displayed++;
            }
            current = current->sommet_suivante;
        }
    }

    fclose(file);
    printf("Graphe simplifié généré: %s\n", filename);
    printf("Arêtes affichées: %d (min_prob=%.2f, max_edges=%d)\n",
           total_displayed, min_probability, max_edges_per_vertex);
}

void generate_clustered_graph(const liste_adjacence *graph, const t_partition *partition,
                              const char *filename) {
    if (graph == NULL || partition == NULL || filename == NULL) return;

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erreur d'ouverture du fichier clusterisé");
        return;
    }

    fprintf(file, "---\n");
    fprintf(file, "config:\n");
    fprintf(file, "    layout: elk\n");
    fprintf(file, "    theme: dark\n");
    fprintf(file, "    look: material\n");
    fprintf(file, "---\n");
    fprintf(file, "flowchart TB\n");

    // Créer une map sommet -> classe
    int *class_map = (int *)malloc((graph->taille + 1) * sizeof(int));
    for (int i = 0; i < partition->taille; i++) {
        for (int j = 0; j < partition->classes[i].taille; j++) {
            class_map[partition->classes[i].sommets[j]] = i;
        }
    }

    // Déclarer les sous-graphes pour chaque classe
    for (int i = 0; i < partition->taille; i++) {
        fprintf(file, "    subgraph %s[Composante %s]\n",
                partition->classes[i].nom, partition->classes[i].nom);

        for (int j = 0; j < partition->classes[i].taille; j++) {
            int sommet = partition->classes[i].sommets[j];
            fprintf(file, "        N%d%d((%d))\n", i, sommet, sommet);
        }
        fprintf(file, "    end\n");
    }
    fprintf(file, "\n");

    // Ajouter les arêtes entre classes (pour le diagramme de Hasse)
    for (int i = 0; i < graph->taille; i++) {
        list_t *liste = &graph->liste_arretes[i];
        arrete_t *current = liste->head;

        while (current != NULL) {
            int class_from = class_map[current->sommet_depart];
            int class_to = class_map[current->sommet_arrive];

            if (class_from != class_to) {
                // Arête entre classes
                fprintf(file, "    N%d%d-->|%.2f|N%d%d\n",
                        class_from, current->sommet_depart,
                        current->probabilite,
                        class_to, current->sommet_arrive);
            }
            current = current->sommet_suivante;
        }
    }

    free(class_map);
    fclose(file);
    printf("Graphe clusterisé généré: %s\n", filename);
}

void generate_interactive_html(const liste_adjacence *graph, const t_partition *partition,
                               const char *filename) {
    if (graph == NULL || filename == NULL) return;

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erreur d'ouverture du fichier HTML");
        return;
    }

    fprintf(file, "<!DOCTYPE html>\n");
    fprintf(file, "<html>\n");
    fprintf(file, "<head>\n");
    fprintf(file, "    <title>Visualisation Graphe Markov</title>\n");
    fprintf(file, "    <script type=\"module\">\n");
    fprintf(file, "        import mermaid from 'https://cdn.jsdelivr.net/npm/mermaid@10/dist/mermaid.esm.min.mjs';\n");
    fprintf(file, "        mermaid.initialize({ startOnLoad: true });\n");
    fprintf(file, "    </script>\n");
    fprintf(file, "    <style>\n");
    fprintf(file, "        body { font-family: Arial, sans-serif; margin: 20px; }\n");
    fprintf(file, "        .container { display: flex; flex-direction: column; align-items: center; }\n");
    fprintf(file, "        .graph { width: 90%%; height: 600px; border: 1px solid #ccc; margin: 10px; }\n");
    fprintf(file, "        .stats { background: #f5f5f5; padding: 15px; border-radius: 5px; margin: 10px; }\n");
    fprintf(file, "    </style>\n");
    fprintf(file, "</head>\n");
    fprintf(file, "<body>\n");
    fprintf(file, "    <div class=\"container\">\n");
    fprintf(file, "        <h1>Graphe de Markov - Visualisation Interactive</h1>\n");
    fprintf(file, "        \n");
    fprintf(file, "        <div class=\"stats\">\n");
    fprintf(file, "            <h3>Statistiques du Graphe</h3>\n");
    fprintf(file, "            <p>Nombre de sommets: %d</p>\n", graph->taille);

    int total_edges = 0;
    for (int i = 0; i < graph->taille; i++) {
        list_t *liste = &graph->liste_arretes[i];
        arrete_t *current = liste->head;
        while (current != NULL) {
            total_edges++;
            current = current->sommet_suivante;
        }
    }
    fprintf(file, "            <p>Nombre d'arêtes: %d</p>\n", total_edges);

    if (partition != NULL) {
        fprintf(file, "            <p>Composantes fortement connexes: %d</p>\n", partition->taille);
    }
    fprintf(file, "        </div>\n");
    fprintf(file, "        \n");
    fprintf(file, "        <div class=\"graph\">\n");
    fprintf(file, "            <pre class=\"mermaid\">\n");

    // Générer le code Mermaid simplifié
    fprintf(file, "flowchart LR\n");
    for (int i = 0; i < graph->taille; i++) {
        fprintf(file, "    N%d((%d))\n", i + 1, i + 1);
    }
    fprintf(file, "\n");

    // Ajouter seulement les arêtes avec probabilité > 0.3 pour la lisibilité
    for (int i = 0; i < graph->taille; i++) {
        list_t *liste = &graph->liste_arretes[i];
        arrete_t *current = liste->head;

        while (current != NULL) {
            if (current->probabilite > 0.3) {
                fprintf(file, "    N%d-->|%.2f|N%d\n",
                        current->sommet_depart,
                        current->probabilite,
                        current->sommet_arrive);
            }
            current = current->sommet_suivante;
        }
    }

    fprintf(file, "            </pre>\n");
    fprintf(file, "        </div>\n");
    fprintf(file, "        \n");
    fprintf(file, "        <div class=\"stats\">\n");
    fprintf(file, "            <p><strong>Instructions:</strong> Cette visualisation utilise Mermaid.js.</p>\n");
    fprintf(file, "            <p>Vous pouvez zoomer, déplacer le graphe et cliquer sur les éléments.</p>\n");
    fprintf(file, "        </div>\n");
    fprintf(file, "    </div>\n");
    fprintf(file, "</body>\n");
    fprintf(file, "</html>\n");

    fclose(file);
    printf("Fichier HTML interactif généré: %s\n", filename);
}

void print_graph_statistics(const liste_adjacence *graph) {
    if (graph == NULL) return;

    printf("\n=== STATISTIQUES DU GRAPHE ===\n");
    printf("Nombre de sommets: %d\n", graph->taille);

    int total_edges = 0;
    int max_edges = 0;
    int min_edges = INT_MAX;
    float total_prob = 0.0;

    for (int i = 0; i < graph->taille; i++) {
        list_t *liste = &graph->liste_arretes[i];
        arrete_t *current = liste->head;
        int edge_count = 0;
        float vertex_prob = 0.0;

        while (current != NULL) {
            edge_count++;
            vertex_prob += current->probabilite;
            current = current->sommet_suivante;
        }

        total_edges += edge_count;
        total_prob += vertex_prob;

        if (edge_count > max_edges) max_edges = edge_count;
        if (edge_count < min_edges) min_edges = edge_count;
    }

    printf("Nombre total d'arêtes: %d\n", total_edges);
    printf("Arêtes par sommet: min=%d, max=%d, moyenne=%.2f\n",
           min_edges, max_edges, (float)total_edges/graph->taille);
    printf("Densité du graphe: %.2f%%\n",
           (float)total_edges / (graph->taille * graph->taille) * 100);

    // Distribution des probabilités
    int prob_ranges[5] = {0}; // 0-0.2, 0.2-0.4, 0.4-0.6, 0.6-0.8, 0.8-1.0

    for (int i = 0; i < graph->taille; i++) {
        list_t *liste = &graph->liste_arretes[i];
        arrete_t *current = liste->head;

        while (current != NULL) {
            float prob = current->probabilite;
            if (prob <= 0.2) prob_ranges[0]++;
            else if (prob <= 0.4) prob_ranges[1]++;
            else if (prob <= 0.6) prob_ranges[2]++;
            else if (prob <= 0.8) prob_ranges[3]++;
            else prob_ranges[4]++;

            current = current->sommet_suivante;
        }
    }

    printf("\nDistribution des probabilités:\n");
    printf("  0.0 - 0.2: %d arêtes (%.1f%%)\n", prob_ranges[0], (float)prob_ranges[0]/total_edges*100);
    printf("  0.2 - 0.4: %d arêtes (%.1f%%)\n", prob_ranges[1], (float)prob_ranges[1]/total_edges*100);
    printf("  0.4 - 0.6: %d arêtes (%.1f%%)\n", prob_ranges[2], (float)prob_ranges[2]/total_edges*100);
    printf("  0.6 - 0.8: %d arêtes (%.1f%%)\n", prob_ranges[3], (float)prob_ranges[3]/total_edges*100);
    printf("  0.8 - 1.0: %d arêtes (%.1f%%)\n", prob_ranges[4], (float)prob_ranges[4]/total_edges*100);
}
