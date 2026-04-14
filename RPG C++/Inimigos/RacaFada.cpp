#include "RacaFada.h"
#include <iostream>
#include <cstdlib>
#include "../Sistema/Personagem.h"
#include "../Inventario/Item.h"
#include "../Inventario/Material.h"

std::string RacaFada::obterNomeRaca() const { return "Fada"; }
Atributos RacaFada::obterAtributosRaca() const { return { -70, 0, 15, 0, 0, 10, 30 }; }
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

void RacaFada::realizarDrops(Personagem* inimigo, Personagem* jogadorAtual, std::vector<std::string>& itensObtidos)
{
    int qtdPo = (std::rand() % 6) + 1;
    for (int i = 0; i < qtdPo; ++i) {
        jogadorAtual->obterInventario()->adicionarItem(new Material("Po magico"));
        itensObtidos.push_back("Po magico");
    }
    std::cout << "\033[37m+" << qtdPo << "x Po magico\033[0m\n";
}