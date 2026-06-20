#include "relatorio.h"

static void imprimir_matriz_texto_genero(const Digrafo *g, const Vocabulario *vocab, double tfidf[][MAX_VOCAB])
{
    printf("Categorizacao DINAMICA Texto -> Genero:\n");
    printf("Matriz de similaridade cosseno (texto x genero):\n");
    printf("        ");
    for (int j = 0; j < g->numVertices; j++)
        if (g->vertices[j].tipo == GENERO)
            printf("%-10s", g->vertices[j].id);
    printf("\n");
    for (int i = 0; i < g->numVertices; i++)
    {
        if (g->vertices[i].tipo != TEXTO)
            continue;
        printf("  %-6s", g->vertices[i].id);
        for (int j = 0; j < g->numVertices; j++)
        {
            if (g->vertices[j].tipo != GENERO)
                continue;
            double s = cosseno((double *)tfidf[i], (double *)tfidf[j], vocab->tamanho);
            printf("%-10.3f", s);
        }
        printf("\n");
    }
}

static void imprimir_arestas_texto_genero(const Digrafo *g, int arestas_genero)
{
    printf("\nArestas T->G criadas (cosseno > %.2f): %d\n", LIMIAR_GENERO, arestas_genero);
    for (int i = 0; i < g->numVertices; i++)
    {
        if (g->vertices[i].tipo != TEXTO)
            continue;
        for (Arco *a = g->adj[i]; a != NULL; a = a->prox)
        {
            if (a->tipo == ARESTA_NORMAL && g->vertices[a->destino].tipo == GENERO)
                printf("  %-3s -> %-9s : %.3f\n",
                       g->vertices[i].id, g->vertices[a->destino].id, a->peso);
        }
    }
    printf("\n");
}

static void imprimir_matriz_texto_texto(const Digrafo *g, double cos_mat[][MAX_VERTICES])
{
    printf("Matriz de similaridade cosseno (texto x texto):\n");
    printf("        ");
    for (int j = 0; j < g->numVertices; j++)
        if (g->vertices[j].tipo == TEXTO)
            printf("%-8s", g->vertices[j].id);
    printf("\n");
    for (int i = 0; i < g->numVertices; i++)
    {
        if (g->vertices[i].tipo != TEXTO)
            continue;
        printf("  %-6s", g->vertices[i].id);
        for (int j = 0; j < g->numVertices; j++)
        {
            if (g->vertices[j].tipo != TEXTO)
                continue;
            printf("%-8.3f", cos_mat[i][j]);
        }
        printf("\n");
    }
}

static void imprimir_arestas_projecao(const Digrafo *g, double cos_mat[][MAX_VERTICES])
{
    printf("\nArestas de projecao T->T criadas (cosseno > theta):\n");
    for (int i = 0; i < g->numVertices; i++)
    {
        if (g->vertices[i].tipo != TEXTO)
            continue;
        for (int j = i + 1; j < g->numVertices; j++)
        {
            if (g->vertices[j].tipo != TEXTO)
                continue;
            if (cos_mat[i][j] > LIMIAR_THETA)
                printf("  %s <-> %s : %.3f\n",
                       g->vertices[i].id, g->vertices[j].id, cos_mat[i][j]);
        }
    }
}

void imprimir_relatorio_fase0(const Digrafo *g, const Vocabulario *vocab,
                              int num_docs, int arestas_genero,
                              double tfidf[][MAX_VOCAB],
                              double cos_mat[][MAX_VERTICES])
{
    printf("==================================================\n");
    printf(" FASE 0 — PLN: TF-IDF e PROJECAO TEXTO-TEXTO\n");
    printf("==================================================\n");
    printf("Documentos processados : %d\n", num_docs);
    printf("Termos no vocabulario  : %d\n", vocab->tamanho);
    printf("Limiar theta (T-T)     : %.2f\n", LIMIAR_THETA);
    printf("Limiar genero (T-G)    : %.2f\n\n", LIMIAR_GENERO);

    imprimir_matriz_texto_genero(g, vocab, tfidf);
    imprimir_arestas_texto_genero(g, arestas_genero);
    imprimir_matriz_texto_texto(g, cos_mat);
    imprimir_arestas_projecao(g, cos_mat);
}

void imprimir_recomendacoes(const Digrafo *g, const Recomendacao *resultado,
                            int n, int usou_fallback)
{
    printf("\n==================================================\n");
    printf(" RECOMENDACOES PARA: u1\n");
    printf("==================================================\n");
    if (usou_fallback && n > 0)
    {
        printf("[BFS por genero nao achou candidatos — usando FALLBACK\n");
        printf(" SEMANTICO via projecao T->T sobre os textos lidos]\n");
    }
    if (n == 0)
    {
        printf("Nenhum texto candidato encontrado para u1.\n");
        printf("Causa: os generos alcancados por u1 nao apontam para nenhum\n");
        printf("texto ainda nao lido (clusters tematicos sem ponte de genero).\n");
    }
    for (int i = 0; i < n; i++)
    {
        printf("\n[%d] Texto : %-8s | Score Total: %.4f\n",
               i + 1, g->vertices[resultado[i].texto_id].id,
               resultado[i].score_total);
        printf("    Dijkstra  (s_dist)    : %.4f\n", resultado[i].s_dist);
        printf("    BFS       (s_caminhos): %.4f\n", resultado[i].s_caminhos);
        printf("    Jaccard   (s_jaccard) : %.4f\n", resultado[i].s_jaccard);
        printf("    Kahn      (s_topo)    : %.4f\n", resultado[i].s_topo);
        printf("    TF-IDF    (s_tfidf)   : %.4f\n", resultado[i].s_tfidf);
    }
}
