#include "Slime.h"
#include <iostream>
#include <cstdlib>
#include "../Sistemas/SistemaPersonagem.h"
#include "../Inventario/FabricaItens.h"
#include "../Utilidades/Tipos.h"
#include <memory>
#include "../Utilidades/SimplificacoesAparencia.h"
#include "../Gerenciadores/GerenciadorDrops.h"

std::string Slime::obterNomeRaca() const { return "Slime"; }
Atributos Slime::obterAtributosRaca() const { return { 50, 15, 5, 10, 15, 0, 0 }; }
std::string Slime::obterNomeHabilidadeRaca() const { return "Toque Gosmento"; }
std::string Slime::obterDescricaoHabilidadeRaca() const { return "Ataques tem 20% de chance de causar Lentidao"; }

std::vector<std::unique_ptr<Item>> Slime::obterEquipamentoRaca() const {
    std::vector<std::unique_ptr<Item>> equipamentos;
    equipamentos.push_back(FabricaItens::criarItem("Gosma acida (Arma)"));
    return equipamentos;
}

std::vector<std::string> Slime::obterAparenciaRaca() const
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

void Slime::realizarDrops(SistemaPersonagem* inimigo, SistemaPersonagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal)
{
    int xpDrop = 35;
    int ouroDrop = 15;
    GerenciadorDrops::relatarEProcessarXpOuro(jogadorAtual, xpDrop, ouroDrop, ouroTotal, xpTotal);

    for (int i = 0; i < 3; ++i) {
        jogadorAtual->obterInventario()->adicionarItem(FabricaItens::criarItem("Gosma acida"));
        itensObtidos.push_back("Gosma acida");
    }
    GerenciadorDrops::relatarDropItem("Gosma acida", 3);
    
    if ((std::rand() % 100) < 30) 
    {
        jogadorAtual->obterInventario()->adicionarItem(FabricaItens::criarItem("Nucleo pegajoso"));
        GerenciadorDrops::relatarDropItem("Nucleo pegajoso", 1);
        itensObtidos.push_back("Nucleo pegajoso");
    }
}

void Slime::aoCausarDano(SistemaPersonagem* atacante, SistemaPersonagem* alvo, int danoCausado) {
    if (std::rand() % 100 < 15) {
        if (!alvo->possuiEfeito(EfeitoNomes::LENTIDAO)) {
            alvo->adicionarEfeito(std::make_unique<EfeitoLentidao>(3));
            std::cout << SimplificacoesAparencia::cor(Cor::MAGENTA) << ">> [PASSIVA SLIME]: Uma gosma esverdeada grudou e deixou " << alvo->obterNome() << " mais lento!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
        }
    }
}