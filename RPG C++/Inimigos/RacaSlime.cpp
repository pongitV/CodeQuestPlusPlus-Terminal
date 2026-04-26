#include "RacaSlime.h"
#include <iostream>
#include <cstdlib>
#include "../Sistema/Personagem.h"
#include "../Inventario/Item.h"
#include "../Inventario/Material.h"
#include "../Inventario/Arma.h"
#include "../Sistema/Tipos.h"
#include <memory>

std::string RacaSlime::obterNomeRaca() const { return "Slime"; }
Atributos RacaSlime::obterAtributosRaca() const { return { 50, 15, 5, 10, 15, 0, 0 }; }
std::string RacaSlime::obterNomeHabilidadeRaca() const { return "Toque Gosmento"; }
std::string RacaSlime::obterDescricaoHabilidadeRaca() const { return "Ataques tem 20% de chance de causar Lentidao"; }

std::vector<std::unique_ptr<Item>> RacaSlime::obterEquipamentoRaca() const {
    std::vector<std::unique_ptr<Item>> equipamentos;
    equipamentos.push_back(std::make_unique<Arma>("Gosma acida", 2, 7));
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
    jogadorAtual->ganharXp(xpDrop);
    jogadorAtual->ganharOuro(ouroDrop);
    xpTotal += xpDrop;
    ouroTotal += ouroDrop;
    
    std::cout << "\033[43m+" << ouroDrop << "G\033[0m \033[44m+" << xpDrop << " XP\033[0m\n";

    for (int i = 0; i < 3; ++i) {
        jogadorAtual->obterInventario()->adicionarItem(std::make_unique<Material>("Gosma acida"));
        itensObtidos.push_back("Gosma acida");
    }
    std::cout << "\033[37m+3x Gosma acida\033[0m\n";
    
    if ((std::rand() % 100) < 30) 
    {
        jogadorAtual->obterInventario()->adicionarItem(std::make_unique<Material>("Nucleo pegajoso"));
        std::cout << "\033[37m+Nucleo pegajoso\033[0m\n";
        itensObtidos.push_back("Nucleo pegajoso");
    }
}

void RacaSlime::aoCausarDano(Personagem* atacante, Personagem* alvo, int danoCausado) {
    if (std::rand() % 100 < 15) {
        if (!alvo->possuiEfeito(EfeitoNomes::LENTIDAO)) {
            alvo->adicionarEfeito(std::make_unique<EfeitoLentidao>(3));
            std::cout << "\033[35m>> [PASSIVA SLIME]: Uma gosma esverdeada grudou e deixou " << alvo->obterNome() << " mais lento!\033[0m\n";
        }
    }
}