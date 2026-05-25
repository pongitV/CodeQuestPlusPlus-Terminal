#include "Arqueiro.h"

#include <iostream>
#include <memory>

#include "../../Sistemas/Combate/Combate.h"
#include "../../Sistemas/Inventario/FabricaItens.h"
#include "../../Core/Utilidades/Aparencia.h"
#include "../../Core/Utilidades/FuncoesDialogo.h"

// --- INFORMACOES DA CLASSE ---
std::string Arqueiro::obterNomeClasse() const 
{
     return "Arqueiro"; 
}

const std::vector<std::string>& Arqueiro::obterAparenciaClasseMenu() const 
{
    static const std::vector<std::string> aparencia = 
    {
        "                  = --%=-                        ",
        "                    #@* @=                       ",
        "                       %@- %*                    ",
        "                         *% *                    ",
        "                         ** *                    ",
        "                          * **                   ",
        "                         +*%*#:*                 ",
        "                  ..      .%:#@%#:               ",
        "               : =-. .       ---=**.             ",
        "            :%#.:*+::::        #  %*             ",
        "           *@#+-:::*.- - #+    %*++**            ",
        "            =:+=*#+=::+#**%*:  #+*..+.           ",
        "            +#*+*%*%*%*+%*=%@#*+*#++ .           ",
        "        #@*::+*-**++**++*=#**==+*++--:           ",
        "       %@###%*#%%:#*%%#@#%@%%#%*:#*+=-+          ",
        "        =%@%@%:%#**####@@%% %@%%   #%.-#         ",
        "               #*%*##%#@%%-##        %*:-#       ",
        "                #%*#%%@%###$          **@%**     ",
        "                *#%%@%@%#*#            -=-=      ",
        "                 #+%%%*%%%#            -@-#      ",
        "                 -%:##*%#%#=%%* * %+    --=      ",
        "      +-=-===-+%==#*@=*%*@#**::##=**.#@@  ==     ",
        "    ++*=******##*%%**%*+#***+##*%# %##@@#        ",
        "      **+*****#*#%#*%#@##*:+#%---==.%%@*         ",
        "              *@#*%#*#+**:*%**                   ",
        "             %%%%##%%#+=%@%%%=                   ",
        "           *@@%%@%%:    %%%**=* *# .%            ",
        "          +*%@@%@        %%%*+* @# @+            ",
        "          #*=@@%%@@@      @%#*#* @* #            ",
        "           %#@@**@@@@#%    %## +=    @           ",
        "                  @.@*@     ##-:+=*              ",
        "                    @@#     %##%#**              ",
        "                    #*      *#@@@%%.             ",
        "                              +@@%%@             ",
        "                                @@%%             ",
        "                                 @%%             ",
        "                                  @%%            ",
        "                                  -%#%           ",
        "                                   %%%%          ",
        "                                    #:%          ",
        "                                    ##%%         ",
        "                                    *=:*         ",
        "                                     *:*         "
    };
    return aparencia;
}

Atributos Arqueiro::obterAtributosClasse() const
{
    return { 0, 10, 20, 3, 10, 5, 5 };
}

std::vector<std::unique_ptr<Item>> Arqueiro::obterEquipamentoClasse() const 
{
    auto equipamentos = FabricaItens::criarKitPocoes();

    equipamentos.push_back(FabricaItens::criarItem(ItemID::ArcoMadeira));
    equipamentos.push_back(FabricaItens::criarItem(ItemID::BracedeirasPrata));
    equipamentos.push_back(FabricaItens::criarItem(ItemID::ArmaduraCouro));
    return equipamentos;
}

// --- PASSIVA DA CLASSE ---
std::string Arqueiro::obterNomePassivaClasse() const 
{ 
    return "Passos leves"; 
}

std::string Arqueiro::obterDescricaoPassivaClasse() const 
{ 
    return "Penalidade de armaduras e debuffs de lentidao reduzidos pela metade."; 
}

int Arqueiro::processarPenalidadeArmaduraPassivaArqueiro(int penalidadeBase) const 
{
    return penalidadeBase / 2;
}

int Arqueiro::aplicarPenalidadeLentidaoPassivaArqueiro(int destrezaAtual) const 
{
    return (destrezaAtual * 3) / 4;
}

int Arqueiro::reverterPenalidadeLentidaoPassivaArqueiro(int destrezaAtual) const 
{
    return (destrezaAtual * 4) / 3;
}

// --- HABILIDADE DA CLASSE ---
std::string Arqueiro::obterRecargaHabilidadeClasse() const 
{ 
    return "Recarga: 1 turno."; 
}

std::string Arqueiro::obterNomeHabilidadeClasse() const 
{ 
    return "Retirada com pontaria"; 
}

std::string Arqueiro::obterDescricaoHabilidadeClasse() const 
{ 
    return "Se afasta durante um turno, no proximo turno causa 2x dano"; 
}

void Arqueiro::usarHabilidadeClasse(Combate* /*combate*/, Personagem* personagemUsuario, std::vector<Personagem*>& /*listaDeInimigos*/) 
{
    int turnosRestantes = personagemUsuario->obterCooldown(HabilidadeID::RetiradaComPontaria);
    if (verificarEReportarRecarga(personagemUsuario, turnosRestantes, obterNomeHabilidadeClasse())) return;

    personagemUsuario->adicionarEfeito(std::make_unique<EfeitoInviolavel>(1));
    personagemUsuario->definirCooldown(HabilidadeID::RetiradaComPontaria, 2);
    std::string msg = FuncoesDialogo::formatarMsgHabilidade("Retirada com pontaria! Voce se afasta neste turno.");
    notificarMensagemCombate(msg, msg);
}
