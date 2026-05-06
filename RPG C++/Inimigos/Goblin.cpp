#include "Goblin.h"
#include <iostream>
#include "../Sistemas/SistemaPersonagem.h"
#include "../Inventario/FabricaItens.h"
#include "../Utilidades/Aparencia.h"
#include "../Gerenciadores/GerenciadorDrops.h"
#include "../Utilidades/GeradorAleatorio.h"

std::string Goblin::obterNomeRaca() const { return "Goblin"; }
Atributos Goblin::obterAtributosRaca() const { return { 60, 10, 15, 5, 5, 0, 0 }; }
std::string Goblin::obterNomeHabilidadeRaca() const { return "Nenhuma"; }
std::string Goblin::obterDescricaoHabilidadeRaca() const { return "Monstros nao possuem passivas"; }

std::vector<std::unique_ptr<Item>> Goblin::obterEquipamentoRaca() const {
    std::vector<std::unique_ptr<Item>> equipamentos;
    equipamentos.push_back(FabricaItens::criarItem(ItemID::AdagaPedra));
    return equipamentos;
}

const std::vector<std::string>& Goblin::obterAparenciaRaca() const
{
    static const std::vector<std::string> aparencia =
    {                    
        "                  --=++++++++*.        ..          ",
        "      #=:      .::+++++*++++++++*.. =#*+=..        ",
        "       #-*:. .+=+++++++=:====+*+**%=%=*=#==+.+     ",
        "       .=--+=:=:=+++*:==*::-***++++-=#+##*### .    ",
        "        ++-=+..-=++#*=#..@#:**++*+#+++++==+=*..    ",
        "        ===+#..@%*-+++..%%+*%%++++-%+=+====+* =    ",
        "        :=+-++%*+:=+++++*:=====++*+=+.==#+=* =     ",
        "    *..  #==#===+:===*++==+*==++++*...:+**# +      ",
        "  -*+#*+%..+#:==+%=+*+++#=*+++***...  ..=+..       ",
        " #%*=*:=*. ..%++#:::*%+=*%+++****+===...*=* .      ",
        " -=***+:=+.= -..++#+*=**%+*+**+++*+-===+==+.-      ",
        " .*.=.=+=+.+.  .+.+++-=:=*++====+*#.*+++==+..      ",
        " .=+. ...=+*.   .#====+++=====+=+*# = ...*# =      ",
        "       .. %=++##==*.:+=========++*@ .   .=-.       ",
        "           .:=+**# ..-=+=====++**%# .              ",
        "            .... +.  #*+++++*%#%##%%..             ",
        "             . .   .########%#%%%%%%%..            ",
        "                 .############%%%%%%%%..           ",
        "               .+*%#####%%%%%%%%%%%%%%%.=          ",
        "               .-=+%#@=%%%%%+%+%*.*@*=+*..         ",
        "               +=+* *.. :--=-=--+-..:+=*.+         ",
        "               .=**=.              .:=++.:         ",
        "               ..=+* .             .:=+..          ",
        "                .#=* -              *=*.=          ",
        "                 .#+ -               =*.:          ",
        "                  .=# .              ++.           ",
        "              .#===+*#              ++===%.        ",
        "         %===+=+=++#..:            ..#-======*=..  ",
        "         #*=+=#==*# =                ..%===--+++%. ",
        "          ....##...                   :.#+++#*.=   "
    };
    return aparencia;
}

void Goblin::realizarDrops(SistemaPersonagem* inimigo, SistemaPersonagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal)
{
    int xpDrop = 40;
    int ouroDrop = 20;
    GerenciadorDrops::relatarEProcessarXpOuro(jogadorAtual, xpDrop, ouroDrop, ouroTotal, xpTotal);

    if (inimigo->obterArma() && inimigo->obterArma()->obterNomeItem() == "Adaga artesanal de pedra") 
    {
        if (GeradorAleatorio::rolarChance(65)) 
        {
            jogadorAtual->obterInventario()->adicionarItem(FabricaItens::criarItem(ItemID::AdagaPedra));
            GerenciadorDrops::relatarDropItem("Adaga artesanal de pedra", 1);
            itensObtidos.push_back("Adaga artesanal de pedra");
        }
    }
    
    int qtdDentes = GeradorAleatorio::obterInteiro(4, 8);
    for (int i = 0; i < qtdDentes; ++i) {
        jogadorAtual->obterInventario()->adicionarItem(FabricaItens::criarItem(ItemID::DenteGoblin));
        itensObtidos.push_back("Dente de goblin");
    }
    GerenciadorDrops::relatarDropItem("Dente de goblin", qtdDentes);
}