#ifndef DIGRAFO_H
#define DIGRAFO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

#define MAX_VERTICES 60
#define MAX_ID_LEN   32
#define MAX_TEXTO    512
#define INF          DBL_MAX

typedef enum { USUARIO = 0, TEXTO = 1, GENERO = 2 } TipoVertice;
typedef enum { ARESTA_NORMAL = 0, ARESTA_PROJECAO = 1 } TipoAresta;

typedef struct Arco {
    int destino;
    double peso;
    TipoAresta tipo;
    struct Arco *prox;
} Arco;

typedef struct {
    char id[MAX_ID_LEN];
    TipoVertice tipo;
    char texto[MAX_TEXTO];
} Vertice;

typedef struct {
    Vertice vertices[MAX_VERTICES];
    Arco *adj[MAX_VERTICES];
    int numVertices;
} Digrafo;

typedef struct {
    int origem;
    double peso;
} ArcInverso;

void inicializar_digrafo(Digrafo *g);
int  buscar_vertice(const Digrafo *g, const char *id);
int  adicionar_vertice(Digrafo *g, const char *id, TipoVertice tipo);
void definir_conteudo(Digrafo *g, int idx, const char *txt);
void adicionar_arco(Digrafo *g, int origem, int destino, double peso);
void adicionar_arco_projecao(Digrafo *g, int origem, int destino, double peso);
void liberar_digrafo(Digrafo *g);
int  textos_do_genero(const Digrafo *g, int gid, ArcInverso *resultado);

#endif
