#include <iostream>

#include "ClasseMago.h"
#include "../Inventario/Arma.h"
#include "../Inventario/Escudo.h"
#include "../Inventario/Armadura.h"
#include "../Inventario/PocaoCura.h"

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

std::vector<Item*> ClasseMago::obterEquipamentoClasse() const 
{
    return 
    {
        new PocaoCura(), new PocaoCura(), new PocaoCura(),
        new Arma("Cajado", 0, 30), // Dano Fisico, Dano Magico
        new Escudo("Barreira magica", 50, 2), // Reducao Fixa, Durabilidade
        new Armadura("Tunica", 2) // Reducao Fixa
    };
}

std::string ClasseMago::obterNomeHabilidadeClasse() const { return "Estrategia arcana"; }
std::string ClasseMago::obterDescricaoHabilidadeClasse() const { return "Alterna entre ataque em area ou alvo unico (não gasta seu turno)"; }
void ClasseMago::usarHabilidadeClasse(Personagem* u, std::vector<Personagem*>& /*inimigos*/) 
{
    u->alternarModoAtaque();
    const char* modo = u->obterModoAtaqueArea() ? "AREA (Dano dividido)" : "UNICO (Dano total)";
    std::cout << "[HABILIDADE]: Estrategia arcana! Modo de ataque: " << modo << "\n";
}
