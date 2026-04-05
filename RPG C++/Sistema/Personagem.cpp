#include <iostream>
#include <string>
#include <vector>

#include "Personagem.h"
#include "../Raças/RacaBase.h"
#include "../Classes/ClasseBase.h"

Personagem::Personagem(std::string nome, RacaBase* r, ClasseBase* c) 
{
    this->nomePersonagem = nome;
    this->raca = r;
    this->classe = c;
    this->mochila = new Inventario();
    this->statsFinais = { 100, 0, 0, 0, 0, 0, 0 }; // Atributos base

    this->arma = nullptr;
    this->escudo = nullptr;
    this->armadura = nullptr;

    std::vector<Item*> kit = c->gerarKitInicial();
    for (Item* item : kit) 
    {
        this->mochila->adicionarItem(item); 
        this->equiparItem(item);            
    }
    calcularAtributos();

    this->podeReviver = true; // Habilidade de raça "Espirito indomavel" ativa por padrão
}

Personagem::~Personagem() 
{
    delete raca;
    delete classe;
    delete mochila;
}  

void Personagem::calcularAtributos() 
{
    this->statsFinais.calcularAtributos(raca->obterAtributosRaca());
    this->statsFinais.calcularAtributos(classe->obterAtributosClasse());
    this->vidaAtual = statsFinais.vida;
}

void Personagem::modificarVida(int valor) 
{
    this->vidaAtual += valor;
    if (this->vidaAtual < 0) this->vidaAtual = 0;
    if (this->vidaAtual > statsFinais.vida) this->vidaAtual = statsFinais.vida;
}

void Personagem::mostrarStatus() const 
{
    std::cout << "[" << nomePersonagem << "] HP: " << vidaAtual << "/" << statsFinais.vida << std::endl;
}

std::string Personagem::obterNomeClasse() const 
{
    return this->classe->obterNomeClasse();
}

void Personagem::equiparItem(Item* item) 
{
    if (item == nullptr) return;
    if (item->obterTipo() == TipoEquipamento::ARMA) this->arma = item;
    else if (item->obterTipo() == TipoEquipamento::ESCUDO) this->escudo = item;
    else if (item->obterTipo() == TipoEquipamento::ARMADURA) this->armadura = item;
}

// CORREÇÕES: Adicionando as definições dos métodos que causavam o erro de linkagem

RacaBase* Personagem::obterRaca() const 
{
    return this->raca;
}

ClasseBase* Personagem::obterClasse() const 
{
    return this->classe;
}