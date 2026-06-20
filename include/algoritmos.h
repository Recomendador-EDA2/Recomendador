#ifndef ALGORITMOS_H
#define ALGORITMOS_H

#include "digrafo.h"

typedef struct {
    int texto_id;
    int num_caminhos;
} Candidato;

int    bfs_candidatos(const Digrafo *g, int uid,
                      const int *textos_lidos, int num_lidos,
                      Candidato *candidatos);
void   dijkstra(const Digrafo *g, int uid, double *dist);
void   calcular_afinidades(const Digrafo *g, int uid, double *afinidades);
int    kahn_ranking(const Digrafo *g, const double *afinidades, int *ranking);
double jaccard_usuarios(const Digrafo *g, int u1, int u2);

#endif
