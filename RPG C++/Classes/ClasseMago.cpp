#include <iostream>
#include <memory>

#include "ClasseMago.h"
#include "../Inventario/FabricaDeItens.h"
#include "../Sistema/SimplificacoesAparencia.h"

std::string ClasseMago::obterNomeClasse() const 
{
     return "Mago"; 
}

std::vector<std::string> ClasseMago::obterAparenciaClasseMenu() const 
{
    static const std::vector<std::string> aparencia = 
    {
        "                                        =@@@%%   ",
        "                                        *%%***.  ",
        "                                        **#- .#  ",
        "                                       #%%%##+#: ",
        "                   -#++                .%%@%-    ",
        "                  ##*+:-=              *@@@@.    ",
        "                 #%#*+:-*.              %@@@*    ",
        "                *%*%##%++*             .@@+      ",
        "               :#@%**.+*+#*            *@@%      ",
        "   : .         *##*@#*-+#:##           :@@.      ",
        "   ::* +    .#%%%%:#@=*+-#*#+*+        =@@-      ",
        "    -=+-   @%#%@@@+*@:*==%-++-::*      *%*       ",
        "    -*-* :@%#%****+*%+**++** --:-*     %@        ",
        "    +-* @#%*#*-=+*-*%+*#**:+::*-#@     *@.       ",
        "    @#*+ **@%==:*:=:%-*+#***:=@+::=    %@        ",
        "    %#*.*#*%@%@#+=-*=--##**%*#@%*::    *@        ",
        "    @@*:=#%%@+@%:**:-**%*=+-#@@@%*::-  #@        ",
        "    @@*=**@@@**+***+:#%*--+**.%@@@*-::-@*        ",
        "    @@*+##@@@: %##+*%#***=***: @@@+*-* ::        ",
        "    @@**%@@@:  @%*%@%*+%%+*#-  @@@@#:%%**+       ",
        "    @@#%@@@@  +@@*%@#++%%=#%* .@@@@*@###         ",
        "    @@%@@@@   @@%%@***#@%+#%#   #@@@@@@@*        ",
        "   :@@#@@.    @@@@@*#*%%#*%%#    @@@@@@@=        ",
        "   =@%%:    -%@@%@%*#=*+*#%%%:   =@@@@@@         ",
        "   =%%  %@%##@@@@%@*#*#*%+#%@#    %@@@@@         ",
        "   *:  =@@%@@#*#%%###%%%@@#%@@.    %@@@@         ",
        "      -@@@@%@@%###*%%@@*%@@*%%@     @@@@         ",
        "        *@@@@%##%%%%*%@%@#%%%%%@     @@          ",
        "          #@#%##@%%%%@@@@#@%%@%@*    @+          ",
        "          @@%@@#@%%%%%#@@@%%@%%@@@:              ",
        "         %@@%@@%@@@@%@@@@@%@@%@@%*@              ",
        "        @@@@%@@%@@%@%%@@@@@@@%@%@@*@             ",
        "        @@@@@@@@@@@@@@@@@@@@@%@@@@@@             ",
        "        @@@@@@@@@@@@@@@@@@@@@%%@%@@%*            ",
        "        @@@@@@@@@@@@@@@@@@@  .%@%@%%#            ",
        "        @%%@@@@@ +@@@@@@@@.   *@%@%%#            ",
        "         @@@@%@@: @@@@@@@.    :%@%%%             ",
        "         #@@@%@@* @@@@%@@      %@%%              ",
        "            #@@@%  %@@%@%@     %@%#              ",
        "             *@@%  @%%%%@@     %@%               ",
        "               +% .@%@%@@%     *#                ",
        "                   %%%#                          ",
        "                  @%%%%                          "
    };
    return aparencia;
}

Atributos ClasseMago::obterAtributosClasse() const
{
    return { 0, 5, 5, 3, 10, 15, 15 };
}

