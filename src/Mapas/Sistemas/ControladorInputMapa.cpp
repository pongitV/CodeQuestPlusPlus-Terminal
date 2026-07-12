#include "ControladorInputMapa.h"
#include "../../Core/Utilidades/InputDispatcher.h"
#include "../../Core/Utilidades/ControleDeInput.h"
#include "../../Perspectiva/TelasBase/Pause/TelaPause.h"
#include "../../Core/Controladores/Debug.h"
#include "../../Sistemas/Inventario/InventarioCombate.h"
#include "../../Perspectiva/TelasBase/Atributos/TelaAtributos.h"
#include "../../Perspectiva/TelasBase/Diario/TelaDiario.h"

bool ControladorInputMapa::processarInputEComandos(char tecla, Personagem* jogador, int& proximaPosicaoX, int& proximaPosicaoY, const std::function<void()>& restaurarTela)
{
    static bool inicializado = false;
    static InputDispatcher dispatcher;
    if (!inicializado) {
        inicializado = true;
        dispatcher.registrar(27, [=]() { TelaPause::exibir(jogador); restaurarTela(); });
        dispatcher.registrar(static_cast<int>('\\'), [=]() { Debug::exibirMenuDebug(jogador); restaurarTela(); });
        dispatcher.registrar(static_cast<int>('`'),  [=]() { Debug::exibirMenuDebug(jogador); restaurarTela(); });
        dispatcher.registrar(static_cast<int>('='),  [=]() { Debug::exibirMenuDebug(jogador); restaurarTela(); });
    }

    if (dispatcher.executar(static_cast<int>(tecla))) return true;

    ComandoMapa comando = ControleDeInput::traduzirTeclaParaComando(tecla);

    if (comando == ComandoMapa::Cima) { proximaPosicaoY--; return false; }
    if (comando == ComandoMapa::Baixo) { proximaPosicaoY++; return false; }
    if (comando == ComandoMapa::Esquerda) { proximaPosicaoX--; return false; }
    if (comando == ComandoMapa::Direita) { proximaPosicaoX++; return false; }

    if (comando == ComandoMapa::Inventario)
    {
        InventarioCombate::gerenciarInventario(jogador);
        restaurarTela();
        return true;
    }
    if (comando == ComandoMapa::Ficha)
    {
        TelaAtributos::gerenciarFichaDoJogador(jogador);
        restaurarTela();
        return true;
    }
    if (comando == ComandoMapa::Bestiario)
    {
        TelaDiario::exibir(jogador);
        restaurarTela();
        return true;
    }
    return false;
}
