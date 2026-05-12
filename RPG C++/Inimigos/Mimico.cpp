#include "Mimico.h"
#include <iostream>
#include "../Sistemas/SistemaPersonagem.h"
#include "../Inventario/FabricaItens.h"
#include "../Utilidades/Aparencia.h"
#include "../Gerenciadores/GerenciadorDrops.h"
#include "../Utilidades/GeradorAleatorio.h"
#include "../Telas/TelaCombate.h"
#include "../Sistemas/SistemaBestiario.h"

// --- INFORMACOES DA RACA ---
std::string Mimico::obterNomeRaca() const { return "Mimico"; }

Atributos Mimico::obterAtributosRaca() const { 
    return { 300, 15, 0, 15, 20, 0, 0 }; 
}

std::vector<std::unique_ptr<Item>> Mimico::obterEquipamentoRaca() const {
    return {};
}

// --- HABILIDADE DA RACA ---
std::string Mimico::obterNomeHabilidadeRaca() const { return "Bolsos Furados"; }
std::string Mimico::obterDescricaoHabilidadeRaca() const { return "Rouba de 30 a 80 de ouro do alvo a cada ataque bem-sucedido."; }

// --- APARENCIA ---
const std::vector<std::string>& Mimico::obterAparenciaRaca() const
{
    static const std::vector<std::string> aparencia =
    {
        "                               .: .       .+-##-=%          ..:*#+###*        ",
        "                           .%+###-#-%##*#+-=-=%+*%#-%        ::*##*###*       ",
        "                      .-====%++%%%%%#####+=%#%%@#*%%@%@-::.  .:-*######       ",
        "               *%++*--+*#%@@@%##+#%%%@#*%%#%#%%%@%*@@@@@@%%%=.::*##**##:      ",
        "              :#%##*##*%#%#:=#%%#%#*%%-#@%**#@%#%%%%%%@%%%%%%**:+###*#*-      ",
        "              -##**#%%##%%%%%#@#*%#=%*.=@@=:*@%#@##%%#@%%@%@%+#-*######.      ",
        "             :-#@@##%%%%%%%#*=%*+##*##+#%##.*%=+%=+@%@%#%%%@%*+=+######       ",
        "             :-#%%####@##%%*%#%%+%@+**@@*#@#@#-*%#%*%%*#@##@*#=+*%####.       ",
        "             .=#@%%%%#%*#%#%#%%@#%@@%#@@##@@@+%%%%*#%##@%#@##++*###*#=.       ",
        "              =+%%@@%#%##%#%@@@@@%%@@%@@@@@@@@@%@@@@%%@%@@@%++*#####= .       ",
        "              =*@@%%%%%@@%@%@@@@@%@%%@%@%@@@@@@@@@@@%@%@@@%++####%=. ..       ",
        "              -##%@%%%%@@@%%@@@@@@@@@@@@@@@@@@@@@@@%%@@%%++*#%#%@+:. .        ",
        "                 -#%#%@@@@%@@@@@@@@@@@@@@@@@@@@@@@@%%@*++##%%%@* +..          ",
        "                 .+@#%@@@@@%@@@@@@@@@%@@@@@@@@@@@@#==**#%%%%%+    . .         ",
        "                  .@%#%%@@@%@@@@@@@@@@@@@@@@@@#=:=*#%%@%%%%=       .          ",
        "                 .:%%#%@@@@%@@@@@@@@@@@@@@#+:=+*#%%@@@@@%%*       .           ",
        "                  :#%%+%@@@%@@@@@@@@@@@#+==**#%@@@@%@@@@%%+.    .             ",
        "                  -%####%@@%@@@@@@@@@#*=*##%@@@@%%%@@@@%%#*                   ",
        "                  =####%#@@@@@@@@@@%**+#%%@@@@%%%%@@@@@%%##-    .             ",
        "                 :##%%%%%#@@%%%@@@#**#%@@@@@@%%%%@@%@%@%%%%*-                 ",
        "                 +*%%%#%@%#%#%@@@#*#%@@@@@%%%@@%%@@%@%%%@@%#*+                ",
        "                =*+*#%%@@@%**%%%##%%@@@@%%%%@@%%@@@@@@%%@%%%%%#+++-           ",
        "               .#%#*#%%@@@%%#*#%%%@@@@@%%@%%@@@%%%%@@@@@%%@%@@%**##           ",
        "             .+#%%*#%%#@@@@%%%%%%@@@@@@%%%%@@@@%%@@@@@@%%%%@#**%%%-           ",
        "            =**@%:##-=#@%#@@@%@@@@@@@@@%%%@@@@@@@@@%%%%%#%##%@%%%%            ",
        "           -##@@##%**%%**#@@=%@@*@@%=@@@*#@@@#*%@@%#%%*%%@@%%@@%%%            ",
        "         ........::-+*-#%%%=*#%-#%@+=*@@++#%%#+#%%##%#*%%%@@@@@%%.            ",
        "        .%%%%%#####%##*#-##****+%#*+*#:##+#*#%#%%%#%%@%@@%@@@@@%@             ",
        "         .%#%@@@@@@@@%%#=*%%+#*##+=%##-##*%%#%%%%#%@@@@@%@@@@@@%+             ",
        "          .#%*@%%%%%%%%@**%@+##%%###@@**%@@%%%%%%%%@@@@@@@@@@@%@              ",
        "           .##*@@%%%%@@@%#%%@#%%%@#%@@%#@@@@#%%@%%%@@@@@@@@@@@%*.+-..         ",
        "             %#*@%%%%%%%%%%%@@%%%%%%%%##%@@%%%@@@@@@@@@@@@@@%%%###@%#. .      ",
        "          ++*.*##@%@@@@@@%@@@@@@%@%@%@%%@@@@@@@@@@@@@@@@@@@@@%%%##%%%#-..     ",
        "        ===#=+*+%#%%%%@@@@@@@@@@@%@%%%%%@@@@@@@%@@@@@@@@@@@@###%@@%##%+::..   ",
        "         ...=*=+#%%%@@@@@@@@@@@@@%@@@@@@@@@@@@@%@@@@@@@@%%%#%%%%%%%%%###*:.   ",
        "              +%#%%#%#******************######%%%%%%%%%##%%%%@%%%%@%%@@@%%#-. ",
        "    @#+#%   -=#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%##%%%%%%#%@@%%%%@%%%%#- ",
        "     .::.   %%%%*=+*#####################%#%#########%%%%@@%#%@%#*%###%%%@%%*.",
        "                 .-===+++++**********%%%@%#*******###%%***##%%%%%%#*+++*=-:.  "
    };
    return aparencia;
}

