#include "Bardo.h"

#include <array>
#include <functional>
#include <iostream>
#include <memory>

#include "../../Sistemas/Combate/Combate.h"
#include "../../Sistemas/Inventario/FabricaItens.h"
#include "../../Core/Utilidades/Constantes.h"
#include "../../Core/Utilidades/Aparencia.h"
#include "../../Core/Utilidades/FuncoesDialogo.h"
#include "../../Core/Utilidades/ControleDeInput.h"
#include "../../Perspectiva/TelasBase/Combate/TelaCombate.h"

// --- INFORMACOES DA CLASSE ---
std::string Bardo::obterNomeClasse() const 
{
     return "Bardo"; 
}

const std::vector<std::string>& Bardo::obterAparenciaClasseMenu() const 
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
    auto equipamentos = FabricaItens::criarKitPocoes();
    
    equipamentos.push_back(FabricaItens::criarItem(ItemID::ViolaoEncantado));
    equipamentos.push_back(FabricaItens::criarItem(ItemID::CapaMagica));
    equipamentos.push_back(FabricaItens::criarItem(ItemID::TrajeNobre));
    return equipamentos;
}

// --- PASSIVA DA CLASSE ---
std::string Bardo::obterNomePassivaClasse() const 
{ 
    return "Touch the sky"; 
}

std::string Bardo::obterDescricaoPassivaClasse() const 
{ 
    return "Curas e buffs recebidos sao 40% mais fortes."; 
}

int Bardo::processarCuraPassivaBardo(int curaBase) const 
{
    return static_cast<int>(curaBase * Constantes::MULTIPLICADOR_CURA_BARDO);
}

double Bardo::processarMultiplicadorBuffPassivaBardo(double multBase) const 
{
    if (multBase > 1.0) return 1.0 + (multBase - 1.0) * 1.4;
    return multBase;
}

// --- HABILIDADE DA CLASSE ---
std::string Bardo::obterRecargaHabilidadeClasse() const 
{ 
    return "Recarga: 3 turnos (Individuais)."; 
}

std::string Bardo::obterNomeHabilidadeClasse() const 
{ 
    return "Sinfonia do Bardo"; 
}

std::string Bardo::obterDescricaoHabilidadeClasse() const 
{ 
    return "Possui 3 habilidades: Flashing lights, On sight e Through the wire."; 
}

void Bardo::usarHabilidadeClasse(Combate* /*combate*/, Personagem* personagemUsuario, std::vector<Personagem*>& /*listaDeInimigos*/)
{
    struct SubHabilidade {
        HabilidadeID id;
        std::string nome;
        std::string descricao;
        std::function<void(Personagem*)> acao;
    };

    const std::array<SubHabilidade, 3> habilidades = {{
        { HabilidadeID::FlashingLights, "Flashing lights", "Cura e pula o turno", [this](Personagem* personagemHabilidade) {
            personagemHabilidade->definirPularTurnoInimigo(true);
            int cura = static_cast<int>((personagemHabilidade->obterSabedoria() * 2) + (personagemHabilidade->obterVidaMaxima() * 0.15));
            personagemHabilidade->modificarVida(cura);
            personagemHabilidade->definirCooldown(HabilidadeID::FlashingLights, 3);
            std::string msg = FuncoesDialogo::formatarMsgHabilidade("!Flashing lights! Voce recuperou " + std::to_string(cura) + " HP e encantou os inimigos!", Cor::VERDE);
            this->notificarMensagemCombate(msg, msg);
        }},
        { HabilidadeID::OnSight, "On sight", "1.5x Dano no proximo ataque", [this](Personagem* personagemHabilidade) {
            personagemHabilidade->definirMultiplicador(1.5);
            personagemHabilidade->definirCooldown(HabilidadeID::OnSight, 3);
            std::string msg1 = FuncoesDialogo::formatarMsgHabilidade(personagemHabilidade->obterNome() + " tocou 'On sight' e ganhara 1.5x de dano!");
            this->notificarMensagemCombate(msg1, msg1);
            std::string msg2 = FuncoesDialogo::formatarMsgHabilidade("!On sight! Seu proximo ataque causara 1.5x de dano!");
            this->notificarMensagemCombate(msg2, msg2);
        }},
        { HabilidadeID::ThroughTheWire, "Through the wire", "Metade do dano recebido", [this](Personagem* personagemHabilidade) {
            personagemHabilidade->adicionarEfeito(std::make_unique<EfeitoMetadeDano>(1));
            personagemHabilidade->definirCooldown(HabilidadeID::ThroughTheWire, 3);
            std::string msg = FuncoesDialogo::formatarMsgHabilidade("!Through the wire! Voce esta protegido contra metade do dano recebido!", Cor::CIANO);
            this->notificarMensagemCombate(msg, msg);
        }}
    }};

    std::cout << "\n" << TelaCombate::margemCombate() << "--- SINFONIA DO BARDO ---\n\n";
    std::vector<std::string> opcoesHabilidades;
    for (size_t i = 0; i < habilidades.size(); ++i) {
        int cd = personagemUsuario->obterCooldown(habilidades[i].id);
        opcoesHabilidades.push_back(habilidades[i].nome + " (" + habilidades[i].descricao + " | Recarga: " + std::to_string(cd) + ")");
    }
    opcoesHabilidades.push_back("CANCELAR");

    int escolha = ControleDeInput::lerSelecaoMenuComSetas(opcoesHabilidades, false, TelaCombate::margemCombate());

    if (escolha == static_cast<int>(habilidades.size())) {
        personagemUsuario->definirHabilidadeCancelada(true);
        return;
    }
    
    const auto& hab = habilidades[escolha];
    int cd = personagemUsuario->obterCooldown(hab.id);
    if (verificarEReportarRecarga(personagemUsuario, cd, hab.nome)) return;

    hab.acao(personagemUsuario);
}
