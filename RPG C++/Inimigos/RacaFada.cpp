#include "RacaFada.h"
#include <iostream>
#include <cstdlib>
#include "../Sistema/Personagem.h"
#include "../Inventario/Item.h"
#include "../Inventario/Material.h"

std::string RacaFada::obterNomeRaca() const { return "Fada"; }
Atributos RacaFada::obterAtributosRaca() const { return { -50, 0, 30, 2, 5, 10, 10 }; }
std::string RacaFada::obterNomeHabilidadeRaca() const { return "Nenhuma"; }
std::string RacaFada::obterDescricaoHabilidadeRaca() const { return "Monstros nao possuem passivas"; }

std::vector<std::string> RacaFada::obterAparenciaRaca() const
{
    static const std::vector<std::string> aparencia =
    {                    
        "..         .                        .         . .   ",
        "   .*#.      :.                     ..      #**.     ",
        "   ***+#-..#**#                    .****..%****#     ",
        " ..*+*+*#******.                   .*++***#**+**.    ",
        "   %*++++#++++*.                   =++#++#++++*.     ",
        "     .*##*####*#.        ..**.    .*-###****+        ",
        " :%#+..**+++****#.   .+#=*%#*+.  .#*-:#*+**:..:+%    ",
        "     .**##++******# *#===*%%#...#*****+++#***.       ",
        "   :.#**+**##*****#%#%*+@@%*. .#*****###**+*##..     ",
        "    .#*####::%###*##%#*+##%#+..##*####::####**.      ",
        "    .#*##******##%##**##+@%**+%%###**%****#**#.      ",
        "      .#*++*###*#####%*+%++*@%@##**%@@%++**#.        ",
        "      .#**##*****#:##***#**%%@%%%*%%%%*#***#.        ",
        "       .-#*****#. .%%%%%%%%%%@%@@%@%#****#...        ",
        "         .*###+.  @%%%%@%%%%%%@%%@..@**#*. .         ",
        "         .#*#..  .%%@@@@%@@@@%@@@@  %:**.            ",
        "          #%.   .%%@@@@@%%%%%%@@@@. % .*.            ",
        "                @%%@@@@%%%%%%%%@@@@.@    .           ",
        "              .@%%@@@++%%%%%%%%@@@@.%                ",
        "              @..%@@%%%%%%%%%%%@@@@..                ",
        "                 @@%%%%%%%%%%%%%@..=.                ",
        "                .@@@%%%%%%%%%%%%. .%.                ",
        "                 @%%%%%%@%%%%%@%.  %                 ",
        "                 .=*%%%%%%%%%%%%. .%.                ",
        "                  +=+++*+**++%#%.  %.                ",
        "                  %%@.@@@%%@%@%+. .*                 ",
        "                  :%..+%*@*@%@.@  ..                 ",
        "                      .-.%=.      %.                 ",
        "                      :-.==.                         ",
        "                      .==.=*                         ",
        "                       .#..."
    };
    return aparencia;
}

void RacaFada::realizarDrops(Personagem* inimigo, Personagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal)
{
    int xpDrop = 45;
    int ouroDrop = 20;
    jogadorAtual->ganharXp(xpDrop);
    jogadorAtual->ganharOuro(ouroDrop);
    xpTotal += xpDrop;
    ouroTotal += ouroDrop;
    
    std::cout << "\033[43m+" << ouroDrop << "G\033[0m \033[44m+" << xpDrop << " XP\033[0m\n";

    int qtdPo = (std::rand() % 6) + 1;
    for (int i = 0; i < qtdPo; ++i) {
        jogadorAtual->obterInventario()->adicionarItem(std::make_unique<Material>("Po magico"));
        itensObtidos.push_back("Po magico");
    }
    std::cout << "\033[37m+" << qtdPo << "x Po magico\033[0m\n";
}