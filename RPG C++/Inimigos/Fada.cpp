#include "Fada.h"
#include <iostream>
#include <cstdlib>
#include "../Sistemas/SistemaPersonagem.h"
#include "../Inventario/FabricaItens.h"
#include "../Utilidades/SimplificacoesAparencia.h"

std::string Fada::obterNomeRaca() const { return "Fada"; }
Atributos Fada::obterAtributosRaca() const { return { 40, 0, 30, 2, 5, 10, 10 }; }
std::string Fada::obterNomeHabilidadeRaca() const { return "Nenhuma"; }
std::string Fada::obterDescricaoHabilidadeRaca() const { return "Monstros nao possuem passivas"; }

std::vector<std::string> Fada::obterAparenciaRaca() const
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

void Fada::realizarDrops(SistemaPersonagem* inimigo, SistemaPersonagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal)
{
    int xpDrop = 45;
    int ouroDrop = 20;
    jogadorAtual->ganharXp(xpDrop);
    jogadorAtual->ganharOuro(ouroDrop);
    xpTotal += xpDrop;
    ouroTotal += ouroDrop;
    
    std::cout << SimplificacoesAparencia::cor(Cor::FUNDO_AMARELO) << "+" << ouroDrop << "G" << SimplificacoesAparencia::cor(Cor::RESET) << " " << SimplificacoesAparencia::cor(Cor::FUNDO_AZUL) << "+" << xpDrop << " XP" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";

    if ((std::rand() % 100) < 65) 
    {
        jogadorAtual->obterInventario()->adicionarItem(FabricaItens::criarItem("Varinha corroida"));
        itensObtidos.push_back("Varinha corroida");
        std::cout << SimplificacoesAparencia::cor(Cor::BRANCO) << "+1x Varinha corroida" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
    }

    int qtdPo = (std::rand() % 6) + 1;
    for (int i = 0; i < qtdPo; ++i) {
        jogadorAtual->obterInventario()->adicionarItem(FabricaItens::criarItem("Po magico"));
        itensObtidos.push_back("Po magico");
    }
    std::cout << SimplificacoesAparencia::cor(Cor::BRANCO) << "+" << qtdPo << "x Po magico" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
}