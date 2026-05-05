#pragma once

#include <string>
#include <vector>

class SistemaPersonagem;

class TelaMenu {
public:
    static void exibirLogoDoJogo(const std::string& tituloDaTela = "");
    static bool exibirConfirmacaoDeEscolhaComArteLadoALado(const std::string& tipoDeEscolha, const std::string& nomeDaEscolha, const std::vector<std::string>& informacoesParaExibir, const std::vector<std::string>& arteAsciiParaExibir);
};
