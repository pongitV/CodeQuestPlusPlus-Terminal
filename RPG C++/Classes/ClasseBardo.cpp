#include <iostream>
#include <vector>

#include "ClasseBardo.h"
#include "../Inventario/Item.h"

std::string ClasseBardo::obterNomeClasse() const 
{
     return "Bardo"; 
}

std::vector<std::string> ClasseBardo::obterAparenciaClasseMenu() const 
{
    return 
    {
        "       _______  ",
        "      /       \\ ",
        "     |  (O O)  |",
        "      \\  _w_  / ",
        "    ___|     |___",
        "   /   |     |   \\",
        "  /    |_____|    \\",
        " |   / |     | \\   |",
        " |  |  |     |  |  |",
        " |  |  |_____|  |  |",
        " |  | /       \\ |  |",
        " |__|/  [#|#]  \\|__|", // Alaúde
        "     \\  [#|#]  /   ",
        "      \\_______/    ",
        "       |     |     ",
        "       |     |     ",
        "      /       \\    ",
        "     /_________\\   "
    };
}

Atributos ClasseBardo::obterAtributosClasse() const
{
    return { 0, 0, 5, 0, 5, 10, 10 };
}

std::vector<Item*> ClasseBardo::gerarKitInicial() const 
{
    return 
    {
        new PocaoCura(), new PocaoCura(), new PocaoCura(),
        new Arma("Violao encantado", 3),
        new Escudo("Capa magica", 0.10),
        new Armadura("Roupas nobres de tecido", 1)
    };
}

void ClasseBardo::usarHabilidadeClasseAtiva(Personagem* u, std::vector<Personagem*>& inimigos) 
{
    if (u->obterRecarga()) 
    {
        std::cout << "[SISTEMA]: Habilidade em recarga! Use um ataque normal.\n";
        return; 
    }

    // Cura 30% da vida perdida
    int vidaPerdida = u->obterVidaMaxima() - u->obterVida();
    int cura = static_cast<int>(vidaPerdida * 0.3);
    u->modificarVida(cura);

    u->definirPularTurnoInimigo(true);
    u->definirRecarga(true); // Ativa recarga para o proximo turno

    std::cout << "[HABILIDADE]: Flashing lights! Voce recuperou " << cura << " HP.\n";
    std::cout << "[EFEITO]: O inimigo ficou atordoado e pulara o proximo turno!\n";
}

std::string ClasseBardo::obterNomeHabilidadeClasseAtiva() const { return "Flashing lights"; }
std::string ClasseBardo::obterDescricaoHabilidadeClasseAtiva() const { 
    return "Cura 30% da vida perdida e pula o turno do inimigo. Recarga: 1 turno."; 
}