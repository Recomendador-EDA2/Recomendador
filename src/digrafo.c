#include "digrafo.h"

void inicializar_digrafo(Digrafo *g)
{
    g->numVertices = 0;
    for (int i = 0; i < MAX_VERTICES; i++)
        g->adj[i] = NULL;
}

/* Busca linear por ID — O(|V|). */
int buscar_vertice(const Digrafo *g, const char *id)
{
    for (int i = 0; i < g->numVertices; i++)
        if (strcmp(g->vertices[i].id, id) == 0)
            return i;
    return -1;
}

/* Adiciona vertice. Idempotente. Inicializa o conteudo textual vazio.
   Retorna o indice do vertice, ou -1 se a capacidade MAX_VERTICES estourar. */
int adicionar_vertice(Digrafo *g, const char *id, TipoVertice tipo)
{
    int idx = buscar_vertice(g, id);
    if (idx != -1)
        return idx;
    if (g->numVertices >= MAX_VERTICES)
    {
        fprintf(stderr, "ERRO: limite de %d vertices excedido ao inserir '%s'.\n",
                MAX_VERTICES, id);
        return -1;
    }
    idx = g->numVertices++;
    strncpy(g->vertices[idx].id, id, MAX_ID_LEN - 1);
    g->vertices[idx].id[MAX_ID_LEN - 1] = '\0';
    g->vertices[idx].tipo = tipo;
    g->vertices[idx].texto[0] = '\0';
    g->adj[idx] = NULL;
    return idx;
}

/* Associa um conteudo textual a um vertice. Avisa se houver truncamento. */
void definir_conteudo(Digrafo *g, int idx, const char *txt)
{
    if (strlen(txt) > MAX_TEXTO - 1)
        fprintf(stderr, "AVISO: conteudo de '%s' truncado para %d caracteres.\n",
                g->vertices[idx].id, MAX_TEXTO - 1);
    strncpy(g->vertices[idx].texto, txt, MAX_TEXTO - 1);
    g->vertices[idx].texto[MAX_TEXTO - 1] = '\0';
}

/* Nucleo de insercao de arco, parametrizado pelo tipo de aresta. */
static void inserir_arco(Digrafo *g, int origem, int destino,
                         double peso, TipoAresta tipo)
{
    for (Arco *a = g->adj[origem]; a != NULL; a = a->prox)
    {
        if (a->destino == destino && a->tipo == tipo)
        {
            a->peso = peso;
            return;
        }
    }
    Arco *novo = (Arco *)malloc(sizeof(Arco));
    novo->destino = destino;
    novo->peso = peso;
    novo->tipo = tipo;
    novo->prox = g->adj[origem];
    g->adj[origem] = novo;
}

/* Aresta normal (U->T ou T->G): preserva a interface original. */
void adicionar_arco(Digrafo *g, int origem, int destino, double peso)
{
    inserir_arco(g, origem, destino, peso, ARESTA_NORMAL);
}

/* Aresta de projecao semantica T->T (peso = similaridade cosseno). */
void adicionar_arco_projecao(Digrafo *g, int origem, int destino, double peso)
{
    inserir_arco(g, origem, destino, peso, ARESTA_PROJECAO);
}

void liberar_digrafo(Digrafo *g)
{
    for (int i = 0; i < g->numVertices; i++)
    {
        Arco *a = g->adj[i];
        while (a)
        {
            Arco *p = a->prox;
            free(a);
            a = p;
        }
        g->adj[i] = NULL;
    }
}

/* Textos que apontam para o genero 'gid'. Ignora arestas de projecao. */
int textos_do_genero(const Digrafo *g, int gid, ArcInverso *resultado)
{
    int count = 0;
    for (int i = 0; i < g->numVertices; i++)
    {
        if (g->vertices[i].tipo != TEXTO)
            continue;
        for (Arco *a = g->adj[i]; a != NULL; a = a->prox)
        {
            if (a->tipo == ARESTA_PROJECAO)
                continue;
            if (a->destino == gid)
            {
                resultado[count].origem = i;
                resultado[count].peso = a->peso;
                count++;
            }
        }
    }
    return count;
}
