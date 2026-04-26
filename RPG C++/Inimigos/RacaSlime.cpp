#include "RacaSlime.h"
#include <iostream>
#include <cstdlib>
#include "../Sistema/Personagem.h"
#include "../Inventario/FabricaDeItens.h"
#include "../Sistema/Tipos.h"
#include <memory>
#include "../Sistema/SimplificacoesAparencia.h"
#include "ControleDeDrops.h"

std::string RacaSlime::obterNomeRaca() const { return "Slime"; }
Atributos RacaSlime::obterAtributosRaca() const { return { 50, 15, 5, 10, 15, 0, 0 }; }
std::string RacaSlime::obterNomeHabilidadeRaca() const { return "Toque Gosmento"; }
std::string RacaSlime::obterDescricaoHabilidadeRaca() const { return "Ataques tem 20% de chance de causar Lentidao"; }

std::vector<std::unique_ptr<Item>> RacaSlime::obterEquipamentoRaca() const {
    std::vector<std::unique_ptr<Item>> equipamentos;
    equipamentos.push_back(FabricaDeItens::criarItem("Gosma acida (Arma)"));
    return equipamentos;
}

std::vector<std::string> RacaSlime::obterAparenciaRaca() const
{
    static const std::vector<std::string> aparencia =
    {
        "                                                                  ",
        "                                                                  ",
        "                                                                  ",
        "                                                                  ",
        "                       .:::.::--=.                                ",
        "                      .: .:.----=+++:                             ",
        "                       .-:-  +**=*#+**=                           ",
        "                             :+:====--==--::---.                  ",
        "                         -*==:=-=-:-==-:. ......--                ",
        "                    ..=+====---==--::-==-::....:.--:              ",
        "                  ++#+===-=-=++=-===+*:=-:::.....::=.             ",
        "                .+#*+++:-=====-==++***==-::.   ..-:=.             ",
        "                +#**+:-==.-+**##****##-=-::. ..:.:=-              ",
        "               -**#==+++*+*+++#%+=%%%%-=.-:::::-==.               ",
        "               +##***+**=***##***%####%+:---:==:.                 ",
        "               #=*#*+#***#*#############%                         ",
        "               =*##+*****=:::::-####+#%%:                         ",
        "               *#***#=+=--:.. .:==*++#*%%                         ",
        "              :##**+=-=-:-:.:----=**=%##*#:                       ",
        "            ..#*#+#+=:=--==-:..*%*+**+%*##***:.                   ",
        "        ..:-=#+*##==:===*=**+**%%#+++*#*#*+******=:..             ",
        "      .:--=#*=*%#*-=-+=*+#*#####*##******##++#*****#*--:          ",
        "     .-=#####*###*#=-*+=+#+%#%###%**##*+*#%#*##*****+#===-.       ",
        "    .-=*###*#######+=*#*++*=%#*##***#####+*+***##++****++#=:      ",
        "    :+***+###*###**#=+**#***+=##*++*#****#+************##*==-.    ",
        "      -==*#%#*#*#*##*+*+##=*++*+++****#***##**++++****##==#*=:    ",
        "     -++===***##***#*#***++++=+=*=++=+#**+==-=-----=-==---=:      ",
        "         *+=+++***##**#***##**+++******##++**=-------.            ",
        "        =======+*++****##**#*#*+*###*####+=++=====-----:          ",
        "       :------=-=-----=*+++**+=++*==*==+===----==----=+**         ",
        "        =+=+---====+====+=-----------==+++=+=----==--=            ",
        "           -=----=====--=--=-----------------=------::=           ",
        "           :=*+**==+*++*=--==------::::-+*=--.**+++=:             ",
        "                         -+==-.:+==:.                             ",
        "                                                                  ",
        "                                                                  ",
        "                                                                  "
    };
    return aparencia;
}

void RacaSlime::realizarDrops(Personagem* inimigo, Personagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal)
{
    int xpDrop = 35;
    int ouroDrop = 15;
    ControleDeDrops::relatarEProcessarXpOuro(jogadorAtual, xpDrop, ouroDrop, ouroTotal, xpTotal);

    for (int i = 0; i < 3; ++i) {
        jogadorAtual->obterInventario()->adicionarItem(FabricaDeItens::criarItem("Gosma acida"));
        itensObtidos.push_back("Gosma acida");
    }
    ControleDeDrops::relatarDropItem("Gosma acida", 3);
    
    if ((std::rand() % 100) < 30) 
    {
        jogadorAtual->obterInventario()->adicionarItem(FabricaDeItens::criarItem("Nucleo pegajoso"));
        ControleDeDrops::relatarDropItem("Nucleo pegajoso", 1);
        itensObtidos.push_back("Nucleo pegajoso");
    }
}

void RacaSlime::aoCausarDano(Personagem* atacante, Personagem* alvo, int danoCausado) {
    if (std::rand() % 100 < 15) {
        if (!alvo->possuiEfeito(EfeitoNomes::LENTIDAO)) {
            alvo->adicionarEfeito(std::make_unique<EfeitoLentidao>(3));
            std::cout << SimplificacoesAparencia::cor(Cor::MAGENTA) << ">> [PASSIVA SLIME]: Uma gosma esverdeada grudou e deixou " << alvo->obterNome() << " mais lento!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
        }
    }
}