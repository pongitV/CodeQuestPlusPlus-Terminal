#include <iostream>
#include <map>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif

#include "Inventario.h"
#include "Item.h"

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
    
    std::vector<Item*> equipamentosOutros;
    std::map<std::string, int> consumiveisMap;
    std::vector<Item*> missoes;

    for (Item* item : itens) {
        TipoEquipamento tipo = item->obterTipo();
        if (tipo == TipoEquipamento::CONSUMIVEL) {
            consumiveisMap[item->obterNomeItem()]++;
        } else if (tipo == TipoEquipamento::MISSAO) {
            missoes.push_back(item);
        } else if ((tipo == TipoEquipamento::ARMA || tipo == TipoEquipamento::ESCUDO || tipo == TipoEquipamento::ARMADURA) 
                   && item != armaEquipada && item != escudoEquipado && item != armaduraEquipada) {
            equipamentosOutros.push_back(item);
        }
    }
    
    std::vector<std::string> linhas;
    linhas.push_back("DINHEIRO: " + std::to_string(ouro) + " moedas");
    linhas.push_back("");

    linhas.push_back("[ EQUIPAMENTO ]");
    if (armaEquipada) linhas.push_back(" [1A] ARMA:     " + armaEquipada->obterNomeItem());
    if (escudoEquipado) linhas.push_back(" [2A] ESCUDO:   " + escudoEquipado->obterNomeItem());
    if (armaduraEquipada) linhas.push_back(" [3A] ARMADURA: " + armaduraEquipada->obterNomeItem());

    linhas.push_back("");
    linhas.push_back("[ ARSENAL ]");
    if (equipamentosOutros.empty()) linhas.push_back(" (Vazio)");
    for (size_t i = 0; i < equipamentosOutros.size(); i++)
        linhas.push_back(" [" + std::to_string(i + 1) + "E] " + equipamentosOutros[i]->obterNomeItem() + " [" + equipamentosOutros[i]->raridadeParaString() + "]");

    linhas.push_back("");
    linhas.push_back("[ CONSUMIVEIS ]");
    if (consumiveisMap.empty()) linhas.push_back(" (Vazio)");
    int contadorC = 1;
    for (auto const& [nome, qtd] : consumiveisMap) 
        linhas.push_back(" [" + std::to_string(contadorC++) + "C] " + std::to_string(qtd) + "x " + nome);

    linhas.push_back("");
    linhas.push_back("[ ITENS DE MISSAO ]");
    if (missoes.empty()) linhas.push_back(" (Vazio)");
    for (size_t i = 0; i < missoes.size(); i++)
        linhas.push_back(" [" + std::to_string(i + 1) + "M] " + missoes[i]->obterNomeItem());

    int maxLen = 0;
    for (const std::string& linha : linhas) {
        if ((int)linha.length() > maxLen) maxLen = (int)linha.length();
    }
    
    int espacos = (largura - maxLen) / 2;
    std::string margem(espacos > 0 ? espacos : 0, ' ');

    for (const std::string& linha : linhas) {
        std::cout << margem << linha << "\n";
    }
    
    std::cout << "\n" << std::string(largura, '=') << "\n";
}

Item* Inventario::buscarItemPorCodigo(std::string codigo, Item* a, Item* e, Item* d)
{
    if (codigo.length() < 2) return nullptr;

    char categoria = std::toupper(codigo.back());
    std::string numParte = codigo.substr(0, codigo.length() - 1);
    
    for(char c : numParte) if(!isdigit(c)) return nullptr;
    int indiceAlvo = std::stoi(numParte);
    if (indiceAlvo <= 0) return nullptr;

    if (categoria == 'A')
    {
        if (indiceAlvo == 1) return a;
        if (indiceAlvo == 2) return e;
        if (indiceAlvo == 3) return d;
    }
    else if (categoria == 'E')
    {
        std::vector<Item*> equipamentosOutros;
        for (Item* item : itens) {
            TipoEquipamento tipo = item->obterTipo();
            if ((tipo == TipoEquipamento::ARMA || tipo == TipoEquipamento::ESCUDO || tipo == TipoEquipamento::ARMADURA) 
                && item != a && item != e && item != d) {
                equipamentosOutros.push_back(item);
            }
        }
        if (indiceAlvo <= static_cast<int>(equipamentosOutros.size())) return equipamentosOutros[indiceAlvo - 1];
    }
    else if (categoria == 'C')
    {
        std::map<std::string, int> contagem;
        for (Item* item : itens) 
            if (item->obterTipo() == TipoEquipamento::CONSUMIVEL) contagem[item->obterNomeItem()]++;

        int contador = 1;
        for (auto const& [nome, qtd] : contagem)
            if (contador++ == indiceAlvo) {
                for (Item* item : itens) if (item->obterNomeItem() == nome) return item;
            }
    }
    else if (categoria == 'M')
    {
        std::vector<Item*> missoes;
        for (Item* item : itens) if (item->obterTipo() == TipoEquipamento::MISSAO) missoes.push_back(item);
        if (indiceAlvo <= static_cast<int>(missoes.size())) return missoes[indiceAlvo - 1];
    }
    
    return nullptr;
}

Item* Inventario::escolherEscudoParaDefesa() 
{
    std::vector<Item*> escudos;
    for (Item* item : itens) 
    {
        if (item->obterTipo() == TipoEquipamento::ESCUDO) escudos.push_back(item);
    }

    if (escudos.empty()) 
    {
        std::cout << "\n[!] Voce nao possui escudos no inventario para usar!\n";
        return nullptr;
    }

    std::cout << "\n=== SELECIONE SEU ESCUDO ===\n";
    for (size_t i = 0; i < escudos.size(); i++) 
    {
        std::cout << " [" << i + 1 << "] " << escudos[i]->obterNomeItem() 
                  << " (Bloqueio Fixo: " << escudos[i]->obterReducaoFixaEscudo() 
                  << " | Durabilidade: " << escudos[i]->obterDurabilidade() << " usos)\n";
    }
    std::cout << " [0] Cancelar\n";
    std::cout << "\nEscolha: ";

    int op;
    if (!(std::cin >> op) || op < 0 || op > static_cast<int>(escudos.size())) 
    {
        std::cin.clear(); std::cin.ignore(1000, '\n');
        std::cout << "Opcao invalida!\n";
        return nullptr;
    }
    if (op == 0) return nullptr;
    return escudos[op - 1];
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