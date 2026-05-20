#include "Guerreiro.h"

#include <iostream>
#include <memory>

#include "../../Sistemas/Inventario/FabricaItens.h"
#include "../../Core/Utilidades/Aparencia.h"
#include "../../Core/Utilidades/FuncoesDialogo.h"
#include "../../Interface/Telas/Combate/TelaCombate.h"

// --- INFORMACOES DA CLASSE ---
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
    auto equipamentos = FabricaItens::criarKitPocoes();

    equipamentos.push_back(FabricaItens::criarItem(ItemID::EspadaFerro));
    equipamentos.push_back(FabricaItens::criarItem(ItemID::EscudoMetal));
    equipamentos.push_back(FabricaItens::criarItem(ItemID::ArmaduraMalha));
    return equipamentos;
}

// --- PASSIVA DA CLASSE ---
std::string Guerreiro::obterNomePassivaClasse() const 
{ 
    return "Golpe decisivo"; 
}

std::string Guerreiro::obterDescricaoPassivaClasse() const 
{ 
    return "Causa +10%/+20%/+30% de dano em inimigos com menos de 30%/20%/10% de HP."; 
}

// --- HABILIDADE DA CLASSE ---
std::string Guerreiro::obterRecargaHabilidadeClasse() const 
{ 
    return "Recarga: 3 turnos."; 
}

std::string Guerreiro::obterNomeHabilidadeClasse() const 
{ 
    return "Grito de guerra"; 
}

std::string Guerreiro::obterDescricaoHabilidadeClasse() const 
{ 
    return "Gasta seu turno para aumentar Forca e Destreza em 1.5x por 2 turnos."; 
}

void Guerreiro::usarHabilidadeClasse(Personagem* personagemUsuario, std::vector<Personagem*>& /*listaDeInimigos*/) 
{
    int turnosRestantes = personagemUsuario->obterCooldown(HabilidadeID::Determinacao);
    if (verificarEReportarRecarga(personagemUsuario, turnosRestantes, obterNomeHabilidadeClasse())) return;

    if (personagemUsuario->possuiEfeito(EfeitoID::GritoDeGuerra)) {
        std::string msg = FuncoesDialogo::formatarMsgSistema("A habilidade " + obterNomeHabilidadeClasse() + " ja esta ativa!", Cor::AMARELO);
        std::cout << "\n" << Aparencia::margemCombate() << msg << "\n";
        Aparencia::registrarLogBatalha(msg);
        ControleDeInput::aguardarEnter();
        personagemUsuario->definirHabilidadeCancelada(true);
        return;
    }

    int bonusForca = personagemUsuario->obterForca() / 2;
    int bonusDestreza = personagemUsuario->obterDestreza() / 2;
    
    personagemUsuario->adicionarEfeito(std::make_unique<EfeitoGritoGuerra>(2, bonusForca, bonusDestreza));
    personagemUsuario->definirCooldown(HabilidadeID::Determinacao, 4);
    
    std::string msg = FuncoesDialogo::formatarMsgHabilidade("Grito de guerra! Forca +" + std::to_string(bonusForca) + " e Destreza +" + std::to_string(bonusDestreza) + "!");
    notificarMensagemCombate(msg, msg);
}

// --- PROCESSAMENTO DE DANO  ---
int Guerreiro::processarDanoPreAtaque(Personagem* atacante, Personagem* defensor, int danoBase, bool isAtacanteJogador, size_t qtdInimigos) {
    int danoFinal = danoBase;
    
    if (!defensor) return danoFinal;

    double percVida = (double)defensor->obterVida() / defensor->obterVidaMaxima();
    int bonus = 0;
    std::string estado = "";
    
    if (percVida < 0.10) { bonus = 30; estado = "nas ultimas"; }
    else if (percVida < 0.20) { bonus = 20; estado = "gravemente ferido"; }
    else if (percVida < 0.30) { bonus = 10; estado = "ferido"; }
    
    if (bonus > 0) {
        danoFinal = static_cast<int>(danoFinal * (1.0 + bonus / 100.0));
        std::string textoLog = FuncoesDialogo::formatarMsgHabilidade("Golpe Decisivo: O inimigo esta " + estado + "! Dano aumentado em " + std::to_string(bonus) + "%!", Cor::VERMELHO);
        notificarMensagemCombate(textoLog, textoLog);
    }
    
    return danoFinal;
}
