#include "NPCAparencia.h"
#include "NPCAparenciaLayout.h"
#include "../../../Core/Controladores/MenuJogo.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../../Sistemas/Progresso/Progressao.h"
#include <iostream>

void NPCAparencia::interagir(Personagem* jogador) {
    ControleDeInput::executarLoopMenuPopup(
        [this, jogador]() { this->exibirDialogo(jogador); },
        [this, jogador]() { return this->obterOpcoesMenu(jogador, Aparencia::obterLarguraTerminal()); },
        [this, jogador](const std::string& op) { this->processarOpcao(jogador, op, Aparencia::obterLarguraTerminal()); return true; },
        obterNomeDoLugar(), obterCorDoCabecalho(), obterArteASCII()
    );
}

std::string NPCAparencia::obterNomeDoLugar() const {
    return "SALA DE CUSTOMIZACAO";
}

Cor NPCAparencia::obterCorDoCabecalho() const {
    return Cor::MARROM_CLARO;
}

Cor NPCAparencia::obterCorDaArte() const {
    return Cor::MARROM_CLARO;
}

const std::vector<std::string>& NPCAparencia::obterArteASCII() const {
    return NPCAparenciaLayouts::arteAparencia;
}

void NPCAparencia::exibirDialogo(Personagem* jogador) {
    std::vector<std::string> linhas = {
        "Saudacoes, viajante! Eu sou Anok.",
        "Deseja renovar seu estilo?",
        "Aqui voce pode comprar novos icones de exibicao para o mapa",
        "e novas cores de fundo do terminal para deixar sua jornada unica!"
    };
    Aparencia::exibirPopup("ANOK - ESTILISTA REAL", linhas, Cor::MARROM_CLARO, obterArteASCII());
}

std::vector<std::string> NPCAparencia::obterOpcoesMenu(Personagem* jogador, int /*larguraDoTerminal*/) {
    return {
        "Comprar Icones",
        "Comprar Cores de Fundo",
        "Mudar Aparencia Atual",
        "Voltar"
    };
}

