#include "Mago.h"

#include <iostream>
#include <memory>

#include "../../Sistemas/Combate/Combate.h"
#include "../../Sistemas/Inventario/FabricaItens.h"
#include "../../Core/Utilidades/Aparencia.h"
#include "../../Core/Utilidades/FuncoesDialogo.h"
#include "../../Perspectiva/TelasBase/Combate/TelaCombate.h"

// --- INFORMACOES DA CLASSE ---
std::string Mago::obterNomeClasse() const 
{
     return "Mago"; 
}

const std::vector<std::string>& Mago::obterAparenciaClasseMenu() const 
{
    static const std::vector<std::string> aparencia = 
    {
        "                                        =@@@%%   ",
        "                                        *%%***.  ",
        "                                        **#- .#  ",
        "                                       #%%%##+#: ",
        "                   -#++                .%%@%-    ",
        "                  ##*+:-=              *@@@@.    ",
        "                 #%#*+:-*.              %@@@*    ",
        "                *%*%##%++*             .@@+      ",
        "               :#@%**.+*+#*            *@@%      ",
        "   : .         *##*@#*-+#:##           :@@.      ",
        "   ::* +    .#%%%%:#@=*+-#*#+*+        =@@-      ",
        "    -=+-   @%#%@@@+*@:*==%-++-::*      *%*       ",
        "    -*-* :@%#%****+*%+**++** --:-*     %@        ",
        "    +-* @#%*#*-=+*-*%+*#**:+::*-#@     *@.       ",
        "    @#*+ **@%==:*:=:%-*+#***:=@+::=    %@        ",
        "    %#*.*#*%@%@#+=-*=--##**%*#@%*::    *@        ",
        "    @@*:=#%%@+@%:**:-**%*=+-#@@@%*::-  #@        ",
        "    @@*=**@@@**+***+:#%*--+**.%@@@*-::-@*        ",
        "    @@*+##@@@: %##+*%#***=***: @@@+*-* ::        ",
        "    @@**%@@@:  @%*%@%*+%%+*#-  @@@@#:%%**+       ",
        "    @@#%@@@@  +@@*%@#++%%=#%* .@@@@*@###         ",
        "    @@%@@@@   @@%%@***#@%+#%#   #@@@@@@@*        ",
        "   :@@#@@.    @@@@@*#*%%#*%%#    @@@@@@@=        ",
        "   =@%%:    -%@@%@%*#=*+*#%%%:   =@@@@@@         ",
        "   =%%  %@%##@@@@%@*#*#*%+#%@#    %@@@@@         ",
        "   *:  =@@%@@#*#%%###%%%@@#%@@.    %@@@@         ",
        "      -@@@@%@@%###*%%@@*%@@*%%@     @@@@         ",
        "        *@@@@%##%%%%*%@%@#%%%%%@     @@          ",
        "          #@#%##@%%%%@@@@#@%%@%@*    @+          ",
        "          @@%@@#@%%%%%#@@@%%@%%@@@:              ",
        "         %@@%@@%@@@@%@@@@@%@@%@@%*@              ",
        "        @@@@%@@%@@%@%%@@@@@@@%@%@@*@             ",
        "        @@@@@@@@@@@@@@@@@@@@@%@@@@@@             ",
        "        @@@@@@@@@@@@@@@@@@@@@%%@%@@%*            ",
        "        @@@@@@@@@@@@@@@@@@@  .%@%@%%#            ",
        "        @%%@@@@@ +@@@@@@@@.   *@%@%%#            ",
        "         @@@@%@@: @@@@@@@.    :%@%%%             ",
        "         #@@@%@@* @@@@%@@      %@%%              ",
        "            #@@@%  %@@%@%@     %@%#              ",
        "             *@@%  @%%%%@@     %@%               ",
        "               +% .@%@%@@%     *#                ",
        "                   %%%#                          ",
        "                  @%%%%                          "
    };
    return aparencia;
}

Atributos Mago::obterAtributosClasse() const
{
    return { 0, 5, 5, 3, 10, 15, 15 };
}

