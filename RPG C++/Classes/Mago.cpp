#include "Mago.h"

#include <iostream>
#include <memory>

#include "../Inventario/FabricaItens.h"
#include "../Utilidades/SimplificacoesAparencia.h"

std::string Mago::obterNomeClasse() const 
{
     return "Mago"; 
}

std::vector<std::string> Mago::obterAparenciaClasseMenu() const 
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
    std::vector<std::unique_ptr<Item>> equipamentos;
    
    int quantidadePocoes = 3;
    int porcentagemCura = 30;
    for (int i = 0; i < quantidadePocoes; ++i) {
        equipamentos.push_back(FabricaItens::criarItem("Pocao de Cura (" + std::to_string(porcentagemCura) + "%VM)"));
    }
    
    equipamentos.push_back(FabricaItens::criarItem("Cajado de cristal magico"));
    equipamentos.push_back(FabricaItens::criarItem("Barreira magica"));
    equipamentos.push_back(FabricaItens::criarItem("Tunica"));
    return equipamentos;
}

std::string Mago::obterNomePassivaClasse() const 
{ return "Foco arcano"; }
std::string Mago::obterDescricaoPassivaClasse() const 
{ return "Ataques ressoam (25% em area) ou causam +25% de dano em alvo unico."; }
std::string Mago::obterRecargaHabilidadeClasse() const 
{ return "Recarga: 3 turnos."; }

std::string Mago::obterNomeHabilidadeClasse() const 
{ return "Canalizacao arcana"; }
std::string Mago::obterDescricaoHabilidadeClasse() const 
{ return "Pula seu turno para se defender e dobra o dano no proximo turno. Recarga: 3 turnos."; }
void Mago::usarHabilidadeClasse(SistemaPersonagem* u, std::vector<SistemaPersonagem*>& /*inimigos*/) 
{
    int turnosRestantes = u->obterCooldown(HabilidadeID::EstrategiaArcana);
    if (turnosRestantes > 0) {
        std::cout << "\n[SISTEMA]: A habilidade " << obterNomeHabilidadeClasse() << " esta em recarga (" << turnosRestantes << " turnos)!\n";
        SimplificacoesAparencia::aguardarEnter();
        u->definirHabilidadeCancelada(true);
        return;
    }
    
    u->definirMultiplicador(2.0);
    u->adicionarEfeito(std::make_unique<EfeitoBuffAtributos>(2)); 
    u->definirCooldown(HabilidadeID::EstrategiaArcana, 4);
    
    Item* escudo = u->obterEscudo();
    if (escudo) {
        u->definirDefendendo(true);
        std::cout << "[HABILIDADE]: Canalizacao arcana! Voce se defende com " << escudo->obterNomeItem() << " e prepara um ataque devastador (2x Dano)!\n";
    } else {
        std::cout << "[HABILIDADE]: Canalizacao arcana! Voce foca sua energia para um ataque devastador (2x Dano) no proximo turno!\n";
    }
}

int Mago::processarDanoPreAtaque(SistemaPersonagem* atacante, SistemaPersonagem* defensor, int danoBase, bool isAtacanteJogador, size_t qtdInimigos) {
    if (defensor == nullptr) return danoBase;
    if (!isAtacanteJogador || qtdInimigos <= 1) {
        int danoAumentado = static_cast<int>(danoBase * 1.25);
        std::cout << SimplificacoesAparencia::cor(Cor::MAGENTA) << "[Foco Arcano]: Dano concentrado aumentado em 25%!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
        return danoAumentado;
    }
    return danoBase;
}

void Mago::processarDanoPosAtaque(SistemaPersonagem* atacante, SistemaPersonagem* alvoAtual, SistemaPersonagem* defensorPrincipal, int danoBase, int danoPerfurante, std::function<void(SistemaPersonagem*, SistemaPersonagem*, int, int)> aplicarDano, bool isAtacanteJogador, bool isArea, bool& ativouPassiva) {
    if (isAtacanteJogador && !isArea && alvoAtual != defensorPrincipal && alvoAtual->obterVida() > 0) {
        if (!ativouPassiva) {
            int danoAreaMsg = static_cast<int>(danoBase * 0.25);
            std::cout << SimplificacoesAparencia::cor(Cor::MAGENTA) << "[Foco Arcano]: A magia ressoa, causando " << danoAreaMsg << " de dano aos inimigos proximos!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
            ativouPassiva = true;
        }
        int danoArea = static_cast<int>(danoBase * 0.25);
        int perfuranteArea = static_cast<int>(danoPerfurante * 0.25);
        aplicarDano(atacante, alvoAtual, danoArea, perfuranteArea);
    }
}

TipoAtaque Mago::obterTipoAtaque() const { return TipoAtaque::UNICO; }
bool Mago::habilidadeConsomeTurno() const { return true; }
