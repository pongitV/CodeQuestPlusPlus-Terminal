#pragma once

#include <string>
#include <vector>
#include "Aparencia.h"
#include <functional>

enum class ComandoMapa {
    Cima,
    Baixo,
    Esquerda,
    Direita,
    Inventario,
    Ficha,
    Bestiario,
    Nenhum
};

class ControleDeInput 
{
public:
    // Abstrai as restricoes do SO ao interagir diretamente com o input do usuario
    static bool teclaPressionada();
    static char lerTecla();
    static ComandoMapa traduzirTeclaParaComando(char tecla);
    static void limparBuffer();
    static std::string lerEntradaProtegida(const std::string& promptMensagem = "");
    
    static int lerInteiroComLimites(const std::string& promptMensagem, int minimo, int maximo, bool centralizarPrompt = false, const std::string& margemPersonalizada = "");
    static int lerSelecaoMenuComSetas(const std::vector<std::string>& opcoes, bool centralizar = true, const std::string& margemPersonalizada = "", const std::vector<std::string>& painelDireito = {});
    static int lerSelecaoMenuEmPopup(const std::string& titulo, const std::vector<std::string>& texto, const std::vector<std::string>& opcoes, Cor corTema = Cor::BRANCO, const std::vector<std::string>& arteAscii = {}, bool animarEntrada = true);
    static void aguardarEnter(const std::string& mensagem = "Pressione ENTER para continuar...");
    
    static void executarLoopMenuPopup(
        const std::function<void()>& exibirDialogo,
        const std::function<std::vector<std::string>()>& obterOpcoes,
        const std::function<bool(const std::string&)>& processarOpcao,
        const std::string& titulo,
        Cor corTema,
        const std::vector<std::string>& arteAscii
    );
    static std::function<void()> onAguardarEnterUpdate;
    static std::string enterPromptText;
};
