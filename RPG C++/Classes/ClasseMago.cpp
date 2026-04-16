#include <iostream>
#include <memory>

#include "ClasseMago.h"
#include "../Inventario/Arma.h"
#include "../Inventario/Escudo.h"
#include "../Inventario/Armadura.h"
#include "../Inventario/ItemConsumivel.h"

std::string ClasseMago::obterNomeClasse() const 
{
     return "Mago"; 
}

std::vector<std::string> ClasseMago::obterAparenciaClasseMenu() const 
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

Atributos ClasseMago::obterAtributosClasse() const
{ 
    // Ordem: { Vida, Forca, Destreza, Resistencia, Constituicao, Inteligencia, Sabedoria }
    return { 0, 5, 5, 2, 10, 15, 15 };
}

std::vector<std::unique_ptr<Item>> ClasseMago::obterEquipamentoClasse() const 
{
    std::vector<std::unique_ptr<Item>> equipamentos;
    for (int i = 0; i < 3; ++i) {
        auto pocao = std::make_unique<ItemConsumivel>("Pocao de Cura (30%)", 6);
        pocao->adicionarPropriedade(Propriedade::ConsumivelCura);
        equipamentos.push_back(std::move(pocao));
    }
    equipamentos.push_back(std::make_unique<Arma>("Cajado", 0, 30));
    equipamentos.push_back(std::make_unique<Escudo>("Barreira magica", 50, 2));
    equipamentos.push_back(std::make_unique<Armadura>("Tunica", 2));
    return equipamentos;
}

std::string ClasseMago::obterNomeHabilidadeClasse() const { return "Estrategia arcana"; }
std::string ClasseMago::obterDescricaoHabilidadeClasse() const { return "Alterna entre ataque em area ou alvo unico (não gasta seu turno)"; }
void ClasseMago::usarHabilidadeClasse(Personagem& u, std::vector<Personagem*>& /*inimigos*/) 
{


    tipoAtaqueAtual = (tipoAtaqueAtual == TipoAtaque::UNICO) ? TipoAtaque::AREA : TipoAtaque::UNICO;
    const char* modo = (tipoAtaqueAtual == TipoAtaque::AREA) ? "AREA (Dano dividido)" : "UNICO (Dano total)";
    std::cout << "[HABILIDADE]: Estrategia arcana! Modo de ataque: " << modo << "\n";
}

TipoAtaque ClasseMago::obterTipoAtaque() const { return tipoAtaqueAtual; }
bool ClasseMago::habilidadeConsomeTurno() const { return false; }
