#include <iostream>
#include <map>

#include "TelaVitoria.h"
#include "../Racas/RacaBase.h"
#include "TelaBase.h"
#include "../Utilidades/Aparencia.h"
#include <sstream>
#include <thread>
#include <chrono>

void TelaVitoria::exibir(SistemaPersonagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate, const std::vector<std::string>& itensObtidos)
{
    Aparencia::limparTela();
    int frames = 20;

    std::vector<std::string> logoVitoria = 
    {
       " █████   █████ █████ ███████████    ███████    ███████████   █████   █████████   ███ ",
       "░░███   ░░███ ░░███ ░█░░░███░░░█  ███░░░░░███ ░░███░░░░░███ ░░███   ███░░░░░███ ░███ ",
       " ░███    ░███  ░███ ░   ░███  ░  ███     ░░███ ░███    ░███  ░███  ░███    ░███ ░███ ",
       " ░███    ░███  ░███     ░███    ░███      ░███ ░██████████   ░███  ░███████████ ░███ ",
       " ░░███   ███   ░███     ░███    ░███      ░███ ░███░░░░░███  ░███  ░███░░░░░███ ░███ ",
       "  ░░░█████░    ░███     ░███    ░░███     ███  ░███    ░███  ░███  ░███    ░███ ░░░  ",
       "    ░░███      █████    █████    ░░░███████░   █████   █████ █████ █████   █████ ███ ",
       "     ░░░      ░░░░░    ░░░░░       ░░░░░░░    ░░░░░   ░░░░░ ░░░░░ ░░░░░   ░░░░░ ░░░  "
    };

    for (int frame = 0; frame <= frames; ++frame) {
        int curOuro = (quantidadeDeOuroObtido * frame) / frames;
        int curXp = (quantidadeDeXpObtido * frame) / frames;

        std::ostringstream buffer;
        std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

        Aparencia::exibirLogoAscii(logoVitoria, 85, Cor::VERDE, "", 0);

        std::vector<std::string> estLinhas;
        estLinhas.push_back("");
        estLinhas.push_back("Turnos         : " + std::to_string(turnosCombate));
        estLinhas.push_back("Dano Causado   : " + std::to_string(totalDeDanoCausado));
        estLinhas.push_back("Dano Recebido  : " + std::to_string(totalDeDanoRecebido));
        estLinhas.push_back("Cura Realizada : " + std::to_string(curaTotalRecebida));
        estLinhas.push_back("");
        std::vector<std::string> caixaEst = Aparencia::criarCaixa(estLinhas, "ESTATISTICAS DA BATALHA", 35, Cor::VERDE);

        std::vector<std::string> lootLinhas;
        lootLinhas.push_back(frame == frames ? "Recompensas:" : "Calculando recompensas...");
        lootLinhas.push_back("");
        lootLinhas.push_back(" + " + std::to_string(curXp) + " XP");
        lootLinhas.push_back(" + " + std::to_string(curOuro) + " G");
        lootLinhas.push_back("");
        lootLinhas.push_back("Itens:");
        if (itensObtidos.empty()) {
            lootLinhas.push_back(" > Nenhum item dropado.");
        } else {
            if (frame == frames) {
                std::map<std::string, int> contagem;
                for (const std::string& item : itensObtidos) contagem[item]++;
                for (auto const& [nome, qtd] : contagem) lootLinhas.push_back(" > " + std::to_string(qtd) + "x " + nome);
            } else {
                lootLinhas.push_back(" > ???");
            }
        }
        std::vector<std::string> caixaLoot = Aparencia::criarCaixa(lootLinhas, "SAQUE (LOOT)", 35, Cor::AMARELO);

        Aparencia::imprimirLadoALado(caixaEst, caixaLoot, 39, 4);

        if (frame == frames && jogadorAtual->podeSubirDeNivel()) {
            std::cout << "\n";
            Aparencia::imprimirCentralizado("*** VOCE PODE SUBIR DE NIVEL! ***", Aparencia::cor(Cor::MAGENTA));
            std::cout << "\n";
        } else {
            std::cout << "\n\n\n";
        }

        std::cout.rdbuf(oldCout);
        std::cout << "\033[H" << buffer.str() << "\033[J" << std::flush;
        
        if (frame < frames) {
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
        }
    }

    Aparencia::aguardarEnter();
}