#include "OrkExilado.h"
#include <iostream>
#include "../Sistemas/SistemaPersonagem.h"
#include "../Inventario/FabricaItens.h"
#include "../Utilidades/SimplificacoesAparencia.h"
#include <memory>

std::string OrkExilado::obterNomeRaca() const { return "Ork Exilado"; }
Atributos OrkExilado::obterAtributosRaca() const { return { 200, 20, 10, 15, 20, 0, 0 }; }

std::vector<std::string> OrkExilado::obterAparenciaRaca() const
{
    static const std::vector<std::string> aparencia =
    {
        "                         @@@@%                                            ",
        "                        @%@@@%                         %                 ",
        "                       #.....+%%+*          @#@      % %                 ",
        "                       :...:=-%*%*     *** *%*#      %#%                 ",
        "                  #  -:===*=##-=-::. *+%##%%%##  ## %%#%                 ",
        "                %   *#-:**-++==**#*+-#%%%@%%%**%@#+#%@%                  ",
        "                 %##**+**+***##%%@% @%%%@@@@#*@@%%#@@@#+ #               ",
        "               @@%###@#*#%@@@%%%@@#@%%%%%%%%*@@@%@@@@@%%*++              ",
        "                @###@@%@#*%%%%@@%%@@%#@@@@@#*@@%@@@@@@@%##               ",
        "               %%%##@%@#%@%@@@@%%@@@@@@@@@@#@@@@@@@@@%%%@                ",
        "            @@@@@@%**%%##%%%@@%@@@@@@@@@@@@%@@@@@@@@@@%*+                ",
        "              @@@%%#%@@%#+#%%@@@@@@@@@@@@@@@@@@@@@%@@%#=*-               ",
        "              @%@%@%@@@*#@@#@%%%@@@@@@@@@@@@@@@@%@@@%%#**=-              ",
        "               %@@@%@@@@%*#%+*#%%@@@@@@@@@@@@@@@%%%####%###+.            ",
        "               @@%@@@@@@#@#%**%%%%%@@@@@@@@@@@@%%%%%#%%%@%%#+==          ",
        "     @@%%@       @@@@@%@%%%%%%%%@%%@@@@@@@@@%@@%%%%%%%@%%@%#*#*+         ",
        "      ##%@#%     %@@@@@@%@@%%%%%%%@@@@@@@@@@@%%###    @%@@@@%%%%%        ",
        "     %#%###@     %@@@@@@@@@@@@@@@@@@@@@@@@@@%%%%#    @@@@@@@@@@@@@       ",
        "     @@%%%@@@  %#@%@@@@@@@@@@@@@@@@@@@@@@@@@@@%%*%   @@@@@@@@@@@@@  @@%% ",
        "     *#@@@@@@%@*#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  @@@@@@@@@@@@%@%@%%%@",
        "  @ ##@@@@@@@@%*##%@@@@@@@@%@@@@@@@@@@@@@@@@@@%@@@@%# @@@@@%%@@@@@#*@%@@ ",
        " %#%%@@@@@@@@@@@***%@@@@@%@@@@@@@@@@@@@%@@@@%%%@@@@%@  @@@@%%%%@%%%%%%   ",
        " ##@@@@%@@@@%@@@@%*#@@@@ %@@@@@@@@@%%@@@@@@@%%@@@@@@    @%#%%%@@@@@%     ",
        "%#@@@@%    ++*@@@%@%% =++*@@@@@%@@@@@@@%@%##*++++%@@     @#*#%@@%%%      ",
        "%#@@@@%    **%#%%%@@@@%*#@@@@@@@@@@%%%@@%*#****++*@@    +=**%#%%%#       ",
        "@%@@@%   *   #%#%%%*##**%@@%@@@%@@@%%%@@****+*****#@    *#*#%%#%#%*      ",
        "@@@@@@#+=#%    %%@%%%%@%#@@%@%#%@@@%%%@%#**++*#%###%   #*#####%####      ",
        " @@@@@@%%%%      *%%%%%%%@@@@##@@@@%%@@%#*#**#%###%%  #*%%%@%%#%%%%      ",
        " @%@@@@@@%%#    +#%%@@@%%@%@%%%@@@@@%@@%#**###%##%%%# %@%%%@@%%%%%       ",
        "   @@@@@@      +*##%%%@@%%@@%%%@@@@@@@@#++*#%%###%@@% @@%@%@%%%%         ",
        "               +=**%%%@@@@@@@%#@@@@@@@@#**#%@@%%%%@@%@@@@@               ",
        "              +**###%@@@@@@@@%%@@@@@@@@%#%%%%%@%@@@@#@@%@@      #        ",
        "             %*#%#@@@@@@@@@@@@@@@@@@@@@@@@%%%%@@@@@%%%#@@%      %        ",
        "             %#%%@%%@@@@@@@@@@@@@@@@@@@ @@@%%@@@@@@%%@%@@@%%  %#%        ",
        "              @@@%@@@@@@%%%#   @@@@@@@@  @@@@@@@@###%%@@@@@%%###%        ",
        "               @@@@@@@@@@%%%%%%  @       %@@@@@@@%@@@@@@@@@@@%%%@        ",
        "              %%@@@@@@@@@@@@@@%%         @@@@@@@@@@@@@@@@@@@@@%%         ",
        "             @%%@%@@@@@@@@@@@@%%@        %@@@@@@@@@@@@@@@@@@@@%%         ",
        "             %%@@@@@@@@@@@@@@@@@%       %%@@@@@@@@@@@@@@@@@@@@@@         ",
        "             @@@@@@@@@@@@@@@@@@        %%@@@@@@@@@@@@@@@@@@@@@%          ",
        "               @@@@@@@@@@@@@@@@       %%@@@@@@@@@@@@@@@@@@@@@%@          ",
        "               %@@@@@@@@@@@@@@#       %%@@@@@@@@@@@@@@@@@@@@@@           ",
        "                @@@@@@@@@@@@@%        @@@@@@@@@@@@@@@@@@@@ @             ",
        "                @@@@@@@@@@@@@@@         @@@@@@@@@@@@@@@@@                ",
        "                 @@@@@@@@@@@@@%%          @@@@@@@@@@@@@@@                ",
        "              @@%%@@@@@@@@@@@@@@           @@@@@@@@@@@@@@@               ",
        "          @%%%%%@@@@@@@@@@@@@@@@@@         @@@@@@@@@@@@@@@               ",
        "       @@@@@@@@@@@@@@@@@@@@@@@@@@@@        @@@@@@@@@@@@@@@               ",
        "         @@@@@@@@@@@@@@@@@@@@@@@@@         %@@@@@@@@@@@@@@               ",
        "          @@@@@@@@@@@@@@@@@@@@@@          @@@@@@%%%%@@@@@@@@             ",
        "                                          @@@@@@@@@@@@@@@@@@             ",
        "                                          @@@@@@@@@@@@@@@@@@@            ",
        "                                         @@@@@@@@@@@@@@@@@@@@@           ",
        "                                         @@@@@@@@@@@@@@@@@@@@@           ",
        "                                           @@@@@@@@@@@@@@@@@@            "
    };
    return aparencia;
}

