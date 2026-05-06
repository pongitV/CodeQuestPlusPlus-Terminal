#include "Mago.h"

#include <iostream>
#include <memory>

#include "../Inventario/FabricaItens.h"
#include "../Utilidades/SimplificacoesAparencia.h"

std::string Mago::obterNomeClasse() const 
{
     return "Mago"; 
}

const std::vector<std::string>& Mago::obterAparenciaClasseMenu() const 
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

Atributos Mago::obterAtributosClasse() const
{
    return { 0, 5, 5, 3, 10, 15, 15 };
}

std::vector<std::unique_ptr<Item>> Mago::obterEquipamentoClasse() const 
{
    std::vector<std::unique_ptr<Item>> equipamentos;
    
    int quantidadePocoes = 3;
    for (int i = 0; i < quantidadePocoes; ++i) {
        equipamentos.push_back(FabricaItens::criarItem(ItemID::PocaoCura30));
    }
    
    equipamentos.push_back(FabricaItens::criarItem(ItemID::CajadoCristal));
    equipamentos.push_back(FabricaItens::criarItem(ItemID::BarreiraMagica));
    equipamentos.push_back(FabricaItens::criarItem(ItemID::Tunica));
    return equipamentos;
}

std::string Mago::obterNomePassivaClasse() const 
{ return "Foco arcano"; }
std::string Mago::obterDescricaoPassivaClasse() const 
{ return "Ataques ressoam (25% em area) ou causam +25% de dano em alvo unico."; }
std::string Mago::obterRecargaHabilidadeClasse() const 
{ return "Recarga: 3 turnos."; }

std::string Mago::obterNomeHabilidadeClasse() const 
{ return "Canalizacao arcana"; }
std::string Mago::obterDescricaoHabilidadeClasse() const 
{ return "Pula seu turno para se defender e dobra o dano no proximo turno. Recarga: 3 turnos."; }
void Mago::usarHabilidadeClasse(SistemaPersonagem* personagemUsuario, std::vector<SistemaPersonagem*>& /*listaDeInimigos*/) 
{
    int turnosRestantes = personagemUsuario->obterCooldown(HabilidadeID::EstrategiaArcana);
    if (turnosRestantes > 0) {
        std::cout << "\n" << SimplificacoesAparencia::margemCombate() << "[SISTEMA]: A habilidade " << obterNomeHabilidadeClasse() << " esta em recarga (" << turnosRestantes << " turnos)!\n";
        SimplificacoesAparencia::registrarLogBatalha("[SISTEMA]: A habilidade " + obterNomeHabilidadeClasse() + " esta em recarga (" + std::to_string(turnosRestantes) + " turnos)!");
        SimplificacoesAparencia::aguardarEnter();
        personagemUsuario->definirHabilidadeCancelada(true);
        return;
    }
    
    personagemUsuario->definirMultiplicador(2.0);
    personagemUsuario->adicionarEfeito(std::make_unique<EfeitoBuffAtributos>(2)); 
    personagemUsuario->definirCooldown(HabilidadeID::EstrategiaArcana, 4);
    
    Item* escudo = personagemUsuario->obterEscudo();
    if (escudo) {
        personagemUsuario->definirDefendendo(true);
        std::cout << SimplificacoesAparencia::margemCombate() << "[HABILIDADE]: Canalizacao arcana! Voce se defende com " << escudo->obterNomeItem() << " e prepara um ataque devastador (2x Dano)!\n";
        SimplificacoesAparencia::registrarLogBatalha("[HABILIDADE]: Canalizacao arcana! Voce se defende com " + escudo->obterNomeItem() + " e prepara um ataque devastador (2x Dano)!");
    } else {
        std::cout << SimplificacoesAparencia::margemCombate() << "[HABILIDADE]: Canalizacao arcana! Voce foca sua energia para um ataque devastador (2x Dano) no proximo turno!\n";
        SimplificacoesAparencia::registrarLogBatalha("[HABILIDADE]: Canalizacao arcana! Voce foca sua energia para um ataque devastador (2x Dano) no proximo turno!");
    }
}

int Mago::processarDanoPreAtaque(SistemaPersonagem* atacante, SistemaPersonagem* defensor, int danoBase, bool isAtacanteJogador, size_t qtdInimigos) {
    if (defensor == nullptr) return danoBase;
    if (!isAtacanteJogador || qtdInimigos <= 1) {
        int danoAumentado = static_cast<int>(danoBase * 1.25);
        std::cout << SimplificacoesAparencia::margemCombate() << SimplificacoesAparencia::cor(Cor::MAGENTA) << "[Foco Arcano]: Dano concentrado aumentado em 25%!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
        SimplificacoesAparencia::registrarLogBatalha("[Foco Arcano]: Dano concentrado aumentado em 25%!");
        return danoAumentado;
    }
    return danoBase;
}

void Mago::processarDanoPosAtaque(SistemaPersonagem* atacante, SistemaPersonagem* alvoAtual, SistemaPersonagem* defensorPrincipal, int danoBase, int danoPerfurante, const std::function<void(SistemaPersonagem*, SistemaPersonagem*, int, int)>& aplicarDano, bool isAtacanteJogador, bool isArea, bool& ativouPassiva) {
    if (isAtacanteJogador && !isArea && alvoAtual != defensorPrincipal && alvoAtual->obterVida() > 0) {
        if (!ativouPassiva) {
            int danoAreaMsg = static_cast<int>(danoBase * 0.25);
            std::cout << SimplificacoesAparencia::margemCombate() << SimplificacoesAparencia::cor(Cor::MAGENTA) << "[Foco Arcano]: A magia ressoa, causando " << danoAreaMsg << " de dano aos inimigos proximos!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
            SimplificacoesAparencia::registrarLogBatalha("[Foco Arcano]: A magia ressoa, causando " + std::to_string(danoAreaMsg) + " de dano aos inimigos proximos!");
            ativouPassiva = true;
        }
        int danoArea = static_cast<int>(danoBase * 0.25);
        int perfuranteArea = static_cast<int>(danoPerfurante * 0.25);
        aplicarDano(atacante, alvoAtual, danoArea, perfuranteArea);
    }
}

TipoAtaque Mago::obterTipoAtaque() const { return TipoAtaque::UNICO; }
bool Mago::habilidadeConsomeTurno() const { return true; }
