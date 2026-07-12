#include "MenuJogo.h"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif

#include "../../Entidades/Classes/FabricaClasses.h"
#include "../../Entidades/Racas/FabricaRacas.h"
#include "CriadorInimigos.h"
#include "../../Sistemas/Inventario/Equipamentos/EquipamentoArma.h"
#include "../../Sistemas/Inventario/Equipamentos/EquipamentoArmadura.h"
#include "../../Sistemas/Inventario/Equipamentos/EquipamentoEscudo.h"
#include "../../Sistemas/Inventario/Item.h"
#include "../../Sistemas/Inventario/Itens/ItemConsumivel.h"
#include "../../Entidades/Racas/Anao.h"
#include "../../Entidades/Racas/Elfo.h"
#include "../../Entidades/Racas/Humano.h"
#include "../../Entidades/Racas/Orc.h"
#include "../../Entidades/Racas/RacaBase.h"
#include "../../Entidades/Inimigos/Goblin.h"
#include "../../Entidades/Inimigos/Slime.h"
#include "../../Entidades/Inimigos/Troll.h"
#include "../../Entidades/Inimigos/Fada.h"
#include "../../Entidades/Inimigos/Mimico.h"
#include "../../Sistemas/Progresso/Diario.h"
#include "../../Perspectiva/TelasBase/Atributos/TelaAtributos.h"
#include "../../Perspectiva/TelasBase/Inventario/TelaInventario.h"
#include "../../Perspectiva/TelasBase/Menu/TelaMenu.h"
#include "../../Perspectiva/TelasBase/Menu/TelaAbertura.h"
#include "../../Perspectiva/TelasBase/Menu/TelaNome.h"
#include "../../Perspectiva/TelasBase/Menu/TelaRaca.h"
#include "../../Perspectiva/TelasBase/Menu/TelaClasse.h"
#include "../../Perspectiva/TelasBase/Menu/TelaDificuldade.h"
#include "../../Perspectiva/TelasBase/Menu/TelaParry.h"
#include "../../Perspectiva/TelasBase/Menu/TelaIntroducao.h"
#include "../../Perspectiva/TelasBase/RegistroTelas.h"
#include "../Utilidades/Aparencia.h"
#include "../Utilidades/FuncoesDialogo.h"
#include "../Utilidades/ControleDeInput.h"
#include "../Utilidades/GeradorAleatorio.h"
#include "../../Perspectiva/TelasBase/TelaBase.h"

std::unique_ptr<Personagem> MenuJogo::menuPrincipal() 
{
    TelaAbertura::exibir();

    while (true) {
        int selecao = TelaMenu::exibirOpcoesMenuPrincipal();
        
        std::string opcaoSelecionada;
        if (selecao == 0) opcaoSelecionada = "Novo Jogo";
        else opcaoSelecionada = "Sair";
        
        if (opcaoSelecionada == "Novo Jogo") {
            auto novoJogador = iniciarCriacaoDeSistemaPersonagem();
            if (novoJogador) return novoJogador;
        } else {
            if (RegistroTelas::modoRaycasterAtivo()) {
                return nullptr;
            } else {
                if (RegistroTelas::confirmarSaida()) {
                    return nullptr;
                }
            }
        }
    }
}

std::unique_ptr<Personagem> MenuJogo::iniciarCriacaoDeSistemaPersonagem() 
{
    std::string nomeDoPersonagem;
    std::unique_ptr<RacaBase> racaEscolhida;
    std::unique_ptr<ClasseBase> classeEscolhida;
    bool sistemaDeParryAtivado = false;
    TelaParry::Resultado::Modo modoParry = TelaParry::Resultado::Modo::Desligado;
    int nivelDeDificuldadeEscolhido = 2;

    for (;;) {
        {
            auto resultado = TelaNome::exibir();
            if (resultado.voltou) return nullptr;
            nomeDoPersonagem = resultado.nome;
        }

        std::string nomeRaca;
        {
            auto resultado = TelaRaca::exibir(nomeDoPersonagem);
            if (resultado.voltou) continue;

            nomeRaca = resultado.nome;
            racaEscolhida = FabricaRacas::criarRaca(resultado.racaSelecionada);
        }

        std::string nomeClasse;
        {
            auto resultado = TelaClasse::exibir(nomeDoPersonagem, nomeRaca);
            if (resultado.voltou) continue;

            nomeClasse = resultado.nome;
            classeEscolhida = FabricaClasses::criarClasse(resultado.classeSelecionada);
        }

        {
            auto resultado = TelaDificuldade::exibir(nomeDoPersonagem, nomeRaca, nomeClasse);
            if (resultado.voltou) continue;
            nivelDeDificuldadeEscolhido = resultado.indice + 1;
        }

        {
            auto resultado = TelaParry::exibir(nomeDoPersonagem, nomeRaca, nomeClasse);
            if (resultado.voltou) continue;
            modoParry = resultado.modo;
            sistemaDeParryAtivado = resultado.modo != TelaParry::Resultado::Modo::Desligado;
        }

        auto personagemCriado = std::make_unique<Personagem>(nomeDoPersonagem, std::move(racaEscolhida), std::move(classeEscolhida));
        personagemCriado->definirParryAtivado(sistemaDeParryAtivado);
        personagemCriado->definirParryModerno(modoParry == TelaParry::Resultado::Modo::Movimento);
        personagemCriado->definirDificuldade(static_cast<DificuldadeJogo>(nivelDeDificuldadeEscolhido));

        Diario::instancia().registrarRaca(personagemCriado->obterRaca()->obterNomeRaca());
        Diario::instancia().registrarClasse(personagemCriado->obterNomeClasse());

        for (Item* item : personagemCriado->obterInventario()->obterTodosOsItens()) {
            Diario::instancia().registrarItem(Aparencia::removerCoresANSI(item->obterNomeItem()));
        }

        TelaIntroducao::exibir();
        return personagemCriado;
    }
}
