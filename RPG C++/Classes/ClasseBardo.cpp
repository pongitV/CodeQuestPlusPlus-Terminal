#include <iostream>
#include <memory>

#include "ClasseBardo.h"
#include "../Inventario/FabricaDeItens.h"
#include "../Sistema/Constantes.h"

std::string ClasseBardo::obterNomeClasse() const 
{
     return "Bardo"; 
}

std::vector<std::string> ClasseBardo::obterAparenciaClasseMenu() const 
{
    static const std::vector<std::string> aparencia = 
    {
        "                       ..:: :                    ",
        "                      =-.-:-:=:                  ",
        "                      =:--+-:--                  ",
        "                      ::-::::*=                  ",
        "                      .. ::-+-:                  ",
        "                    :-*-:-**=:.                  ",
        "                -**+******=+=**:                 ",
        "              :*+++*++++***#*#*#-                ",
        "             +*+==++===**=*+**+*-:               ",
        "             .+==+=*+====++.-==*==               ",
        "             :=*++***+=+*+*:====++               ",
        "            +:*****#*+++===++**+**               ",
        "            :=**+*##***+=+=+=#****               ",
        "           -*****#%****++++*##****.              ",
        "           ##*#***%#*****+*%#****#:              ",
        "           ##%+**#%******%#***#***+              ",
        "          *#%###***%#+*%%#**#+*###*:             ",
        "          ##@##*:-*#*%%%%%%%%+###***:            ",
        "         =##%%**#**%%%**%%%%*%******+            ",
        "         *%#@@#@@%##%#******%********#*.         ",
        "        -#%#%@@@@%%%%%####*===****** =#=#+       ",
        "        *%%%%%@@@@%@%==---==++#*****  %**        ",
        "       *%%%%%%@@@@%*=====++*+=#****   **=        ",
        "      +*%%%#%%%@@@%*++===+*#+=####*  -**         ",
        "     **%%%%%%%%###%*+===*+%#*====-   #**         ",
        "      *%#%%%%#%###%*+==+=%%%*++==    ***         ",
        "       #%%%%%#####%*+==+#-@%**++*   :**=         ",
        "       #@%%%######%*+++*% @%**+*.  :***:         ",
        "      +%%%%%#####%#*++*#* @%*++* -#****=         ",
        "      #%%%%#####*%*****%*:%#****#*******+        ",
        "    .#*%%%##%**##%*****@*@%****%*********+       ",
        "    %=%%%%###**#@%#***#**%%####@******#**+       ",
        "     %*%%%%     @#%%#%%@%@@%%#%@***#******=      ",
        "    #%%%@@%:   :%@%%%@@+@@@@@%%@***********      ",
        "      %%@%%%   @@@%@@@@. @@%%%%#***********      ",
        "          ** %@@@@%@: -@@@@   ***#********:      ",
        "                %@@@@@   :@@@# ***********       ",
        "                @@@@@:   -@@%%  *********        ",
        "                @%%@#::= #@@@@%%##               ",
        "                #%%@+      **%@%%@@              ",
        "               #%#%@=                            ",
        "              =@#%@@-                            ",
        "              %*#@@:                             ",
        "               :=:                               "
    };
    return aparencia;
}

Atributos ClasseBardo::obterAtributosClasse() const
{
    return { 0, 10, 10, 3, 10, 10, 10 };
}

std::vector<std::unique_ptr<Item>> ClasseBardo::obterEquipamentoClasse() const 
{
    std::vector<std::unique_ptr<Item>> equipamentos;
    
    int quantidadePocoes = 3;
    int porcentagemCura = 30;
    for (int i = 0; i < quantidadePocoes; ++i) {
        equipamentos.push_back(FabricaDeItens::criarItem("Pocao de Cura (" + std::to_string(porcentagemCura) + "%VM)"));
    }
    
    equipamentos.push_back(FabricaDeItens::criarItem("Violao encantado"));
    equipamentos.push_back(FabricaDeItens::criarItem("Capa magica"));
    equipamentos.push_back(FabricaDeItens::criarItem("Traje de Couro e tecido nobre"));
    return equipamentos;
}

