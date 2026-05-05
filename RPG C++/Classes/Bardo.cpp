#include "Bardo.h"

#include <array>
#include <functional>
#include <iostream>
#include <memory>

#include "../Inventario/FabricaItens.h"
#include "../Utilidades/Constantes.h"
#include "../Utilidades/SimplificacoesAparencia.h"

std::string Bardo::obterNomeClasse() const 
{
     return "Bardo"; 
}

std::vector<std::string> Bardo::obterAparenciaClasseMenu() const 
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

Atributos Bardo::obterAtributosClasse() const
{
    return { 0, 10, 10, 3, 10, 10, 10};
}

std::vector<std::unique_ptr<Item>> Bardo::obterEquipamentoClasse() const 
{
    std::vector<std::unique_ptr<Item>> equipamentos;
    
    int quantidadePocoes = 3;
    for (int i = 0; i < quantidadePocoes; ++i) {
        equipamentos.push_back(FabricaItens::criarItem(ItemID::PocaoCura30));
    }
    
    equipamentos.push_back(FabricaItens::criarItem(ItemID::ViolaoEncantado));
    equipamentos.push_back(FabricaItens::criarItem(ItemID::CapaMagica));
    equipamentos.push_back(FabricaItens::criarItem(ItemID::TrajeNobre));
    return equipamentos;
}

std::string Bardo::obterNomePassivaClasse() const 
{ return "Touch the sky"; }
std::string Bardo::obterDescricaoPassivaClasse() const 
{ return "Curas e buffs recebidos sao 40% mais fortes."; }
std::string Bardo::obterRecargaHabilidadeClasse() const 
{ return "Recarga: 3 turnos (Individuais)."; }

std::string Bardo::obterNomeHabilidadeClasse() const 
{ return "Sinfonia do Bardo"; }
std::string Bardo::obterDescricaoHabilidadeClasse() const 
{ return "Possui 3 habilidades: Flashing lights, On sight e Through the wire."; }
void Bardo::usarHabilidadeClasse(SistemaPersonagem* personagemUsuario, std::vector<SistemaPersonagem*>& /*listaDeInimigos*/)
{
    struct SubHabilidade {
        HabilidadeID id;
        std::string nome;
        std::string descricao;
        std::function<void(SistemaPersonagem*)> acao;
    };

    static const std::array<SubHabilidade, 3> habilidades = {{
        { HabilidadeID::FlashingLights, "Flashing lights", "Cura e pula o turno", [](SistemaPersonagem* personagemHabilidade) {
            personagemHabilidade->definirPularTurnoInimigo(true);
            int cura = static_cast<int>((personagemHabilidade->obterSabedoria() * 2) + (personagemHabilidade->obterVidaMaxima() * 0.15));
            personagemHabilidade->modificarVida(cura);
            personagemHabilidade->definirCooldown(HabilidadeID::FlashingLights, 3);
            std::cout << SimplificacoesAparencia::cor(Cor::VERDE) << "[HABILIDADE]: !Flashing lights! Voce recuperou " << cura << " HP e encantou os inimigos!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
        }},
        { HabilidadeID::OnSight, "On sight", "1.5x Dano no proximo ataque", [](SistemaPersonagem* personagemHabilidade) {
            personagemHabilidade->definirMultiplicador(1.5);
            std::cout << SimplificacoesAparencia::cor(Cor::AMARELO) << personagemHabilidade->obterNome() << " tocou 'On sight' e ganhara 1.5x de dano!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
            personagemHabilidade->definirCooldown(HabilidadeID::OnSight, 3);
            std::cout << SimplificacoesAparencia::cor(Cor::AMARELO) << "[HABILIDADE]: !On sight! Seu proximo ataque causara 1.5x de dano!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
        }},
        { HabilidadeID::ThroughTheWire, "Through the wire", "Metade do dano recebido", [](SistemaPersonagem* personagemHabilidade) {
            personagemHabilidade->adicionarEfeito(std::make_unique<EfeitoMetadeDano>(1));
            personagemHabilidade->definirCooldown(HabilidadeID::ThroughTheWire, 3);
            std::cout << SimplificacoesAparencia::cor(Cor::CIANO) << "[HABILIDADE]: !Through the wire! Voce esta protegido contra metade do dano recebido!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
        }}
    }};

    std::cout << "\n--- SINFONIA DO BARDO ---\n";
    for (size_t i = 0; i < habilidades.size(); ++i) {
        int cd = personagemUsuario->obterCooldown(habilidades[i].id);
        std::cout << "[" << i + 1 << "] " << habilidades[i].nome 
                  << " (" << habilidades[i].descricao << " | Recarga: " << cd << ")\n";
    }
    std::cout << "[0] CANCELAR\n";
    std::cout << "Escolha: ";

    int escolha;
    while (!(std::cin >> escolha)) { std::cin.clear(); std::cin.ignore(1000, '\n'); std::cout << "Escolha: "; }

    if (escolha == 0) {
        personagemUsuario->definirHabilidadeCancelada(true);
        return;
    }
    
    if (escolha > 0 && escolha <= static_cast<int>(habilidades.size())) {
        const auto& hab = habilidades[escolha - 1];
        int cd = personagemUsuario->obterCooldown(hab.id);
        if (cd > 0) {
            std::cout << "\n" << SimplificacoesAparencia::cor(Cor::VERMELHO) << "[SISTEMA]: A habilidade " << hab.nome << " esta em recarga (" << cd << " turnos)!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n"; 
            SimplificacoesAparencia::aguardarEnter();
            personagemUsuario->definirHabilidadeCancelada(true); 
            return; 
        }
        hab.acao(personagemUsuario);
        return;
    }

    std::cout << "\n" << SimplificacoesAparencia::cor(Cor::VERMELHO) << "[SISTEMA]: Opcao invalida!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
    SimplificacoesAparencia::aguardarEnter();
    personagemUsuario->definirHabilidadeCancelada(true);
}

TipoAtaque Bardo::obterTipoAtaque() const { return TipoAtaque::UNICO; }
bool Bardo::habilidadeConsomeTurno() const { return true; }

int Bardo::processarCuraPassivaBardo(int curaBase) const 
{
    return static_cast<int>(curaBase * Constantes::MULTIPLICADOR_CURA_BARDO);
}

double Bardo::processarMultiplicadorBuffPassivaBardo(double multBase) const 
{
    if (multBase > 1.0) return 1.0 + (multBase - 1.0) * 1.4;
    return multBase;
}
