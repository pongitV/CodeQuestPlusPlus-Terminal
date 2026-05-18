#include "TelaClasses.h"
#include "TelaClassesLayouts.h"
#include "../../Sistemas/SistemaPersonagem.h"
#include "../../Utilidades/Aparencia.h"
#include "../../Utilidades/ControleDeInput.h"
#include <iostream>
#include <vector>

void TelaClasses::exibir(SistemaPersonagem* jogador) {
    bool continuar = true;
    std::vector<std::string> classesJogaveis = {"Arqueiro", "Bardo", "Guerreiro", "Mago"};
    
    do {
        Aparencia::limparTela();
        Aparencia::exibirLogoAscii(ArtesClasses::logoClasses, 86, Cor::ROXO, "LINHAGENS DE BATALHA", false);
        std::cout << "\n";
        Aparencia::imprimirCentralizado("Selecione uma classe para ver seus detalhes:");
        std::cout << "\n";

        std::vector<std::string> menuOpcoes = classesJogaveis;
        menuOpcoes.push_back("VOLTAR");

        int escolha = ControleDeInput::lerSelecaoMenuComSetas(menuOpcoes, true);

        if (escolha == static_cast<int>(menuOpcoes.size()) - 1) {
            continuar = false;
        } else if (escolha >= 0 && escolha < static_cast<int>(classesJogaveis.size())) {
            exibirFichaClasse(jogador, classesJogaveis[escolha]);
        }
    } while (continuar);
}

void TelaClasses::exibirFichaClasse(SistemaPersonagem* jogador, const std::string& nomeClasse) {
    Aparencia::limparTela();
    Aparencia::exibirLogoAscii(ArtesClasses::logoClasses, 86, Cor::ROXO, "FICHA DA CLASSE");
    std::cout << "\n";
    Aparencia::imprimirCentralizado("Classe: " + nomeClasse);
    std::cout << "\n\n";
    
    std::vector<std::string> info;
    if (nomeClasse == "Arqueiro") {
        info = {
            "Os Arqueiros sao mestres do combate a distancia e da precisao.",
            "Eles dependem da Destreza para causar dano massivo e agir rapidamente,",
            "possuindo habilidades perfurantes para quebrar defesas."
        };
    } else if (nomeClasse == "Bardo") {
        info = {
            "Os Bardos sao curandeiros e suportes excepcionais.",
            "Eles usam Sabedoria e Inteligencia para fortalecer a si mesmos e aos",
            "aliados, tornando-se o pilar de sobrevivencia em batalhas dificeis."
        };
    } else if (nomeClasse == "Guerreiro") {
        info = {
            "Os Guerreiros sao a linha de frente de qualquer batalha.",
            "Guiados pela Forca e Resistencia, eles causam muito dano corporal e",
            "aguentam golpes pesados, sendo perfeitos para combates brutais."
        };
    } else if (nomeClasse == "Mago") {
        info = {
            "Os Magos manipulam as forcas arcanas para causar devastacao.",
            "Focados em Inteligencia, suas magias causam dano alto e frequente-",
            "mente ignoram defesas fisicas, embora sejam frageis fisicamente."
        };
    }
    
    Aparencia::imprimirBlocoCentralizado(info);
    std::cout << "\n";
    ControleDeInput::aguardarEnter();
}
