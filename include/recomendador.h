#ifndef RECOMENDADOR_H
#define RECOMENDADOR_H

#include "digrafo.h"
#include "algoritmos.h"

typedef struct {
    int texto_id;
    double score_total;
    double s_dist;
    double s_caminhos;
    double s_jaccard;
    double s_topo;
    double s_tfidf;
} Recomendacao;

int    cmp_rec(const void *a, const void *b);
double calcular_s_tfidf(int tid, const int *textos_lidos, int num_lidos,
                        double cos_mat[][MAX_VERTICES]);
int    fallback_candidatos_semanticos(const Digrafo *g,
                                      const int *textos_lidos, int num_lidos,
                                      Candidato *candidatos);
int    recomendar(const Digrafo *g, int uid, Recomendacao *resultado, int top_n,
                  double cos_mat[][MAX_VERTICES], int *usou_fallback);

#endif
