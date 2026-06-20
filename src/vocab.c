#include "vocab.h"
#include <math.h>
#include <ctype.h>

/* Lista minima de stopwords em portugues (corpus em ASCII, sem acentos). */
static int eh_stopword(const char *t)
{
    static const char *stop[] = {
        "a", "o", "as", "os", "um", "uma", "uns", "umas", "de", "do", "da", "dos", "das",
        "e", "em", "no", "na", "nos", "nas", "que", "se", "por", "com", "para", "ao", "aos",
        "sua", "seu", "suas", "seus", "ele", "ela", "eles", "elas", "mais", "mas", "ou",
        "como", "foi", "era", "sao", "ser", "ter", "seu", "entre", "sobre", "ja", "sem",
        "the", "of", "and", "to", "in", NULL};
    for (int i = 0; stop[i] != NULL; i++)
        if (strcmp(t, stop[i]) == 0)
            return 1;
    return 0;
}

/* Funcao hash djb2 sobre a string do termo. */
static unsigned long hash_djb2(const char *s)
{
    unsigned long h = 5381;
    int c;
    while ((c = *s++))
        h = ((h << 5) + h) + (unsigned char)c;
    return h;
}

void inicializar_vocab(Vocabulario *v)
{
    v->tamanho = 0;
    for (int i = 0; i < HASH_SIZE; i++)
        v->bucket[i] = -1;
    for (int i = 0; i < MAX_VOCAB; i++)
        v->df[i] = 0;
}

/* Retorna o indice do termo no vocabulario, inserindo-o se for novo.
   Sondagem linear na tabela hash — O(1) amortizado.
   Retorna -1 (sem corromper memoria) se o vocabulario (MAX_VOCAB) ou a
   tabela hash (HASH_SIZE) estiverem cheios. */
int vocab_indice(Vocabulario *v, const char *termo)
{
    unsigned long inicio = hash_djb2(termo) % HASH_SIZE;
    unsigned long h = inicio;
    int sondagens = 0;
    while (v->bucket[h] != -1)
    {
        if (strcmp(v->termo[v->bucket[h]], termo) == 0)
            return v->bucket[h];
        h = (h + 1) % HASH_SIZE;
        if (++sondagens >= HASH_SIZE)
        {
            fprintf(stderr, "ERRO: tabela hash do vocabulario cheia (%d slots).\n",
                    HASH_SIZE);
            return -1;
        }
    }
    if (v->tamanho >= MAX_VOCAB)
    {
        fprintf(stderr, "ERRO: limite de %d termos no vocabulario excedido.\n",
                MAX_VOCAB);
        return -1;
    }
    int idx = v->tamanho;
    v->tamanho++;
    strncpy(v->termo[idx], termo, MAX_TERMO - 1);
    v->termo[idx][MAX_TERMO - 1] = '\0';
    v->bucket[h] = idx;
    return idx;
}

static void processar_token(Vocabulario *v, const char *token, int *tf_cont_doc, int *total_tokens_doc, int *visto)
{
    if (eh_stopword(token))
        return;
    int k = vocab_indice(v, token);
    if (k < 0)
        return;
    tf_cont_doc[k]++;
    (*total_tokens_doc)++;
    if (!visto[k])
    {
        visto[k] = 1;
        v->df[k]++;
    }
}

static void processar_documento(Vocabulario *v, const char *texto, int *tf_cont_doc, int *total_tokens_doc)
{
    int visto[MAX_VOCAB] = {0};
    char token[MAX_TERMO];
    int tlen = 0;
    int trunc = 0;

    for (int p = 0;; p++)
    {
        char c = texto[p];
        if (isalpha((unsigned char)c))
        {
            if (tlen < MAX_TERMO - 1)
                token[tlen++] = (char)tolower((unsigned char)c);
            else
                trunc = 1;
        }
        else
        {
            if (trunc)
                fprintf(stderr, "AVISO: token truncado em %d caracteres.\n", MAX_TERMO - 1);
            if (tlen > 1)
            {
                token[tlen] = '\0';
                processar_token(v, token, tf_cont_doc, total_tokens_doc, visto);
            }
            tlen = 0;
            trunc = 0;
            if (c == '\0')
                break;
        }
    }
}

