#include "Guerreiro.h"

#include <iostream>
#include <memory>

#include "../Inventario/FabricaItens.h"
#include "../Utilidades/Aparencia.h"
#include "../Telas/TelaCombate.h"

std::string Guerreiro::obterNomeClasse() const 
{ 
    return "Guerreiro"; 
}

const std::vector<std::string>& Guerreiro::obterAparenciaClasseMenu() const 
{
    static const std::vector<std::string> aparencia = 
    {
        "                                                          ",
        "                              *###:                       ",
        "                             #+=%##%                      ",
        "                             -::+###-                     ",
        "                             +*%%+++*..                   ",
        "                             +: ==-%%.                    ",
        "                            .-%#%@@%@@%                   ",
        "                             -##@@@=:.###                 ",
        "                          =:#*##%@#:+%@@%@@=              ",
        "                        #+#*@:.#%*%@-:==-=#@              ",
        "                        *%@=:#*-: :=@*+#@#=@              ",
        "                        =++#==+=#+##@::#*=#               ",
        "                        =#*@##@%+@%@@=:+=*#               ",
        "                         =%%#%@#@%@@@===*%#               ",
        "                        =#==:==%#@@@@@+==%#               ",
        "                        =#===::+-:-####-=#*#              ",
        "                       -+#.==+=:=%###==-##-@              ",
        "                      =+#.*#@####+%#%+:=###@              ",
        "                     =:-#+@#@#%@#*@#%=+#*=                ",
        "                    :#:==-##@#%====%@#=@#*                ",
        "                    =#*+#=+#@@@@@@@@*+@@-                 ",
        "                    %#%%#+#+%##%=@@#:=@@#+                ",
        "                   ++@##=@#==%@*#*=.:=#:+#                ",
        "                   %=@@####%*=@**@:%##*@%@                ",
        "                  .==%#*=%%#@+@*#%#@@@@@@@#               ",
        "                 -==###@##=#*@%%+##@@@@@@@@               ",
        "                 @%###@%#%++@%@@%#@%@@@@%@#               ",
        "                 %%  ####=@%#@#@@@*@##@@@@@@              ",
        "                ##  ##%#+%##@@#@%@##@#@@@@%@#             ",
        "               #%-   **@@#++@%#@@%*%%%@@@@=%%             ",
        "              =#* ++#*@+=@#+@%@*@#+@@@@%%*#               ",
        "              %#      @@@@%@##*@@%+**=@@@@#=@#            ",
        "             ##       #=@@%@@@@@@+=#*@@@@%#@#%#           ",
        "            ##         =*@@@@@@@@-=*=@@@@@#@*%#*          ",
        "           ###         ::@@@@@ @@=+##@@@@@#@##%@          ",
        "          %##           %#%####  #++#%@@@@#@%*#           ",
        "         #%@            %@@#@@:  **+#+@%@@#@%@%           ",
        "         #%:            =@#%%@=  = #% %@@@@*==@#          ",
        "        ##              =##%@%         @@@@#=##           ",
        "       *#               %%#@@#         %@@@@.             ",
        "       #             *%%%%#@%@*        @@@@@+             ",
        "                  +####%@%%###       @#%%@#@@             ",
        "                                    ##%*#@@%%             ",
        "                                                          "
    };
    return aparencia;
}

Atributos Guerreiro::obterAtributosClasse() const
{
    return { 0, 20, 10, 5, 10, 5, 5 };
}

std::vector<std::unique_ptr<Item>> Guerreiro::obterEquipamentoClasse() const 
{
    std::vector<std::unique_ptr<Item>> equipamentos;
    
    int quantidadePocoes = 3;
    for (int i = 0; i < quantidadePocoes; ++i) {
        equipamentos.push_back(FabricaItens::criarItem(ItemID::PocaoCura30));
    }

    equipamentos.push_back(FabricaItens::criarItem(ItemID::EspadaFerro));
    equipamentos.push_back(FabricaItens::criarItem(ItemID::EscudoMetal));
    equipamentos.push_back(FabricaItens::criarItem(ItemID::ArmaduraMalha));
    return equipamentos;
}

std::string Guerreiro::obterNomePassivaClasse() const 
{ return "Golpe decisivo"; }
std::string Guerreiro::obterDescricaoPassivaClasse() const 
{ return "Causa +10%/+20%/+30% de dano em inimigos com menos de 30%/20%/10% de HP."; }
std::string Guerreiro::obterRecargaHabilidadeClasse() const 
{ return "Recarga: 3 turnos."; }

