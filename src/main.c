/* ============================================================
   SISTEMA DE RECOMENDACAO EM GRAFOS — EDA2
   Implementacao em C (C99) com pipeline algoritmico completo
   + Fase 0 de PLN: TF-IDF, projecao texto-texto por cosseno
   ------------------------------------------------------------
   Compilar:  make
   Executar:  ./recomendador
   ============================================================ */

#include "digrafo.h"
#include "vocab.h"
#include "algoritmos.h"
#include "recomendador.h"
#include "relatorio.h"

int main(void)
{
   Digrafo g;
   inicializar_digrafo(&g);

   /* ── Vertices ── */

   /* ── Usuarios ── */
   int u1 = adicionar_vertice(&g, "u1", USUARIO); /* Joao */
   int u2 = adicionar_vertice(&g, "u2", USUARIO); /* Maria */
   int u3 = adicionar_vertice(&g, "u3", USUARIO); /* Pedro */
   int u4 = adicionar_vertice(&g, "u4", USUARIO); /* Thiago */
   int u5 = adicionar_vertice(&g, "u5", USUARIO); /* Ana */
   int u6 = adicionar_vertice(&g, "u6", USUARIO); /* Carlos */
   int u7 = adicionar_vertice(&g, "u7", USUARIO); /* Larissa */
   int u8 = adicionar_vertice(&g, "u8", USUARIO); /* Rafael */
   int u9 = adicionar_vertice(&g, "u9", USUARIO); /* Marjorie */

   /* ── Livros ── */
   int t1 = adicionar_vertice(&g, "t1", TEXTO); /* Dom Casmurro */
   int t2 = adicionar_vertice(&g, "t2", TEXTO); /* Memorias Postumas de Bras Cubas */
   int t3 = adicionar_vertice(&g, "t3", TEXTO); /* Romeu e Julieta */
   int t4 = adicionar_vertice(&g, "t4", TEXTO); /* A Ilha do Tesouro */
   int t5 = adicionar_vertice(&g, "t5", TEXTO); /* O Conde de Monte Cristo */
   int t6 = adicionar_vertice(&g, "t6", TEXTO); /* Percy Jackson */
   int t7 = adicionar_vertice(&g, "t7", TEXTO); /* O Hobbit */
   int t8 = adicionar_vertice(&g, "t8", TEXTO); /* O Codigo Da Vinci */
   int t9  = adicionar_vertice(&g, "t9",  TEXTO); /* O Senhor dos Aneis */
   int t10 = adicionar_vertice(&g, "t10", TEXTO); /* Orgulho e Preconceito */
   int t11 = adicionar_vertice(&g, "t11", TEXTO); /* A Garota no Trem */
   int t12 = adicionar_vertice(&g, "t12", TEXTO); /* As Cronicas de Narnia */
   int t13 = adicionar_vertice(&g, "t13", TEXTO); /* O Apanhador no Campo de Centeio */
   int t14 = adicionar_vertice(&g, "t14", TEXTO); /* Garota Exemplar */
   int t15 = adicionar_vertice(&g, "t15", TEXTO); /* Moby Dick */
   int t16 = adicionar_vertice(&g, "t16", TEXTO); /* Jane Eyre */
   int t17 = adicionar_vertice(&g, "t17", TEXTO); /* O Iluminado */
   int t18 = adicionar_vertice(&g, "t18", TEXTO); /* Robinson Crusoe */
   int t19 = adicionar_vertice(&g, "t19", TEXTO); /* Wuthering Heights */
   int t20 = adicionar_vertice(&g, "t20", TEXTO); /* O Silencio dos Inocentes */
   int t21 = adicionar_vertice(&g, "t21", TEXTO); /* O Nome da Rosa */
   int t22 = adicionar_vertice(&g, "t22", TEXTO); /* Duna */
   int t23 = adicionar_vertice(&g, "t23", TEXTO); /* Orgulho e Paixao */

   /* ── Generos ── */
   int fi = adicionar_vertice(&g, "ficcao", GENERO);
   int av = adicionar_vertice(&g, "aventura", GENERO);
   int ro = adicionar_vertice(&g, "romance", GENERO);
   int th = adicionar_vertice(&g, "thriller", GENERO);

   /* ── Conteudo textual (DADOS SINTETICOS) ── */
   definir_conteudo(&g, t1, "amor e paixao definem esta historia de ciume e traicao. o ciume corroi o coracao e a traicao leva o amor a tragedia. o destino do casal termina em morte e sofrimento. uma paixao marcada pelo ciume pela duvida e pela dor do amor traido.");
   definir_conteudo(&g, t2, "um amor proibido e uma paixao adultera conduzem a traicao. o ciume e a culpa cercam o amor enquanto a morte se aproxima. a tragedia revela o destino e o sofrimento de um coracao dividido entre o amor e a perda. paixao traicao e morte.");
   definir_conteudo(&g, t3, "o amor proibido e a paixao ardente terminam em tragedia. a traicao do destino separa o casal e o ciume alimenta o odio. o amor verdadeiro encontra a morte e o coracao mergulha no sofrimento. uma paixao eterna desfeita pela tragedia e pela morte.");
   definir_conteudo(&g, t4, "uma aventura pelo mar em busca de um tesouro perdido. a viagem enfrenta o perigo a batalha e a coragem. o heroi vence o inimigo supera o perigo e conquista o tesouro. aventura coragem e perigo no mar em uma viagem de batalha.");
   definir_conteudo(&g, t5, "uma aventura de vinganca nascida da traicao e do amor perdido. a paixao e o ciume movem um coracao ferido enquanto a busca por um tesouro leva o heroi ao mar. vinganca traicao e amor caminham juntos nesta aventura de coragem e destino.");
   definir_conteudo(&g, t6, "uma aventura epica onde um jovem heroi enfrenta o perigo e a batalha. a coragem guia a viagem pelo mar contra o inimigo. cada perigo exige coragem e o heroi parte em busca do destino enfrentando a batalha. aventura coragem e perigo no mar.");
   definir_conteudo(&g, t7, "uma aventura pelo mar e pelas montanhas em busca de um tesouro guardado. a viagem enfrenta o perigo a batalha e a coragem do heroi. o inimigo protege o tesouro mas a coragem vence o perigo. aventura tesouro e coragem em uma viagem de batalha.");
   definir_conteudo(&g, t8, "um thriller de traicao e vinganca onde o perigo ronda cada passo. a morte e o inimigo cercam a busca enquanto o destino se revela. a traicao move o coracao e a vinganca leva a morte. perigo inimigo traicao e destino marcam este thriller.");
   definir_conteudo(&g, t9, "uma aventura perigosa leva o heroi pela floresta em busca de destruir um mal antigo. a coragem do grupo enfrenta o inimigo e o perigo a cada passo da viagem. a batalha final decide o destino de todos. aventura coragem perigo e batalha contra o inimigo.");
   definir_conteudo(&g, t10, "um amor que nasce do orgulho e do preconceito entre o casal. a paixao cresce lentamente enquanto o coracao supera a duvida e o ciume. o sofrimento do mal-entendido se desfaz e o amor verdadeiro vence. amor paixao coracao e casal.");
   definir_conteudo(&g, t11, "um thriller psicologico onde a traicao e o ciume escondem um crime perigoso. a protagonista busca a verdade enquanto o inimigo se esconde na propria vizinhanca. a morte de uma mulher revela segredos e destino. traicao perigo morte e inimigo.");
   definir_conteudo(&g, t12, "uma aventura magica leva as criancas a um mundo desconhecido e perigoso. a coragem do grupo enfrenta o inimigo em uma grande batalha pelo destino do reino. o heroi vence o perigo e a tragedia se transforma em esperanca. aventura coragem batalha e destino.");
   definir_conteudo(&g, t13, "a historia de um jovem perdido enfrenta a tragedia do proprio destino e o sofrimento da solidao. o coracao confuso busca sentido em meio a duvida e a dor. a ciume e a traicao dos outros marcam sua jornada. destino sofrimento coracao e tragedia.");
   definir_conteudo(&g, t14, "um thriller de traicao onde o casamento esconde uma vinganca calculada. o ciume e a manipulacao levam ao perigo e a morte. o inimigo verdadeiro estava dentro de casa e o destino do casal termina em tragedia. traicao vinganca perigo e morte.");
   definir_conteudo(&g, t15, "uma aventura obsessiva pelo mar leva o capitao a cacar uma criatura gigante. a vinganca move o heroi contra o inimigo em uma batalha perigosa. o destino da tripulacao se decide em meio ao perigo e a coragem. aventura mar perigo batalha e vinganca.");
   definir_conteudo(&g, t16, "um amor sincero nasce em meio a duvida e ao sofrimento de uma jovem orfa. a paixao enfrenta segredos e a traicao do passado do homem que ela ama. o coracao supera a tragedia e encontra o destino do amor verdadeiro. amor paixao coracao e sofrimento.");
   definir_conteudo(&g, t17, "um perigo sombrio cresce dentro de um hotel isolado durante o inverno. a mente do pai se corrompe e o inimigo se torna ele mesmo. a familia enfrenta a morte enquanto o destino se revela aos poucos. perigo morte inimigo e destino marcam este thriller.");
   definir_conteudo(&g, t18, "uma aventura de sobrevivencia apos um naufragio em uma ilha desconhecida. a coragem do heroi enfrenta o perigo sozinho por anos no mar. a batalha contra a natureza exige coragem e engenho a cada dia. aventura mar coragem perigo e batalha.");
   definir_conteudo(&g, t19, "uma paixao intensa e destrutiva une duas almas desde a infancia na montanha. o ciume e a traicao alimentam um amor que vira obsessao e tragedia. o coracao sofre ate a morte sem nunca esquecer o destino. paixao ciume traicao e tragedia.");
   definir_conteudo(&g, t20, "um thriller perturbador onde uma agente busca capturar um assassino perigoso. a traicao e a manipulacao de um inimigo brilhante guiam a investigacao da morte. o destino das vitimas depende da coragem da agente. traicao perigo morte e inimigo.");
   definir_conteudo(&g, t21, "historia de misterio e suspense com um crime sem solucao. o detetive busca a verdade e enfrenta o perigo e a morte escondida. traicao e destino revelam o inimigo.");
   definir_conteudo(&g, t22, "viagem pelo deserto de um planeta perigoso. a coragem do heroi e testada contra o inimigo numa grande batalha pelo destino. aventura coragem perigo e traicao marcam a jornada.");
   definir_conteudo(&g, t23, "romance de um heroi e uma jovem corajosa. amor paixao e sofrimento marcam o destino do casal. o coracao supera o ciume e a tragedia para encontrar a paz.");

   /* ── Perfis tematicos dos GENEROS (palavras-chave) ── */
   definir_conteudo(&g, fi, "amor tragedia destino ciume coracao");
   definir_conteudo(&g, av, "aventura mar tesouro viagem perigo coragem heroi batalha");
   definir_conteudo(&g, ro, "amor paixao coracao sofrimento casal");
   definir_conteudo(&g, th, "traicao vinganca morte perigo inimigo destino");

   /* ── Leituras: Usuario -> Texto (peso = nota) ── */
   adicionar_arco(&g, u1, t1, 5.0);
   adicionar_arco(&g, u1, t2, 4.0);
   adicionar_arco(&g, u1, t3, 2.0);
   adicionar_arco(&g, u2, t1, 3.0);
   adicionar_arco(&g, u2, t4, 5.0);
   adicionar_arco(&g, u3, t3, 4.0);
   adicionar_arco(&g, u3, t5, 5.0);
   adicionar_arco(&g, u4, t6, 5.0);
   adicionar_arco(&g, u4, t7, 4.0);
   adicionar_arco(&g, u4, t4, 3.0);
   adicionar_arco(&g, u5, t9, 5.0);
   adicionar_arco(&g, u5, t10, 4.0);
   adicionar_arco(&g, u5, t12, 3.0);
   adicionar_arco(&g, u6, t11, 5.0);
   adicionar_arco(&g, u6, t13, 4.0);
   adicionar_arco(&g, u6, t14, 4.0);
   adicionar_arco(&g, u7, t15, 5.0);
   adicionar_arco(&g, u7, t17, 4.0);
   adicionar_arco(&g, u7, t20, 4.0);
   adicionar_arco(&g, u8, t16, 5.0);
   adicionar_arco(&g, u8, t18, 4.0);
   adicionar_arco(&g, u8, t19, 3.0);
   adicionar_arco(&g, u9, t21, 4.5);
   adicionar_arco(&g, u9, t22, 5.0);
   adicionar_arco(&g, u9, t23, 3.5);

   /* ── FASE 0: PLN (TF-IDF) e projecao texto-texto ── */
   Vocabulario vocab;
   inicializar_vocab(&vocab);

   static int tf_cont[MAX_VERTICES][MAX_VOCAB];
   static double tfidf[MAX_VERTICES][MAX_VOCAB];
   static double cos_mat[MAX_VERTICES][MAX_VERTICES];
   int total_tokens[MAX_VERTICES];

   int num_docs = construir_vocab_e_contagens(&g, &vocab, tf_cont, total_tokens);
   calcular_tfidf(&g, &vocab, num_docs, tf_cont, total_tokens, tfidf);
   int arestas_genero = construir_ligacoes_genero(&g, &vocab, tfidf);
   construir_projecao_texto(&g, &vocab, tfidf, cos_mat);

   /* ── Relatorios e Recomendacoes ── */
   imprimir_relatorio_fase0(&g, &vocab, num_docs, arestas_genero, tfidf, cos_mat);

   /* ── Recomendacoes para u1 ── */
   Recomendacao resultado[MAX_VERTICES];
   int usou_fallback = 0;
   int n = recomendar(&g, u1, resultado, 5, cos_mat, &usou_fallback);

   imprimir_recomendacoes(&g, resultado, n, usou_fallback);

   /* Suprimir warnings de variaveis nao utilizadas nos dados sinteticos */
   (void)u2; (void)u3; (void)t2; (void)t3; (void)t4; (void)t5;
   (void)fi; (void)av; (void)ro; (void)th; (void)t6; (void)t7;
   (void)t8; (void)u5; (void)u6; (void)t9; (void)t10; (void)t11;
   (void)t12; (void)t13; (void)t14; (void)u7; (void)u8; (void)t15;
   (void)t16; (void)t17; (void)t18; (void)t19; (void)t20; 
   (void)u9; (void)t21; (void)t22; (void)t23;

   liberar_digrafo(&g);
   return 0;
}