void NPCAparencia::processarOpcao(Personagem* jogador, const std::string& opcao, int larguraDoTerminal) {
    auto& progresso = Progressao::instancia();

    if (opcao == "Comprar Icones") {
        std::vector<std::pair<std::string, std::pair<char, int>>> iconesLoja = {
            {"Coracao (â™¥)", {'H', 100}}, // Usando caractere comum H ou simbolo se o terminal suportar. Para seguranca de UTF-8, usamos caracteres visiveis elegantes
            {"Estrela (*)", {'S', 150}},
            {"Espadas (X)", {'X', 200}},
            {"Coroa (K)", {'K', 400}},
            {"Cifrao ($)", {'$', 300}}
        };

        std::vector<std::string> opcoesItem;
        std::vector<std::pair<std::string, std::pair<char, int>>> disponiveis;

        for (const auto& item : iconesLoja) {
            std::string flag = "Aparencia_Icone_" + std::string(1, item.second.first);
            if (!progresso.obterFlag(flag)) {
                opcoesItem.push_back(item.first + " - " + std::to_string(item.second.second) + "G");
                disponiveis.push_back(item);
            }
        }

        if (opcoesItem.empty()) {
            Aparencia::exibirPopup("ANOK", {"Voce ja comprou todos os icones disponiveis!"}, Cor::MARROM_CLARO, obterArteASCII());
            return;
        }

        opcoesItem.push_back("Voltar");

        int escolha = ControleDeInput::lerSelecaoMenuEmPopup(
            "COMPRAR ICONES",
            {"Seu Ouro: " + std::to_string(jogador->obterInventario()->obterOuro()) + "G", "Selecione um icone para comprar:"},
            opcoesItem, Cor::MARROM_CLARO, obterArteASCII()
        );

        if (escolha >= 0 && escolha < (int)disponiveis.size()) {
            auto selecionado = disponiveis[escolha];
            if (jogador->obterInventario()->obterOuro() >= selecionado.second.second) {
                jogador->obterInventario()->adicionarOuro(-selecionado.second.second);
                std::string flag = "Aparencia_Icone_" + std::string(1, selecionado.second.first);
                progresso.definirFlag(flag, true);
                
                Aparencia::exibirPopup("COMPRA REALIZADA", {"Voce comprou o icone: " + selecionado.first}, Cor::MARROM_CLARO, obterArteASCII());
            } else {
                Aparencia::exibirPopup("OURO INSUFICIENTE", {"Voce nao tem ouro suficiente para esta compra!"}, Cor::VERMELHO, obterArteASCII());
            }
        }
    }
    else if (opcao == "Comprar Cores de Fundo") {
        std::vector<std::pair<std::string, std::pair<Cor, int>>> coresLoja = {
            {"Fundo Azul", {Cor::FUNDO_AZUL, 150}},
            {"Fundo Verde", {Cor::FUNDO_VERDE, 150}},
            {"Fundo Vermelho", {Cor::FUNDO_VERMELHO, 200}},
            {"Fundo Magenta", {Cor::FUNDO_MAGENTA, 250}},
            {"Fundo Ciano", {Cor::FUNDO_CIANO, 250}}
        };

        std::vector<std::string> opcoesItem;
        std::vector<std::pair<std::string, std::pair<Cor, int>>> disponiveis;

        for (const auto& item : coresLoja) {
            std::string flag = "Aparencia_Fundo_" + std::to_string(static_cast<uint32_t>(item.second.first));
            if (!progresso.obterFlag(flag)) {
                opcoesItem.push_back(item.first + " - " + std::to_string(item.second.second) + "G");
                disponiveis.push_back(item);
            }
        }

        if (opcoesItem.empty()) {
            Aparencia::exibirPopup("ANOK", {"Voce ja comprou todas as cores disponiveis!"}, Cor::MARROM_CLARO, obterArteASCII());
            return;
        }

        opcoesItem.push_back("Voltar");

        int escolha = ControleDeInput::lerSelecaoMenuEmPopup(
            "COMPRAR CORES DE FUNDO",
            {"Seu Ouro: " + std::to_string(jogador->obterInventario()->obterOuro()) + "G", "Selecione uma cor para comprar:"},
            opcoesItem, Cor::MARROM_CLARO, obterArteASCII()
        );

        if (escolha >= 0 && escolha < (int)disponiveis.size()) {
            auto selecionado = disponiveis[escolha];
            if (jogador->obterInventario()->obterOuro() >= selecionado.second.second) {
                jogador->obterInventario()->adicionarOuro(-selecionado.second.second);
                std::string flag = "Aparencia_Fundo_" + std::to_string(static_cast<uint32_t>(selecionado.second.first));
                progresso.definirFlag(flag, true);
                
                Aparencia::exibirPopup("COMPRA REALIZADA", {"Voce comprou a cor de fundo: " + selecionado.first}, Cor::MARROM_CLARO, obterArteASCII());
            } else {
                Aparencia::exibirPopup("OURO INSUFICIENTE", {"Voce nao tem ouro suficiente para esta compra!"}, Cor::VERMELHO, obterArteASCII());
            }
        }
    }
    else if (opcao == "Mudar Aparencia Atual") {
        std::vector<std::string> subOpcoes = {"Mudar Icone do Jogador", "Mudar Cor de Fundo do Terminal", "Voltar"};
        int subEscolha = ControleDeInput::lerSelecaoMenuEmPopup(
            "MUDAR APARENCIA",
            {"Escolha o que deseja customizar:"},
            subOpcoes, Cor::MARROM_CLARO, obterArteASCII()
        );

        if (subEscolha == 0) {
            // Icone
            std::vector<std::pair<std::string, char>> iconesDisponiveis = {
                {"Icone Padrao (@)", '@'}
            };

            std::vector<std::pair<std::string, char>> iconesLoja = {
                {"Coracao (â™¥)", 'H'},
                {"Estrela (*)", 'S'},
                {"Espadas (X)", 'X'},
                {"Coroa (K)", 'K'},
                {"Cifrao ($)", '$'}
            };

            for (const auto& item : iconesLoja) {
                std::string flag = "Aparencia_Icone_" + std::string(1, item.second);
                if (progresso.obterFlag(flag)) {
                    iconesDisponiveis.push_back(item);
                }
            }

            std::vector<std::string> opcoesMenu;
            for (const auto& ic : iconesDisponiveis) {
                std::string status = (Aparencia::iconeJogadorPersonalizado == ic.second) ? " (Equipado)" : "";
                opcoesMenu.push_back(ic.first + status);
            }
            opcoesMenu.push_back("Voltar");

            int escolha = ControleDeInput::lerSelecaoMenuEmPopup(
                "SELECIONAR ICONE",
                {"Selecione o icone de exibicao no mapa:"},
                opcoesMenu, Cor::MARROM_CLARO, obterArteASCII()
            );

            if (escolha >= 0 && escolha < (int)iconesDisponiveis.size()) {
                Aparencia::iconeJogadorPersonalizado = iconesDisponiveis[escolha].second;
                Aparencia::exibirPopup("ICONE ALTERADO", {"Icone alterado com sucesso para: " + iconesDisponiveis[escolha].first}, Cor::MARROM_CLARO, obterArteASCII());
            }
        }
        else if (subEscolha == 1) {
            // Cor de Fundo
            std::vector<std::pair<std::string, Cor>> coresDisponiveis = {
                {"Fundo Padrao (Preto)", Cor::RESET}
            };

            std::vector<std::pair<std::string, Cor>> coresLoja = {
                {"Fundo Azul", Cor::FUNDO_AZUL},
                {"Fundo Verde", Cor::FUNDO_VERDE},
                {"Fundo Vermelho", Cor::FUNDO_VERMELHO},
                {"Fundo Magenta", Cor::FUNDO_MAGENTA},
                {"Fundo Ciano", Cor::FUNDO_CIANO}
            };

            for (const auto& item : coresLoja) {
                std::string flag = "Aparencia_Fundo_" + std::to_string(static_cast<uint32_t>(item.second));
                if (progresso.obterFlag(flag)) {
                    coresDisponiveis.push_back(item);
                }
            }

            std::vector<std::string> opcoesMenu;
            for (const auto& c : coresDisponiveis) {
                std::string status = (Aparencia::corFundoAtiva == c.second) ? " (Equipado)" : "";
                opcoesMenu.push_back(c.first + status);
            }
            opcoesMenu.push_back("Voltar");

            int escolha = ControleDeInput::lerSelecaoMenuEmPopup(
                "SELECIONAR COR DE FUNDO",
                {"Selecione a cor de fundo do terminal:"},
                opcoesMenu, Cor::MARROM_CLARO, obterArteASCII()
            );

            if (escolha >= 0 && escolha < (int)coresDisponiveis.size()) {
                Aparencia::corFundoAtiva = coresDisponiveis[escolha].second;
                Aparencia::limparTela(); // Limpa e redesenha a tela inteira para aplicar a cor de fundo
                Aparencia::exibirPopup("COR ALTERADA", {"Cor de fundo alterada com sucesso!"}, Cor::MARROM_CLARO, obterArteASCII());
            }
        }
    }
}