// --- LOGICA DE ROUBO ---
void Mimico::aoCausarDano(SistemaPersonagem* atacante, SistemaPersonagem* alvo, int danoCausado) {
    if (danoCausado <= 0) return;
    
    int ouroAtual = alvo->obterInventario()->obterOuro();
    if (ouroAtual <= 0) return; // Jogador já está pobre

    int roubo = GeradorAleatorio::obterInteiro(30, 80);
    if (roubo > ouroAtual) roubo = ouroAtual; // Não pode roubar mais do que o jogador tem
    
    alvo->obterInventario()->adicionarOuro(-roubo);
    ouroRoubadoTotal += roubo;
    
    // A mensagem na UI foi removida para priorizar o combate limpo
    // std::string msg = Aparencia::margemCombate() + Aparencia::cor(Cor::AMARELO) + ">> [MIMICO]: Com uma lingua grotesca, o Mimico roubou " + std::to_string(roubo) + "G do seu bolso!" + Aparencia::cor(Cor::RESET) + "\n";
    // TelaCombate::adicionarMensagemFixa(msg);
    Aparencia::registrarLogBatalha(Aparencia::cor(Cor::AMARELO) + ">> [MIMICO]: Com uma lingua grotesca, o Mimico roubou " + std::to_string(roubo) + "G do seu bolso!" + Aparencia::cor(Cor::RESET));
}

// --- BESTIARIO E DROPS ---
InfoBestiario Mimico::obterInfoBestiario() const {
    return {
        "Masmorras e Cavernas", 
        "Dentro de Baus",
        "Uma criatura magica horrenda que se disfarca de baus de tesouro para devorar aventureiros gananciosos.",
        "Mimicos digerem carne rapidamente, mas cospem os metais preciosos.",
        {"Armadura de bau", "Ouro", "Ouro Extra (Ouro roubado retornado)", FabricaItens::obterNomeDeID(ItemID::PocaoCura30)},
        4
    };
}

void Mimico::realizarDrops(SistemaPersonagem* inimigo, SistemaPersonagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal)
{
    GerenciadorDrops::relatarEProcessarXpOuro(jogadorAtual, 150, 120 + ouroRoubadoTotal, ouroTotal, xpTotal);

    if (ouroRoubadoTotal > 0) {
         Aparencia::registrarLogBatalha(Aparencia::cor(Cor::AMARELO) + ">> Voce recuperou " + std::to_string(ouroRoubadoTotal) + "G que haviam sido roubados!" + Aparencia::cor(Cor::RESET));
         SistemaBestiario::instancia().registrarDrop(inimigo->obterNome(), "Ouro Extra (Ouro roubado retornado)");
    }

    GerenciadorDrops::darEProcessarItem(jogadorAtual, ItemID::PocaoCura30, GeradorAleatorio::obterInteiro(1, 2), itensObtidos);
    GerenciadorDrops::darEProcessarItem(jogadorAtual, ItemID::ArmaduraBau, 1, itensObtidos);
}