int construir_vocab_e_contagens(const Digrafo *g, Vocabulario *v,
                                int tf_cont[][MAX_VOCAB],
                                int *total_tokens)
{
    int num_docs = 0;
    for (int i = 0; i < g->numVertices; i++)
    {
        for (int k = 0; k < MAX_VOCAB; k++)
            tf_cont[i][k] = 0;
        total_tokens[i] = 0;
    }

    for (int i = 0; i < g->numVertices; i++)
    {
        if (g->vertices[i].tipo != TEXTO && g->vertices[i].tipo != GENERO)
            continue;
        num_docs++;
        processar_documento(v, g->vertices[i].texto, tf_cont[i], &total_tokens[i]);
    }
    return num_docs;
}

void calcular_tfidf(const Digrafo *g, const Vocabulario *v, int num_docs,
                    int tf_cont[][MAX_VOCAB], const int *total_tokens,
                    double tfidf[][MAX_VOCAB])
{
    for (int i = 0; i < g->numVertices; i++)
        for (int k = 0; k < MAX_VOCAB; k++)
            tfidf[i][k] = 0.0;

    for (int i = 0; i < g->numVertices; i++)
    {
        if (total_tokens[i] == 0)
            continue;
        for (int k = 0; k < v->tamanho; k++)
        {
            if (tf_cont[i][k] == 0)
                continue;
            double tf = (double)tf_cont[i][k] / total_tokens[i];
            double idf = log((double)num_docs / (double)v->df[k]);
            tfidf[i][k] = tf * idf;
        }
        double norma = 0.0;
        for (int k = 0; k < v->tamanho; k++)
            norma += tfidf[i][k] * tfidf[i][k];
        norma = sqrt(norma);
        if (norma > 0.0)
            for (int k = 0; k < v->tamanho; k++)
                tfidf[i][k] /= norma;
    }
}

double cosseno(const double *va, const double *vb, int n)
{
    double dot = 0.0;
    for (int k = 0; k < n; k++)
        dot += va[k] * vb[k];
    if (dot < 0.0)
        dot = 0.0;
    if (dot > 1.0)
        dot = 1.0;
    return dot;
}

void construir_projecao_texto(Digrafo *g, const Vocabulario *v,
                              double tfidf[][MAX_VOCAB],
                              double cos_mat[][MAX_VERTICES])
{
    for (int i = 0; i < g->numVertices; i++)
        for (int j = 0; j < g->numVertices; j++)
            cos_mat[i][j] = 0.0;

    for (int i = 0; i < g->numVertices; i++)
    {
        if (g->vertices[i].tipo != TEXTO)
            continue;
        for (int j = i + 1; j < g->numVertices; j++)
        {
            if (g->vertices[j].tipo != TEXTO)
                continue;
            double sim = cosseno(tfidf[i], tfidf[j], v->tamanho);
            cos_mat[i][j] = cos_mat[j][i] = sim;
            if (sim > LIMIAR_THETA)
            {
                adicionar_arco_projecao(g, i, j, sim);
                adicionar_arco_projecao(g, j, i, sim);
            }
        }
    }
}

int construir_ligacoes_genero(Digrafo *g, const Vocabulario *v,
                              double tfidf[][MAX_VOCAB])
{
    int criadas = 0;
    for (int i = 0; i < g->numVertices; i++)
    {
        if (g->vertices[i].tipo != TEXTO)
            continue;
        for (int j = 0; j < g->numVertices; j++)
        {
            if (g->vertices[j].tipo != GENERO)
                continue;
            double sim = cosseno(tfidf[i], tfidf[j], v->tamanho);
            if (sim > LIMIAR_GENERO)
            {
                adicionar_arco(g, i, j, sim);
                criadas++;
            }
        }
    }
    return criadas;
}
