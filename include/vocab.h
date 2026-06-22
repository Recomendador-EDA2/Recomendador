#ifndef VOCAB_H
#define VOCAB_H

#include "digrafo.h"

#define MAX_VOCAB     512
#define MAX_TERMO     40
#define HASH_SIZE     509
#define LIMIAR_THETA  0.10
#define LIMIAR_GENERO 0.15

typedef struct {
    char termo[MAX_VOCAB][MAX_TERMO];
    int df[MAX_VOCAB];
    int tamanho;
    int bucket[HASH_SIZE];
} Vocabulario;

void inicializar_vocab(Vocabulario *v);
int  vocab_indice(Vocabulario *v, const char *termo);
int  construir_vocab_e_contagens(const Digrafo *g, Vocabulario *v,
                                 int tf_cont[][MAX_VOCAB],
                                 int *total_tokens);
void calcular_tfidf(const Digrafo *g, const Vocabulario *v, int num_docs,
                    int tf_cont[][MAX_VOCAB], const int *total_tokens,
                    double tfidf[][MAX_VOCAB]);
double cosseno(const double *va, const double *vb, int n);
void construir_projecao_texto(Digrafo *g, const Vocabulario *v,
                              double tfidf[][MAX_VOCAB],
                              double cos_mat[][MAX_VERTICES]);
int  construir_ligacoes_genero(Digrafo *g, const Vocabulario *v,
                               double tfidf[][MAX_VOCAB]);

#endif
