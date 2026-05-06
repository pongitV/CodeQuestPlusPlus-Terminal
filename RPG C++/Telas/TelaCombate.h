#pragma once

#include <string>
#include <vector>

class SistemaPersonagem;

class TelaCombate 
{
public:
    static void exibirLogoParaTelaDeCombate(const std::string& tituloDaTela = "");
    static void exibirBarraDeStatusDoJogador(SistemaPersonagem* jogadorAtual);
    static void exibirHordaDeInimigosLadoALado(const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* alvoAnimacao = nullptr, int frameAnimacao = 0, bool isCura = false);
    static std::vector<std::string> comporEstatisticasBatalha(SistemaPersonagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate);
    static void animarDanoNoInimigo(const std::string& tituloCombate, const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* alvoAnimacao, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& listaDeAliados = {});
    static void animarCuraNoInimigo(const std::string& tituloCombate, const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* alvoAnimacao, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& listaDeAliados = {});
};