std::vector<std::unique_ptr<Item>> ClasseMago::obterEquipamentoClasse() const 
{
    std::vector<std::unique_ptr<Item>> equipamentos;
    
    int quantidadePocoes = 3;
    int porcentagemCura = 30;
    for (int i = 0; i < quantidadePocoes; ++i) {
        equipamentos.push_back(FabricaDeItens::criarItem("Pocao de Cura (" + std::to_string(porcentagemCura) + "%VM)"));
    }
    
    equipamentos.push_back(FabricaDeItens::criarItem("Cajado"));
    equipamentos.push_back(FabricaDeItens::criarItem("Barreira magica"));
    equipamentos.push_back(FabricaDeItens::criarItem("Tunica"));
    return equipamentos;
}

std::string ClasseMago::obterNomePassivaClasse() const 
{ return "Foco arcano"; }
std::string ClasseMago::obterDescricaoPassivaClasse() const 
{ return "Ataques ressoam (25% em area) ou causam +25% de dano em alvo unico."; }
std::string ClasseMago::obterRecargaHabilidadeClasse() const 
{ return "Recarga: 3 turnos."; }

std::string ClasseMago::obterNomeHabilidadeClasse() const 
{ return "Canalizacao arcana"; }
std::string ClasseMago::obterDescricaoHabilidadeClasse() const 
{ return "Pula seu turno para se defender e dobra o dano no proximo turno. Recarga: 3 turnos."; }
void ClasseMago::usarHabilidadeClasse(Personagem* u, std::vector<Personagem*>& /*inimigos*/) 
{
    if (u->obterCooldown("EstrategiaArcana") > 0) {
        std::cout << "[SISTEMA]: Canalizacao arcana em recarga!\n";
        u->definirHabilidadeCancelada(true);
        return;
    }
    
    u->definirMultiplicador(2.0);
    u->adicionarEfeito(std::make_unique<EfeitoBuffAtributos>(2)); 
    u->definirCooldown("EstrategiaArcana", 4);
    
    Item* escudo = u->obterEscudo();
    if (escudo) {
        u->definirDefendendo(true);
        std::cout << "[HABILIDADE]: Canalizacao arcana! Voce se defende com " << escudo->obterNomeItem() << " e prepara um ataque devastador (2x Dano)!\n";
    } else {
        std::cout << "[HABILIDADE]: Canalizacao arcana! Voce foca sua energia para um ataque devastador (2x Dano) no proximo turno!\n";
    }
}

void ClasseMago::executarAtaqueComPassivaDaClasse(Personagem* atacante, Personagem* defensor, int danoBase, int danoPerfurante, std::vector<std::unique_ptr<Personagem>>& inimigos, std::function<void(Personagem*, Personagem*, int, int)> aplicarDano) 
{
    if (defensor == nullptr) return;

    bool isAtacanteJogador = true;
    for (const auto& ini : inimigos) {
        if (ini.get() == atacante) { isAtacanteJogador = false; break; }
    }

    if (isAtacanteJogador && inimigos.size() > 1) {
        std::cout << atacante->obterNome() << " ataca " << defensor->obterNome() << "!\n";
        aplicarDano(atacante, defensor, danoBase, danoPerfurante);
        
        int danoArea = static_cast<int>(danoBase * 0.25);
        int perfuranteArea = static_cast<int>(danoPerfurante * 0.25);
        std::cout << SimplificacoesAparencia::cor(Cor::MAGENTA) << "[Foco Arcano]: A magia ressoa, causando " << danoArea << " de dano aos inimigos proximos!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
        
        for (auto& ini : inimigos) {
            if (ini.get() != defensor && ini->obterVida() > 0) {
                aplicarDano(atacante, ini.get(), danoArea, perfuranteArea);
            }
        }
    } else {
        int danoAumentado = static_cast<int>(danoBase * 1.25);
        std::cout << SimplificacoesAparencia::cor(Cor::MAGENTA) << "[Foco Arcano]: Dano concentrado aumentado em 25%!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
        std::cout << atacante->obterNome() << " ataca " << defensor->obterNome() << "!\n";
        aplicarDano(atacante, defensor, danoAumentado, danoPerfurante);
    }
}

TipoAtaque ClasseMago::obterTipoAtaque() const { return TipoAtaque::UNICO; }
bool ClasseMago::habilidadeConsomeTurno() const { return true; }
