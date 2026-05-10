#include "Fada.h"
#include <iostream>
#include "../Sistemas/SistemaPersonagem.h"
#include "../Inventario/FabricaItens.h"
#include "../Utilidades/Aparencia.h"
#include "../Utilidades/GeradorAleatorio.h"
#include "../Gerenciadores/GerenciadorDrops.h"
#include <memory>

// --- INFORMACOES DA RACA ---
std::string Fada::obterNomeRaca() const 
{ 
    return "Fada"; 
}

Atributos Fada::obterAtributosRaca() const 
{ 
    return { 40, 0, 30, 2, 5, 5, 5 }; 
}

std::vector<std::unique_ptr<Item>> Fada::obterEquipamentoRaca() const 
{
    std::vector<std::unique_ptr<Item>> equipamentos;
    equipamentos.push_back(FabricaItens::criarItem(ItemID::VarinhaCorroida));
    return equipamentos;
}

// --- HABILIDADE DA RACA ---
std::string Fada::obterNomeHabilidadeRaca() const { return "Nenhuma"; }
std::string Fada::obterDescricaoHabilidadeRaca() const { return "Monstros nao possuem passivas"; }

// --- APARENCIA ---
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

// --- BESTIARIO E DROPS ---
InfoBestiario Fada::obterInfoBestiario() const {
    return {
        "Floresta", 
        "Superficie",
        "Fadas corrompidas pela energia negra da floresta.",
        "Elas costumavam guiar viajantes, agora os perdem.",
        {FabricaItens::obterNomeDeID(ItemID::PoMagico), FabricaItens::obterNomeDeID(ItemID::VarinhaCorroida), "Ouro"},
        3
    };
}

void Fada::realizarDrops(SistemaPersonagem* inimigo, SistemaPersonagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal)
{
    int xpDrop = 45;
    int ouroDrop = 20;
    GerenciadorDrops::relatarEProcessarXpOuro(jogadorAtual, xpDrop, ouroDrop, ouroTotal, xpTotal);

    if (GeradorAleatorio::rolarChance(65)) {
        GerenciadorDrops::darEProcessarItem(jogadorAtual, ItemID::VarinhaCorroida, 1, itensObtidos);
    }

    int qtdPo = GeradorAleatorio::obterInteiro(1, 6);
    GerenciadorDrops::darEProcessarItem(jogadorAtual, ItemID::PoMagico, qtdPo, itensObtidos);
}