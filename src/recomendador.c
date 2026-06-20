#include "recomendador.h"
#include "vocab.h"

int cmp_rec(const void *a, const void *b)
{
    double d = ((const Recomendacao *)b)->score_total - ((const Recomendacao *)a)->score_total;
    return (d > 0) - (d < 0);
}

/* s_tfidf(t) = media das similaridades cosseno entre o candidato t e os
   textos ja lidos pelo usuario-alvo (canal semantico de PLN). */
double calcular_s_tfidf(int tid, const int *textos_lidos, int num_lidos,
                        double cos_mat[][MAX_VERTICES])
{
    if (num_lidos == 0)
        return 0.0;
    double soma = 0.0;
    for (int i = 0; i < num_lidos; i++)
        soma += cos_mat[tid][textos_lidos[i]];
    return soma / num_lidos;
}

/* FALLBACK SEMANTICO (PLN) */
static int foi_lido(int tid, const int *textos_lidos, int num_lidos)
{
    for (int j = 0; j < num_lidos; j++)
        if (textos_lidos[j] == tid)
            return 1;
    return 0;
}

int fallback_candidatos_semanticos(const Digrafo *g,
                                   const int *textos_lidos, int num_lidos,
                                   Candidato *candidatos)
{
    int num_cand = 0;
    int idx_cand[MAX_VERTICES];
    for (int i = 0; i < MAX_VERTICES; i++)
        idx_cand[i] = -1;

    for (int l = 0; l < num_lidos; l++)
    {
        int r = textos_lidos[l];
        for (Arco *a = g->adj[r]; a != NULL; a = a->prox)
        {
            if (a->tipo != ARESTA_PROJECAO)
                continue;
            int viz = a->destino;

            if (foi_lido(viz, textos_lidos, num_lidos))
                continue;

            if (idx_cand[viz] == -1)
            {
                idx_cand[viz] = num_cand;
                candidatos[num_cand].texto_id = viz;
                candidatos[num_cand].num_caminhos = 1;
                num_cand++;
            }
            else
            {
                candidatos[idx_cand[viz]].num_caminhos++;
            }
        }
    }
    return num_cand;
}

static double calcular_s_jac(const Digrafo *g, int tid, int uid, const double *j_scores)
{
    double s_jac = 0.0;
    for (int u = 0; u < g->numVertices; u++)
    {
        if (g->vertices[u].tipo != USUARIO || u == uid)
            continue;
        for (Arco *a = g->adj[u]; a != NULL; a = a->prox)
        {
            if (a->tipo != ARESTA_PROJECAO && a->destino == tid)
            {
                s_jac += j_scores[u];
                break;
            }
        }
    }
    return s_jac;
}

static double calcular_s_top(const Digrafo *g, int tid, const int *ranking, int num_rank)
{
    int pos = num_rank;
    for (Arco *a = g->adj[tid]; a != NULL; a = a->prox)
    {
        if (a->tipo == ARESTA_PROJECAO)
            continue;
        for (int r = 0; r < num_rank; r++)
        {
            if (ranking[r] == a->destino && r < pos)
                pos = r;
        }
    }
    return 1.0 / (pos + 1);
}

int recomendar(const Digrafo *g, int uid, Recomendacao *resultado, int top_n,
               double cos_mat[][MAX_VERTICES], int *usou_fallback)
{
    int textos_lidos[MAX_VERTICES];
    int num_lidos = 0;
    for (Arco *a = g->adj[uid]; a != NULL; a = a->prox)
        if (a->tipo != ARESTA_PROJECAO)
            textos_lidos[num_lidos++] = a->destino;

    Candidato candidatos[MAX_VERTICES];
    int num_cand = bfs_candidatos(g, uid, textos_lidos, num_lidos, candidatos);

    *usou_fallback = 0;
    if (num_cand == 0)
    {
        num_cand = fallback_candidatos_semanticos(g, textos_lidos, num_lidos,
                                                  candidatos);
        *usou_fallback = 1;
    }
    if (num_cand == 0)
        return 0;

    double dist[MAX_VERTICES];
    dijkstra(g, uid, dist);

    double afinidades[MAX_VERTICES];
    calcular_afinidades(g, uid, afinidades);
    int ranking[MAX_VERTICES];
    int num_rank = kahn_ranking(g, afinidades, ranking);

    double j_scores[MAX_VERTICES] = {0.0};
    for (int i = 0; i < g->numVertices; i++)
        if (g->vertices[i].tipo == USUARIO && i != uid)
            j_scores[i] = jaccard_usuarios(g, uid, i);

    for (int c = 0; c < num_cand; c++)
    {
        int tid = candidatos[c].texto_id;

        double d = dist[tid];
        double s_dist = (d > 0.0 && d < INF) ? (1.0 / d) : 0.0;
        double s_cam = candidatos[c].num_caminhos * 0.1;

        double s_jac = calcular_s_jac(g, tid, uid, j_scores);
        double s_top = calcular_s_top(g, tid, ranking, num_rank);
        double s_tf = calcular_s_tfidf(tid, textos_lidos, num_lidos, cos_mat);

        resultado[c] = (Recomendacao){
            tid, s_dist + s_cam + s_jac + s_top + s_tf,
            s_dist, s_cam, s_jac, s_top, s_tf};
    }

    qsort(resultado, num_cand, sizeof(Recomendacao), cmp_rec);
    return (num_cand < top_n) ? num_cand : top_n;
}
