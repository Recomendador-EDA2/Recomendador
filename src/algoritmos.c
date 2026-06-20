#include "algoritmos.h"

/* ── FASE 1: BFS COM LOOKUP INVERSO ── */

static int foi_lido(int tid, const int *textos_lidos, int num_lidos)
{
    for (int j = 0; j < num_lidos; j++)
        if (textos_lidos[j] == tid)
            return 1;
    return 0;
}

static void processar_genero_bfs(const Digrafo *g, int atual, const int *textos_lidos, int num_lidos, int *idx_cand, Candidato *candidatos, int *num_cand)
{
    ArcInverso textos[MAX_VERTICES];
    int n = textos_do_genero(g, atual, textos);
    for (int i = 0; i < n; i++)
    {
        int tid = textos[i].origem;
        if (!foi_lido(tid, textos_lidos, num_lidos))
        {
            if (idx_cand[tid] == -1)
            {
                idx_cand[tid] = *num_cand;
                candidatos[*num_cand].texto_id = tid;
                candidatos[*num_cand].num_caminhos = 1;
                (*num_cand)++;
            }
            else
            {
                candidatos[idx_cand[tid]].num_caminhos++;
            }
        }
    }
}

int bfs_candidatos(const Digrafo *g, int uid,
                   const int *textos_lidos, int num_lidos,
                   Candidato *candidatos)
{
    int visitados[MAX_VERTICES] = {0};
    int fila[MAX_VERTICES];
    int frente = 0;
    int fim = 0;
    int num_cand = 0;

    int idx_cand[MAX_VERTICES];
    for (int i = 0; i < MAX_VERTICES; i++)
        idx_cand[i] = -1;

    visitados[uid] = 1;
    fila[fim++] = uid;

    while (frente < fim)
    {
        int atual = fila[frente++];

        for (Arco *a = g->adj[atual]; a != NULL; a = a->prox)
        {
            if (a->tipo == ARESTA_PROJECAO)
                continue;
            if (!visitados[a->destino])
            {
                visitados[a->destino] = 1;
                fila[fim++] = a->destino;
            }
        }

        if (g->vertices[atual].tipo == GENERO)
        {
            processar_genero_bfs(g, atual, textos_lidos, num_lidos, idx_cand, candidatos, &num_cand);
        }
    }
    return num_cand;
}

/* ── FASE 2: DIJKSTRA COM INVERSAO DE PESOS ── */

static int encontrar_vertice_mais_proximo(int n, const int *visitados, const double *dist)
{
    int u = -1;
    for (int i = 0; i < n; i++)
        if (!visitados[i] && (u == -1 || dist[i] < dist[u]))
            u = i;
    return u;
}

static void relaxar_arestas(const Digrafo *g, int u, double *dist)
{
    for (Arco *a = g->adj[u]; a != NULL; a = a->prox)
    {
        if (a->tipo == ARESTA_PROJECAO)
            continue;
        double custo = (a->peso > 0.0) ? (1.0 / a->peso) : INF;
        if (dist[u] + custo < dist[a->destino])
            dist[a->destino] = dist[u] + custo;
    }
}

static void processar_genero_dijkstra(const Digrafo *g, int u, double *dist)
{
    ArcInverso textos[MAX_VERTICES];
    int n_t = textos_do_genero(g, u, textos);
    for (int i = 0; i < n_t; i++)
    {
        double custo = (textos[i].peso > 0.0) ? (1.0 / textos[i].peso) : INF;
        if (dist[u] + custo < dist[textos[i].origem])
            dist[textos[i].origem] = dist[u] + custo;
    }
}

void dijkstra(const Digrafo *g, int uid, double *dist)
{
    int visitados[MAX_VERTICES] = {0};
    int n = g->numVertices;

    for (int i = 0; i < n; i++)
        dist[i] = INF;
    dist[uid] = 0.0;

    for (int iter = 0; iter < n; iter++)
    {
        int u = encontrar_vertice_mais_proximo(n, visitados, dist);
        if (u == -1 || dist[u] == INF)
            break;
        visitados[u] = 1;

        relaxar_arestas(g, u, dist);

        if (g->vertices[u].tipo == GENERO)
        {
            processar_genero_dijkstra(g, u, dist);
        }
    }
}

