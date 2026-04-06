#include <iostream>
#include <algorithm>
#include <cctype>

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
    this->ouroRecompensa = 15;
    
    this->nivel = 1;
    this->xpAtual = 0;
    this->xpParaSubir = 100;
    this->xpRecompensa = 40;

    this->multiplicadorAtual = 1.0;
    this->estaDefendendo = false;
    this->recargaDefesa = false;
    this->estaInviolavel = false;
    this->turnosBuff = 0;
    this->recargaHabilidade = false;
    this->modoAtaqueArea = false;
    this->pularTurnoInimigo = false;
    this->parryAtivado = false;
    this->dificuldadeAtual = 2; // Padrao: Normal

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

bool Personagem::subirDeNivel(std::string atributo) 
{
    if (xpAtual < xpParaSubir) return false;

    std::transform(atributo.begin(), atributo.end(), atributo.begin(),
                   [](unsigned char c){ return std::tolower(c); });

    bool upou = false;
    if (atributo == "vida") {
        statsFinais.vida += 20;
        vidaAtual += 20;
        upou = true;
    }
    else if (atributo == "forca" || atributo == "força") { statsFinais.forca += 5; upou = true; }
    else if (atributo == "destreza") { statsFinais.destreza += 5; upou = true; }
    else if (atributo == "resistencia" || atributo == "resistência") { statsFinais.resistencia += 5; upou = true; }
    else if (atributo == "constituicao" || atributo == "constituição") { statsFinais.constituicao += 5; upou = true; }
    else if (atributo == "inteligencia" || atributo == "inteligência") { statsFinais.inteligencia += 5; upou = true; }
    else if (atributo == "sabedoria") { statsFinais.sabedoria += 5; upou = true; }

    if (upou) {
        xpAtual -= xpParaSubir;
        xpParaSubir = static_cast<int>(xpParaSubir * 1.5);
        nivel++;
        return true;
    }
    return false;
}

void Personagem::calcularAtributos() 
{
    this->statsFinais.calcularAtributos(raca->obterAtributosRaca());
    this->statsFinais.calcularAtributos(classe->obterAtributosClasse());
    this->vidaAtual = statsFinais.vida;
}

void Personagem::aplicarMultiplicadorDificuldade(double mult) 
{
    if (mult <= 1.0) return; // Facil nao sofre alteracao (1x)
    this->statsFinais.vida = static_cast<int>(this->statsFinais.vida * mult);
    this->statsFinais.forca = static_cast<int>(this->statsFinais.forca * mult);
    this->statsFinais.destreza = static_cast<int>(this->statsFinais.destreza * mult);
    this->statsFinais.resistencia = static_cast<int>(this->statsFinais.resistencia * mult);
    this->statsFinais.constituicao = static_cast<int>(this->statsFinais.constituicao * mult);
    this->statsFinais.inteligencia = static_cast<int>(this->statsFinais.inteligencia * mult);
    this->statsFinais.sabedoria = static_cast<int>(this->statsFinais.sabedoria * mult);
    this->vidaAtual = this->statsFinais.vida;
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