#include "Fada.h"
#include <iostream>
#include "../Sistemas/SistemaPersonagem.h"
#include "../Inventario/FabricaItens.h"
#include "../Utilidades/Aparencia.h"
#include "../Utilidades/GeradorAleatorio.h"
#include "../Gerenciadores/GerenciadorDrops.h"
#include <memory>

std::string Fada::obterNomeRaca() const { return "Fada"; }
Atributos Fada::obterAtributosRaca() const { return { 40, 0, 30, 2, 5, 10, 10 }; }
std::string Fada::obterNomeHabilidadeRaca() const { return "Nenhuma"; }
std::string Fada::obterDescricaoHabilidadeRaca() const { return "Monstros nao possuem passivas"; }

std::vector<std::unique_ptr<Item>> Fada::obterEquipamentoRaca() const {
    std::vector<std::unique_ptr<Item>> equipamentos;
    equipamentos.push_back(FabricaItens::criarItem(ItemID::VarinhaCorroida));
    return equipamentos;
}

const std::vector<std::string>& Fada::obterAparenciaRaca() const
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
    GerenciadorDrops::relatarEProcessarXpOuro(jogadorAtual, xpDrop, ouroDrop, ouroTotal, xpTotal);

    if (GeradorAleatorio::rolarChance(65)) 
    {
        jogadorAtual->obterInventario()->adicionarItem(FabricaItens::criarItem(ItemID::VarinhaCorroida));
        itensObtidos.push_back("Varinha corroida");
        GerenciadorDrops::relatarDropItem("Varinha corroida", 1);
    }

    int qtdPo = GeradorAleatorio::obterInteiro(1, 6);
    for (int i = 0; i < qtdPo; ++i) {
        jogadorAtual->obterInventario()->adicionarItem(FabricaItens::criarItem(ItemID::PoMagico));
        itensObtidos.push_back("Po magico");
    }
    GerenciadorDrops::relatarDropItem("Po magico", qtdPo);
}