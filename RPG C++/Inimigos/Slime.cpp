#include "Slime.h"
#include <iostream>
#include "../Sistemas/SistemaPersonagem.h"
#include "../Inventario/FabricaItens.h"
#include <memory>
#include "../Utilidades/Aparencia.h"
#include "../Gerenciadores/GerenciadorDrops.h"
#include "../Utilidades/GeradorAleatorio.h"
#include "../Telas/TelaCombate.h"

// --- INFORMACOES DA RACA ---
std::string Slime::obterNomeRaca() const 
{ 
    return "Slime"; 
}

Atributos Slime::obterAtributosRaca() const 
{ 
    return { 150, 15, 5, 10, 15, 0, 0 }; 
}

std::vector<std::unique_ptr<Item>> Slime::obterEquipamentoRaca() const 
{
    std::vector<std::unique_ptr<Item>> equipamentos;
    equipamentos.push_back(FabricaItens::criarItem(ItemID::GosmaAcidaArma));
    return equipamentos;
}

// --- HABILIDADE DA RACA ---
std::string Slime::obterNomeHabilidadeRaca() const { return "Toque Gosmento"; }
std::string Slime::obterDescricaoHabilidadeRaca() const { return "Ataques tem 20% de chance de causar Lentidao"; }

// --- APARENCIA ---
const std::vector<std::string>& Slime::obterAparenciaRaca() const
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

// --- BESTIARIO E DROPS ---
InfoBestiario Slime::obterInfoBestiario() const {
    return {
        "Floresta", 
        "Superficie",
        "Massas gelatinosas que absorvem tudo o que tocam.",
        "Slimes podem digerir materiais em dias, mas detestam sal.",
        {FabricaItens::obterNomeDeID(ItemID::GosmaAcida), FabricaItens::obterNomeDeID(ItemID::NucleoPegajoso), "Ouro"},
        2
    };
}

void Slime::realizarDrops(SistemaPersonagem* inimigo, SistemaPersonagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal)
{
    GerenciadorDrops::relatarEProcessarXpOuro(jogadorAtual, 35, 15, ouroTotal, xpTotal);

    GerenciadorDrops::darEProcessarItem(jogadorAtual, ItemID::GosmaAcida, 3, itensObtidos);
    GerenciadorDrops::darEProcessarItem(jogadorAtual, ItemID::NucleoPegajoso, 1, itensObtidos, 30);
}

// --- PROCESSAMENTO DE DANO  ---
void Slime::aoCausarDano(SistemaPersonagem* atacante, SistemaPersonagem* alvo, int danoCausado) {
    if (GeradorAleatorio::rolarChance(15)) {
        if (!alvo->possuiEfeito(EfeitoID::Lentidao)) {
            alvo->adicionarEfeito(std::make_unique<EfeitoLentidao>(3));
            // A mensagem na UI foi removida para priorizar o combate limpo
            // std::string msg = Aparencia::margemCombate() + Aparencia::cor(Cor::MAGENTA) + ">> [PASSIVA SLIME]: Uma gosma esverdeada grudou e deixou " + alvo->obterNome() + " mais lento!" + Aparencia::cor(Cor::RESET) + "\n";
            // TelaCombate::adicionarMensagemFixa(msg);
            Aparencia::registrarLogBatalha(Aparencia::cor(Cor::MAGENTA) + ">> [PASSIVA SLIME]: Uma gosma esverdeada grudou e deixou " + alvo->obterNome() + " mais lento!" + Aparencia::cor(Cor::RESET));
        }
    }
}