std::vector<std::unique_ptr<Item>> OrkExilado::obterEquipamentoRaca() const {
    std::vector<std::unique_ptr<Item>> equipamentos;
    equipamentos.push_back(FabricaItens::criarItem(ItemID::MachadoGuerra));
    equipamentos.push_back(FabricaItens::criarItem(ItemID::ArmaduraTrapos));
    return equipamentos;
}

void OrkExilado::realizarDrops(SistemaPersonagem* inimigo, SistemaPersonagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal)
{
    int xpDrop = 120;
    int ouroDrop = 100;
    jogadorAtual->ganharXp(xpDrop);
    jogadorAtual->ganharOuro(ouroDrop);
    xpTotal += xpDrop;
    ouroTotal += ouroDrop;
    
    std::cout << SimplificacoesAparencia::cor(Cor::FUNDO_AMARELO) << "+" << ouroDrop << "G" << SimplificacoesAparencia::cor(Cor::RESET) << " " << SimplificacoesAparencia::cor(Cor::FUNDO_AZUL) << "+" << xpDrop << " XP" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";

    for (int i = 0; i < 2; ++i) {
        jogadorAtual->obterInventario()->adicionarItem(FabricaItens::criarItem(ItemID::MachadoGuerra));
        itensObtidos.push_back("Machado de guerra danificado");
    }
    std::cout << SimplificacoesAparencia::cor(Cor::BRANCO) << "+2x Machado de guerra danificado" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
    
    jogadorAtual->obterInventario()->adicionarItem(FabricaItens::criarItem(ItemID::ArmaduraTrapos));
    std::cout << SimplificacoesAparencia::cor(Cor::BRANCO) << "+1x Armadura de trapos e sucata" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
    itensObtidos.push_back("Armadura de trapos e sucata");
}

std::vector<std::string> OrkExilado::obterMapaCaverna(bool bjornResgatado)
{
    std::vector<std::string> mapa = {
        "  ########################################",
        "#############################################",
        "###########.........###########################",
        "######[^S]......................Om...Bn.########",
        "#######.........................###############",
        "###############################################",
        "  ########################################",
    };
    if (bjornResgatado) mapa[3] = "######[^S]......................Om......########";
    return mapa;
}