#include "TelaAtributos.h"
#include "../../GerenciadorPerspectiva.h"
#include "../../PerspectivaIDE/TelasIDE/Atributos/TelaAtributosIDE.h"
#include "../TelaBase.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../../Core/Utilidades/FuncoesDialogo.h"
#include "../../../Sistemas/Inventario/Item.h"

PoderCombate TelaAtributos::calcularPoderCombate(Personagem* jogadorAtual, double multiplicador) {
    int danoFis = 1, danoMag = 0;
    if (jogadorAtual->obterArma()) {
        danoFis = jogadorAtual->obterArma()->obterDanoFisico();
        danoMag = jogadorAtual->obterArma()->obterDanoMagico();
    }
    int forca = jogadorAtual->obterForca();
    int destreza = jogadorAtual->obterDestreza();
    int inteli = jogadorAtual->obterInteligencia();
    int sabedoria = jogadorAtual->obterSabedoria();

    if (danoFis == 0 && danoMag > 0) { forca /= 10; destreza /= 10; }
    else if (danoFis > 0 && danoMag == 0) { inteli /= 10; sabedoria /= 10; }

    PoderCombate p;
    p.danoFisEst = std::max(0, static_cast<int>((danoFis + forca) * (1.0 + (destreza / 100.0)) * multiplicador));
    p.danoMagEst = std::max(0, static_cast<int>((danoMag + inteli) * (1.0 + (sabedoria / 100.0)) * multiplicador));
    p.defFixa = jogadorAtual->obterResistencia();
    p.mitigacao = std::min(50.0, jogadorAtual->obterConstituicao() / 2.0);
    return p;
}

DebuffInfo TelaAtributos::calcularDebuff(Personagem* jogadorAtual) {
    DebuffInfo d;
    d.temBuff = (jogadorAtual->obterTurnosEfeito(EfeitoID::BuffAtributos) > 0 &&
                 jogadorAtual->obterMultiplicador() > 1.0);
    d.forcaPerdida    = jogadorAtual->possuiEfeito(EfeitoID::Fraqueza)     ? (jogadorAtual->obterForca() / 3)      : 0;
    d.destrezaPerdida = jogadorAtual->possuiEfeito(EfeitoID::Lentidao)     ? jogadorAtual->obterDestreza()       : 0;
    d.resPerdida      = jogadorAtual->possuiEfeito(EfeitoID::QuebraResistencia) ? jogadorAtual->obterResistencia() : 0;
    d.constPerdida    = jogadorAtual->possuiEfeito(EfeitoID::QuebraResistencia) ? (jogadorAtual->obterConstituicao() / 2) : 0;
    return d;
}

void TelaAtributos::exibir(Personagem* jogadorAtual)
{
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        GerenciadorPerspectiva::obterAtributosUI().exibir(jogadorAtual);
    } else {
        TelaAtributosIDE::exibir(jogadorAtual);
    }
}

void TelaAtributos::gerenciarFichaDoJogador(Personagem* jogadorAtual)
{
    if (jogadorAtual == nullptr) return;

    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        GerenciadorPerspectiva::obterAtributosUI().gerenciarFichaDoJogador(jogadorAtual);
        return;
    }

    TelaBase::executarLoop(
        nullptr,
        [jogadorAtual]() {
            TelaAtributos::exibir(jogadorAtual);
        },
        []() {
            return std::vector<std::string>{ "SUBIR DE NIVEL", "DETALHES DE ATRIBUTOS", "VOLTAR" };
        },
        [jogadorAtual](int selecao) {
            if (selecao == 0) {
                if (!jogadorAtual->podeSubirDeNivel()) {
                    Aparencia::exibirPrompt(FuncoesDialogo::formatarMsgSistema("Voce nao tem XP suficiente para subir de nivel!", Cor::AMARELO));
                    ControleDeInput::aguardarEnter();
                } else {
                    std::vector<std::string> opcoesAtr;
                    std::string nomesAtr[] = {"Vida", "Forca", "Destreza", "Resistencia", "Constituicao", "Inteligencia", "Sabedoria"};

                    for (int i = 1; i <= 7; ++i) {
                        auto clonePreview = jogadorAtual->clone();
                        clonePreview->subirDeNivel(static_cast<TipoAtributo>(i));

                        int valAtual = 0, valNovo = 0;
                        switch (i) {
                            case 1: valAtual = jogadorAtual->obterVidaMaxima(); valNovo = clonePreview->obterVidaMaxima(); break;
                            case 2: valAtual = jogadorAtual->obterForca(); valNovo = clonePreview->obterForca(); break;
                            case 3: valAtual = jogadorAtual->obterDestreza(); valNovo = clonePreview->obterDestreza(); break;
                            case 4: valAtual = jogadorAtual->obterResistencia(); valNovo = clonePreview->obterResistencia(); break;
                            case 5: valAtual = jogadorAtual->obterConstituicao(); valNovo = clonePreview->obterConstituicao(); break;
                            case 6: valAtual = jogadorAtual->obterInteligencia(); valNovo = clonePreview->obterInteligencia(); break;
                            case 7: valAtual = jogadorAtual->obterSabedoria(); valNovo = clonePreview->obterSabedoria(); break;
                        }

                        int ganho = valNovo - valAtual;
                        opcoesAtr.push_back(nomesAtr[i - 1] + " " + Aparencia::cor(Cor::CINZA) + "(" + std::to_string(valAtual) + " -> " + std::to_string(valNovo) + " [" + Aparencia::cor(Cor::VERDE) + "+" + std::to_string(ganho) + Aparencia::cor(Cor::CINZA) + "])" + Aparencia::cor(Cor::RESET));
                    }
                    opcoesAtr.push_back("Cancelar");

                    std::cout << "\n";
                    Aparencia::imprimirCentralizado("Escolha o atributo para melhorar:");
                    std::cout << "\n";
                    int escolhaAtr = ControleDeInput::lerSelecaoMenuComSetas(opcoesAtr, true);

                    if (escolhaAtr >= 0 && escolhaAtr <= 6) {
                        TipoAtributo atributo = static_cast<TipoAtributo>(escolhaAtr + 1);
                        if (jogadorAtual->subirDeNivel(atributo)) {
                            Aparencia::exibirPrompt(FuncoesDialogo::formatarMsgSistema("Nivel subiu! Atributo melhorado.", Cor::VERDE));
                            ControleDeInput::aguardarEnter();
                        }
                    }
                }
            } else if (selecao == 1) {
                if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
                    GerenciadorPerspectiva::obterAtributosUI().exibirDetalhesAtributos(jogadorAtual);
                } else {
                    TelaAtributosIDE::exibirDetalhesAtributos(jogadorAtual);
                }
            } else if (selecao == 2 || selecao == -1) {
                return false;
            }
            return true;
        }
    );
}
