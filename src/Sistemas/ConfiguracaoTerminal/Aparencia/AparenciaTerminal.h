#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include <functional>

class AparenciaTerminal {
public:
    // --- CORES DA PALETA C++ ---
    static std::string corPalavraChave();
    static std::string corTipo();
    static std::string corString();
    static std::string corNumero();
    static std::string corComentario();
    static std::string corFuncao();
    static std::string corVariavel();
    static std::string corOperador();
    static std::string corReset();

    // --- FUNCOES DE BARRA DE VIDA COM CARACTERES ASCII ---
    static std::string gerarBarraVidaASCII(double pct, int tamanho);
    static std::string gerarBarraVidaIDE(double pct, int tamanho);

    // --- ESTILIZACAO DE TEXTO EM ESTILO CODIGO ---
    static std::string estilizarComoCodigo(const std::string& texto, const std::string& tipo = "string");
    static std::string estilizarComoTipo(const std::string& texto);
    static std::string estilizarComoFuncao(const std::string& texto);
    static std::string estilizarComoComentario(const std::string& texto);
    static std::string estilizarComoNumero(const std::string& texto);

    // --- CONSTRUCAO DE CAIXAS EM ESTILO CODIGO ---
    static std::vector<std::string> criarCaixaCodigo(const std::vector<std::string>& conteudo, const std::string& titulo = "Info");
    static std::vector<std::string> criarCaixaDrop(const std::vector<std::string>& drops);

    // --- IMPRESSOES ESPECIFICAS DO TERMINAL ---
    static void imprimirTituloComoCodigo(const std::string& titulo);
    static void imprimirStatsComoStruct(const std::vector<std::pair<std::string, std::string>>& campos);
    static void imprimirLogsCombate(const std::vector<std::string>& mensagens);
    static void imprimirCaixaDrops(const std::vector<std::string>& drops);

    // --- UTILIDADES DE FORMATAO ---
    static std::string formatarVarNome(const std::string& nome);
    static std::string formatarTipo(const std::string& tipo);
    static std::string formatarValor(const std::string& valor);
    static std::string formatarOperacao(const std::string& operacao);
};