/* ── FASE 3: AFINIDADES E ALGORITMO DE KAHN ── */

void calcular_afinidades(const Digrafo *g, int uid, double *afinidades)
{
    for (int i = 0; i < g->numVertices; i++)
        afinidades[i] = 0.0;
    for (Arco *at = g->adj[uid]; at != NULL; at = at->prox)
    {
        if (at->tipo == ARESTA_PROJECAO)
            continue;
        double nota = at->peso;
        for (Arco *ag = g->adj[at->destino]; ag != NULL; ag = ag->prox)
        {
            if (ag->tipo == ARESTA_PROJECAO)
                continue;
            afinidades[ag->destino] += nota * ag->peso;
        }
    }
}

static void construir_graus(int ng, const int *generos, const double *afinidades, int *grau)
{
    for (int i = 0; i < ng; i++)
        for (int j = 0; j < ng; j++)
            if (i != j && afinidades[generos[i]] > afinidades[generos[j]])
                grau[generos[j]]++;
}

static int inicializar_fila(int ng, const int *generos, const int *grau, int *fila)
{
    int fim = 0;
    for (int i = 0; i < ng; i++)
        if (grau[generos[i]] == 0)
            fila[fim++] = generos[i];
    return fim;
}

static void atualizar_graus(int atual, int ng, const int *generos, const double *afinidades, int *grau, int *fila, int *fim)
{
    for (int j = 0; j < ng; j++)
    {
        if (afinidades[atual] > afinidades[generos[j]])
        {
            grau[generos[j]]--;
            if (grau[generos[j]] == 0)
                fila[(*fim)++] = generos[j];
        }
    }
}

int kahn_ranking(const Digrafo *g, const double *afinidades, int *ranking)
{
    int generos[MAX_VERTICES];
    int ng = 0;
    for (int i = 0; i < g->numVertices; i++)
        if (g->vertices[i].tipo == GENERO && afinidades[i] > 0.0)
            generos[ng++] = i;

    int grau[MAX_VERTICES] = {0};
    construir_graus(ng, generos, afinidades, grau);

    int fila[MAX_VERTICES];
    int frente = 0;
    int fim = inicializar_fila(ng, generos, grau, fila);

    int ordem = 0;
    while (frente < fim)
    {
        int melhor = frente;
        for (int k = frente + 1; k < fim; k++)
            if (afinidades[fila[k]] > afinidades[fila[melhor]])
                melhor = k;
        int tmp = fila[frente];
        fila[frente] = fila[melhor];
        fila[melhor] = tmp;

        int atual = fila[frente++];
        ranking[ordem++] = atual;

        atualizar_graus(atual, ng, generos, afinidades, grau, fila, &fim);
    }
    return ordem;
}

/* ── FASE 4: INDICE DE JACCARD ── */

static void coletar_generos_usuario(const Digrafo *g, int uid, int *generos)
{
    for (Arco *at = g->adj[uid]; at != NULL; at = at->prox)
    {
        if (at->tipo == ARESTA_PROJECAO)
            continue;
        for (Arco *ag = g->adj[at->destino]; ag != NULL; ag = ag->prox)
        {
            if (ag->tipo == ARESTA_PROJECAO)
                continue;
            generos[ag->destino] = 1;
        }
    }
}

double jaccard_usuarios(const Digrafo *g, int u1, int u2)
{
    int g1[MAX_VERTICES] = {0};
    int g2[MAX_VERTICES] = {0};

    coletar_generos_usuario(g, u1, g1);
    coletar_generos_usuario(g, u2, g2);

    int intersec = 0;
    int uniao = 0;

    for (int i = 0; i < g->numVertices; i++)
    {
        if (g->vertices[i].tipo != GENERO)
            continue;
        if (g1[i] && g2[i])
            intersec++;
        if (g1[i] || g2[i])
            uniao++;
    }
    return (uniao > 0) ? ((double)intersec / uniao) : 0.0;
}
