#include "Arqueiro.h"

#include <iostream>
#include <memory>

#include "../Inventario/FabricaItens.h"
#include "../Utilidades/SimplificacoesAparencia.h"

std::string Arqueiro::obterNomeClasse() const 
{
     return "Arqueiro"; 
}

std::vector<std::string> Arqueiro::obterAparenciaClasseMenu() const 
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
    std::vector<std::unique_ptr<Item>> equipamentos;
    
    int quantidadePocoes = 3;
    for (int i = 0; i < quantidadePocoes; ++i) {
        equipamentos.push_back(FabricaItens::criarItem(ItemID::PocaoCura30));
    }
    
    equipamentos.push_back(FabricaItens::criarItem(ItemID::ArcoMadeira));
    equipamentos.push_back(FabricaItens::criarItem(ItemID::BracedeirasPrata));
    equipamentos.push_back(FabricaItens::criarItem(ItemID::ArmaduraCouro));
    return equipamentos;
}

std::string Arqueiro::obterNomePassivaClasse() const 
{ return "Passos leves"; }
std::string Arqueiro::obterDescricaoPassivaClasse() const 
{ return "Penalidade de armaduras e debuffs de lentidao reduzidos pela metade."; }
std::string Arqueiro::obterRecargaHabilidadeClasse() const 
{ return "Recarga: 1 turno."; }

std::string Arqueiro::obterNomeHabilidadeClasse() const 
{ return "Retirada com pontaria"; }
std::string Arqueiro::obterDescricaoHabilidadeClasse() const 
{ return "Se afasta durante um turno, no proximo turno causa 2x dano"; }
void Arqueiro::usarHabilidadeClasse(SistemaPersonagem* personagemUsuario, std::vector<SistemaPersonagem*>& /*listaDeInimigos*/) {
    if (personagemUsuario->obterRecarga()) 
    {
        std::cout << "\n" << SimplificacoesAparencia::cor(Cor::VERMELHO) << "[SISTEMA]: A habilidade " << obterNomeHabilidadeClasse() << " esta em recarga (1 turnos)!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
        SimplificacoesAparencia::aguardarEnter();
        personagemUsuario->definirHabilidadeCancelada(true);
        return;
    }
    personagemUsuario->adicionarEfeito(std::make_unique<EfeitoInviolavel>(1));
    personagemUsuario->definirRecarga(true);
    std::cout << SimplificacoesAparencia::cor(Cor::VERDE) << "[HABILIDADE]: Retirada com pontaria! Voce se afasta neste turno." << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
}

TipoAtaque Arqueiro::obterTipoAtaque() const { return TipoAtaque::UNICO; }
bool Arqueiro::habilidadeConsomeTurno() const { return true; }

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