std::string ClasseBardo::obterNomePassivaClasse() const 
{ return "Touch the sky"; }
std::string ClasseBardo::obterDescricaoPassivaClasse() const 
{ return "Curas e buffs recebidos sao 40% mais fortes."; }
std::string ClasseBardo::obterRecargaHabilidadeClasse() const 
{ return "Recarga: 3 turnos (Individuais)."; }

std::string ClasseBardo::obterNomeHabilidadeClasse() const 
{ return "Sinfonia do Bardo"; }
std::string ClasseBardo::obterDescricaoHabilidadeClasse() const 
{ return "Possui 3 habilidades: Flashing lights, On sight e Through the wire."; }
void ClasseBardo::usarHabilidadeClasse(Personagem* u, std::vector<Personagem*>& /*inimigos*/)
{
    int cdFlashing = u->obterCooldown("FlashingLights");
    int cdOnSight = u->obterCooldown("OnSight");
    int cdThroughWire = u->obterCooldown("ThroughTheWire");

    std::cout << "\n--- SINFONIA DO BARDO ---\n";
    std::cout << "[1] Flashing lights (Cura e pula o turno | Recarga: " << cdFlashing << ")\n";
    std::cout << "[2] On sight (1.5x Dano no proximo ataque | Recarga: " << cdOnSight << ")\n";
    std::cout << "[3] Through the wire (Metade do dano recebido | Recarga: " << cdThroughWire << ")\n";
    std::cout << "[0] CANCELAR\n";
    std::cout << "Escolha: ";

    int escolha;
    while (!(std::cin >> escolha)) { std::cin.clear(); std::cin.ignore(1000, '\n'); std::cout << "Escolha: "; }

    if (escolha == 0) {
        u->definirHabilidadeCancelada(true);
        return;
    }
    if (escolha == 1) {
        if (cdFlashing > 0) { std::cout << "[SISTEMA]: Em recarga!\n"; u->definirHabilidadeCancelada(true); return; }
        u->definirPularTurnoInimigo(true);
        int cura = (u->obterSabedoria() * 2) + (u->obterVidaMaxima() * 0.15);
        u->modificarVida(cura);
        u->definirCooldown("FlashingLights", 3);
        std::cout << "[HABILIDADE]: !Flashing lights! Voce recuperou " << cura << " HP e encantou os inimigos!\n";
        return;
    }
    if (escolha == 2) {
        if (cdOnSight > 0) { std::cout << "[SISTEMA]: Em recarga!\n"; u->definirHabilidadeCancelada(true); return; }
        u->definirMultiplicador(1.5);
        std::cout << u->obterNome() << " tocou 'On sight' e ganhara 1.5x de dano!\n";
        u->definirCooldown("OnSight", 3);
        std::cout << "[HABILIDADE]: !On sight! Seu proximo ataque causara 1.5x de dano!\n";
        return;
    }
    if (escolha == 3) {
        if (cdThroughWire > 0) { std::cout << "[SISTEMA]: Em recarga!\n"; u->definirHabilidadeCancelada(true); return; }
        u->adicionarEfeito(std::make_unique<EfeitoMetadeDano>(1));
        u->definirCooldown("ThroughTheWire", 3);
        std::cout << "[HABILIDADE]: !Through the wire! Voce esta protegido contra metade do dano recebido!\n";
        return;
    }
    std::cout << "[SISTEMA]: Opcao invalida!\n";
    u->definirHabilidadeCancelada(true);
}

TipoAtaque ClasseBardo::obterTipoAtaque() const { return TipoAtaque::UNICO; }
bool ClasseBardo::habilidadeConsomeTurno() const { return true; }

int ClasseBardo::processarCuraPassivaBardo(int curaBase) const 
{
    return static_cast<int>(curaBase * Constantes::MULTIPLICADOR_CURA_BARDO);
}

double ClasseBardo::processarMultiplicadorBuffPassivaBardo(double multBase) const 
{
    if (multBase > 1.0) return 1.0 + (multBase - 1.0) * 1.4;
    return multBase;
}