std::string Guerreiro::obterNomeHabilidadeClasse() const 
{ return "Grito de guerra"; }
std::string Guerreiro::obterDescricaoHabilidadeClasse() const 
{ return "Gasta seu turno para aumentar Forca e Destreza em 1.5x por 2 turnos."; }
void Guerreiro::usarHabilidadeClasse(SistemaPersonagem* personagemUsuario, std::vector<SistemaPersonagem*>& /*listaDeInimigos*/) 
{
    int turnosRestantes = personagemUsuario->obterCooldown(HabilidadeID::Determinacao);
    if (turnosRestantes > 0) {
        std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: A habilidade " << obterNomeHabilidadeClasse() << " esta em recarga (" << turnosRestantes << " turnos)!\n";
        Aparencia::registrarLogBatalha("[SISTEMA]: A habilidade " + obterNomeHabilidadeClasse() + " esta em recarga (" + std::to_string(turnosRestantes) + " turnos)!");
        Aparencia::aguardarEnter();
        personagemUsuario->definirHabilidadeCancelada(true);
        return;
    }
    if (personagemUsuario->possuiEfeito(EfeitoID::GritoDeGuerra)) {
        std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: A habilidade " << obterNomeHabilidadeClasse() << " ja esta ativa!\n";
        Aparencia::registrarLogBatalha("[SISTEMA]: A habilidade " + obterNomeHabilidadeClasse() + " ja esta ativa!");
        Aparencia::aguardarEnter();
        personagemUsuario->definirHabilidadeCancelada(true);
        return;
    }

    int bonusForca = personagemUsuario->obterForca() / 2;
    int bonusDestreza = personagemUsuario->obterDestreza() / 2;
    
    personagemUsuario->adicionarEfeito(std::make_unique<EfeitoGritoGuerra>(2, bonusForca, bonusDestreza));
    personagemUsuario->definirCooldown(HabilidadeID::Determinacao, 4);
    
    std::cout << Aparencia::margemCombate() << "[HABILIDADE]: Grito de guerra! Forca +" << bonusForca << " e Destreza +" << bonusDestreza << "!\n";
    Aparencia::registrarLogBatalha("[HABILIDADE]: Grito de guerra! Forca +" + std::to_string(bonusForca) + " e Destreza +" + std::to_string(bonusDestreza) + "!");
}

int Guerreiro::processarDanoPreAtaque(SistemaPersonagem* atacante, SistemaPersonagem* defensor, int danoBase, bool isAtacanteJogador, size_t qtdInimigos) {
    int danoFinal = danoBase;
    
    if (defensor != nullptr) {
        double percVida = (double)defensor->obterVida() / defensor->obterVidaMaxima();
        if (percVida < 0.10) {
            danoFinal = static_cast<int>(danoFinal * 1.30);
            std::string msg = Aparencia::margemCombate() + Aparencia::cor(Cor::VERMELHO) + "[Golpe Decisivo]: O inimigo esta nas ultimas! Dano aumentado em 30%!" + Aparencia::cor(Cor::RESET) + "\n";
            std::cout << msg;
            TelaCombate::adicionarMensagemFixa(msg);
            Aparencia::registrarLogBatalha("[Golpe Decisivo]: O inimigo esta nas ultimas! Dano aumentado em 30%!");
        } else if (percVida < 0.20) {
            danoFinal = static_cast<int>(danoFinal * 1.20);
            std::string msg = Aparencia::margemCombate() + Aparencia::cor(Cor::VERMELHO) + "[Golpe Decisivo]: O inimigo esta gravemente ferido! Dano aumentado em 20%!" + Aparencia::cor(Cor::RESET) + "\n";
            std::cout << msg;
            TelaCombate::adicionarMensagemFixa(msg);
            Aparencia::registrarLogBatalha("[Golpe Decisivo]: O inimigo esta gravemente ferido! Dano aumentado em 20%!");
        } else if (percVida < 0.30) {
            danoFinal = static_cast<int>(danoFinal * 1.10);
            std::string msg = Aparencia::margemCombate() + Aparencia::cor(Cor::VERMELHO) + "[Golpe Decisivo]: O inimigo esta ferido! Dano aumentado em 10%!" + Aparencia::cor(Cor::RESET) + "\n";
            std::cout << msg;
            TelaCombate::adicionarMensagemFixa(msg);
            Aparencia::registrarLogBatalha("[Golpe Decisivo]: O inimigo esta ferido! Dano aumentado em 10%!");
        }
    }
    
    return danoFinal;
}

TipoAtaque Guerreiro::obterTipoAtaque() const { return TipoAtaque::UNICO; }
bool Guerreiro::habilidadeConsomeTurno() const { return true; }
