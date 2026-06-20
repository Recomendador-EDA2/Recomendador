#ifndef RELATORIO_H
#define RELATORIO_H

#include "digrafo.h"
#include "vocab.h"
#include "recomendador.h"

void imprimir_relatorio_fase0(const Digrafo *g, const Vocabulario *vocab,
                              int num_docs, int arestas_genero,
                              double tfidf[][MAX_VOCAB],
                              double cos_mat[][MAX_VERTICES]);
void imprimir_recomendacoes(const Digrafo *g, const Recomendacao *resultado,
                            int n, int usou_fallback);

#endif
