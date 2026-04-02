#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>
#include <cctype>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif

#include "Inventario.h"
#include "Item.h"

// Função auxiliar para detetar a largura do terminal
int obterLargura() 
{
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
#else
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
#endif
}

Inventario::Inventario() : ouro(0) {}

Inventario::~Inventario() 
{
    for (Item* item : itens) delete item;
    itens.clear();
}

void Inventario::listarItens(Item* armaEquipada, Item* escudoEquipado, Item* armaduraEquipada) const 
{
    int largura = obterLargura();
    std::string titulo = "INVENTARIO";
    int espacos = (largura - (int)titulo.length()) / 2;

    // Título horizontal dinâmico
    std::cout << std::string(largura, '=') << "\n";
    std::cout << std::string(espacos > 0 ? espacos : 0, ' ') << titulo << "\n";
    std::cout << std::string(largura, '=') << "\n";
    
    std::cout << " DINHEIRO: " << ouro << " moedas\n";
    
    // 1. ARSENAL (A)
    std::cout << "\n [ ARSENAL - EQUIPADOS ]" << std::endl;
    if (armaEquipada) std::cout << "  [1A] ARMA:    " << armaEquipada->obterNomeItem() << std::endl;
    if (escudoEquipado) std::cout << "  [2A] ESCUDO:  " << escudoEquipado->obterNomeItem() << std::endl;
    if (armaduraEquipada) std::cout << "  [3A] ARMADURA: " << armaduraEquipada->obterNomeItem() << std::endl;

    // 2. EQUIPAMENTOS (E)
    std::cout << "\n [ EQUIPAMENTOS ]" << std::endl;
    int contadorE = 1;
    bool temEquip = false;
    for (Item* item : itens)
    {
        TipoEquipamento tipo = item->obterTipo();
        if ((tipo == TipoEquipamento::ARMA || tipo == TipoEquipamento::ESCUDO || tipo == TipoEquipamento::ARMADURA) 
            && item != armaEquipada && item != escudoEquipado && item != armaduraEquipada)
        {
            std::cout << "  [" << contadorE++ << "E] " << item->obterNomeItem() << " [" << item->raridadeParaString() << "]\n";
            temEquip = true;
        }
    }
    if (!temEquip) std::cout << "  (Vazio)\n";

    // 3. CONSUMIVEIS (C)
    std::cout << "\n [ CONSUMIVEIS ]" << std::endl;
    std::map<std::string, int> contagem;
    for (Item* item : itens) 
        if (item->obterTipo() == TipoEquipamento::CONSUMIVEL) contagem[item->obterNomeItem()]++;

    if (contagem.empty()) std::cout << "  (Vazio)\n";
    int contadorC = 1;
    for (auto const& [nome, qtd] : contagem) 
        std::cout << "  [" << contadorC++ << "C] " << qtd << "x " << nome << std::endl;

    // 4. ITENS DE MISSAO (M)
    std::cout << "\n [ ITENS DE MISSAO ]" << std::endl;
    int contadorM = 1;
    bool temMissao = false;
    for (Item* item : itens)
        if (item->obterTipo() == TipoEquipamento::MISSAO)
        {
            std::cout << "  [" << contadorM++ << "M] " << item->obterNomeItem() << std::endl;
            temMissao = true;
        }
    if (!temMissao) std::cout << "  (Vazio)\n";
    
    std::cout << std::string(largura, '=') << std::endl;
}

Item* Inventario::buscarItemPorCodigo(std::string codigo, Item* a, Item* e, Item* d)
{
    if (codigo.length() < 2) return nullptr;

    char categoria = std::toupper(codigo.back());
    std::string numParte = codigo.substr(0, codigo.length() - 1);
    
    for(char c : numParte) if(!isdigit(c)) return nullptr;
    int indiceAlvo = std::stoi(numParte);
    int contador = 1;

    if (categoria == 'A')
    {
        if (indiceAlvo == 1) return a;
        if (indiceAlvo == 2) return e;
        if (indiceAlvo == 3) return d;
    }
    else if (categoria == 'C')
    {
        std::map<std::string, int> contagem;
        for (Item* item : itens) 
            if (item->obterTipo() == TipoEquipamento::CONSUMIVEL) contagem[item->obterNomeItem()]++;

        for (auto const& [nome, qtd] : contagem)
            if (contador++ == indiceAlvo)
                for (Item* item : itens) if (item->obterNomeItem() == nome) return item;
    }
    return nullptr;
}

void Inventario::adicionarItem(Item* n) { if(n) itens.push_back(n); }

void Inventario::removerPorNome(std::string nome) 
{
    for (size_t i = 0; i < itens.size(); i++) 
    {
        if (itens[i]->obterNomeItem() == nome) 
        { 
            delete itens[i]; 
            itens.erase(itens.begin() + i); 
            break; 
        }
    }
}

int Inventario::contarItensPorNome(std::string nome) const 
{
    int c = 0; 
    for (Item* i : itens) if (i->obterNomeItem() == nome) c++; 
    return c;
}

bool Inventario::possuiPocaoCura() const 
{
    for (Item* i : itens) if (dynamic_cast<PocaoCura*>(i)) return true; 
    return false;
}

void Inventario::adicionarOuro(int q) { if (q > 0) ouro += q; }
int Inventario::obterOuro() const { return ouro; }
bool Inventario::estaVazio() const { return itens.empty(); }