std::vector<std::unique_ptr<Item>> Mago::obterEquipamentoClasse() const 
{
    auto equipamentos = FabricaItens::criarKitPocoes();

    equipamentos.push_back(FabricaItens::criarItem(ItemID::CajadoCristal));
    equipamentos.push_back(FabricaItens::criarItem(ItemID::BarreiraMagica));
    equipamentos.push_back(FabricaItens::criarItem(ItemID::Tunica));
    return equipamentos;
}

// --- PASSIVA DA CLASSE ---
std::string Mago::obterNomePassivaClasse() const 
{ 
    return "Foco arcano"; 
}

std::string Mago::obterDescricaoPassivaClasse() const 
{ 
    return "Ataques ressoam (25% em area) ou causam +25% de dano em alvo unico."; 
}

// --- HABILIDADE DA CLASSE ---
std::string Mago::obterRecargaHabilidadeClasse() const 
{ 
    return "Recarga: 3 turnos."; 
}

std::string Mago::obterNomeHabilidadeClasse() const 
{ 
    return "Canalizacao arcana"; 
}

std::string Mago::obterDescricaoHabilidadeClasse() const 
{ 
    return "Pula seu turno para se defender e dobra o dano no proximo turno. Recarga: 3 turnos."; 
}

void Mago::usarHabilidadeClasse(Combate* /*combate*/, Personagem* personagemUsuario, std::vector<Personagem*>& /*listaDeInimigos*/) 
{
    int turnosRestantes = personagemUsuario->obterCooldown(HabilidadeID::CanalizacaoArcana);
    if (verificarEReportarRecarga(personagemUsuario, turnosRestantes, obterNomeHabilidadeClasse())) return;
    
    personagemUsuario->definirMultiplicador(2.0);
    personagemUsuario->adicionarEfeito(std::make_unique<EfeitoBuffAtributos>(2)); 
    personagemUsuario->definirCooldown(HabilidadeID::CanalizacaoArcana, 4);
    
    Item* escudo = personagemUsuario->obterEscudo();
    if (escudo) {
        personagemUsuario->definirDefendendo(true);
        std::string msg = FuncoesDialogo::formatarMsgHabilidade("Canalizacao arcana! Defendendo com " + escudo->obterNomeItem() + "! 2x Dano no prox. ataque!");
        notificarMensagemCombate(msg, msg);
    } else {
        std::string msg = FuncoesDialogo::formatarMsgHabilidade("Canalizacao arcana! Foco magico para 2x Dano no proximo ataque!");
        notificarMensagemCombate(msg, msg);
    }
}

// --- PROCESSAMENTO DE DANO  ---
int Mago::processarDanoPreAtaque(Personagem* /*atacante*/, Personagem* defensor, int danoBase, bool isAtacanteJogador, size_t qtdInimigos) {
    if (defensor == nullptr) return danoBase;
    if (!isAtacanteJogador || qtdInimigos <= 1) {
        int danoAumentado = static_cast<int>(danoBase * 1.25);
        std::string logMsg = FuncoesDialogo::formatarMsgHabilidade("Foco Arcano: Dano concentrado aumentado em 25%!", Cor::MAGENTA);
        notificarMensagemCombate(logMsg, logMsg);
        return danoAumentado;
    }
    return danoBase;
}

void Mago::processarDanoPosAtaque(Personagem* atacante, Personagem* alvoAtual, Personagem* defensorPrincipal, int danoBase, int danoPerfurante, const std::function<void(Personagem*, Personagem*, int, int)>& aplicarDano, bool isAtacanteJogador, bool isArea, bool& ativouPassiva) {
    if (isAtacanteJogador && !isArea && alvoAtual != defensorPrincipal && alvoAtual->obterVida() > 0) {
        if (!ativouPassiva) {
            int danoAreaMsg = static_cast<int>(danoBase * 0.25);
            std::string logMsg = FuncoesDialogo::formatarMsgHabilidade("Foco Arcano: A magia ressoa, causando " + std::to_string(danoAreaMsg) + " de dano aos inimigos proximos!", Cor::MAGENTA);
            notificarMensagemCombate(logMsg, logMsg);
            ativouPassiva = true;
        }
        int danoArea = static_cast<int>(danoBase * 0.25);
        int perfuranteArea = static_cast<int>(danoPerfurante * 0.25);
        aplicarDano(atacante, alvoAtual, danoArea, perfuranteArea);
    }
}
