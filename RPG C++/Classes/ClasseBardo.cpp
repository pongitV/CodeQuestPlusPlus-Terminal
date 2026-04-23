#include <iostream>
#include <memory>

#include "ClasseBardo.h"
#include "../Inventario/Arma.h"
#include "../Inventario/Escudo.h"
#include "../Inventario/Armadura.h"
#include "../Inventario/ItemConsumivel.h"

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
    // Ordem: { Vida, Forca, Destreza, Resistencia, Constituicao, Inteligencia, Sabedoria }
    return { 0, 10, 10, 3, 10, 10, 10 };
}

std::vector<std::unique_ptr<Item>> ClasseBardo::obterEquipamentoClasse() const 
{
    std::vector<std::unique_ptr<Item>> equipamentos;
    
    int quantidadePocoes = 3;
    int porcentagemCura = 30;
    for (int i = 0; i < quantidadePocoes; ++i) {
        equipamentos.push_back(std::make_unique<ItemConsumivel>("Pocao de Cura (" + std::to_string(porcentagemCura) + "%VM)"));
    }
    
    equipamentos.push_back(std::make_unique<Arma>("Violao encantado", 0, 10));
    equipamentos.push_back(std::make_unique<Escudo>("Capa magica", 6, 10));
    equipamentos.push_back(std::make_unique<Armadura>("Traje de Couro e tecido nobre", 4));
    return equipamentos;
}

std::string ClasseBardo::obterNomeHabilidadeClasse() const { return "Sinfonia do Bardo"; }
std::string ClasseBardo::obterDescricaoHabilidadeClasse() const { return "Possui 3 habilidades: Flashing lights, On sight e Through the wire."; }
void ClasseBardo::usarHabilidadeClasse(Personagem* u, std::vector<Personagem*>& /*inimigos*/) 
{
    while (true) {
        std::cout << "\n--- SINFONIA DO BARDO ---\n";
        std::cout << "[1] Flashing lights (Cura e pula o turno | Recarga: " << u->obterCooldown("FlashingLights") << ")\n";
        std::cout << "[2] On sight (1.5x Dano no proximo ataque | Recarga: " << u->obterCooldown("OnSight") << ")\n";
        std::cout << "[3] Through the wire (Metade do dano recebido | Recarga: " << u->obterCooldown("ThroughTheWire") << ")\n";
        std::cout << "[0] CANCELAR\n";
        std::cout << "Escolha: ";
        
        int escolha;
        if (!(std::cin >> escolha)) { std::cin.clear(); std::cin.ignore(1000, '\n'); continue; }
        
        if (escolha == 0) {
            u->definirHabilidadeCancelada(true);
            return;
        }
        if (escolha == 1) {
            if (u->obterCooldown("FlashingLights") > 0) { std::cout << "[SISTEMA]: Em recarga!\n"; continue; }
            u->definirPularTurnoInimigo(true);
            
            // Cura baseada na sabedoria do Bardo + % da vida maxima
            int cura = (u->obterSabedoria() * 2) + (u->obterVidaMaxima() * 0.15);
            u->modificarVida(cura);
            u->definirCooldown("FlashingLights", 3);
            std::cout << "[HABILIDADE]: !Flashing lights! Voce recuperou " << cura << " HP e encantou os inimigos!\n";
            return;
        }
        if (escolha == 2) {
            if (u->obterCooldown("OnSight") > 0) { std::cout << "[SISTEMA]: Em recarga!\n"; continue; }
            u->definirMultiplicador(1.5);
            std::cout << u->obterNome() << " tocou 'On sight' e ganhara 1.5x de dano!\n";
            u->definirCooldown("OnSight", 3);
            std::cout << "[HABILIDADE]: !On sight! Seu proximo ataque causara 1.5x de dano!\n";
            return;
        }
        if (escolha == 3) {
            if (u->obterCooldown("ThroughTheWire") > 0) { std::cout << "[SISTEMA]: Em recarga!\n"; continue; }
            u->adicionarEfeito(std::make_unique<EfeitoMetadeDano>(1));
            u->definirCooldown("ThroughTheWire", 3);
            std::cout << "[HABILIDADE]: !Through the wire! Voce esta protegido contra metade do dano recebido!\n";
            return;
        }
        std::cout << "[SISTEMA]: Opcao invalida!\n";
    }
}

TipoAtaque ClasseBardo::obterTipoAtaque() const { return TipoAtaque::UNICO; }
bool ClasseBardo::habilidadeConsomeTurno() const { return true; }
