#include "../Sistemas/SistemaPersonagem.h"
#include "../Inventario/Inventario.h"
#include "../Inventario/Item.h"
#include <memory>
#include <functional>
#include <iostream>
#include <algorithm>

#pragma once

enum class TipoClasse 
{
    Nenhum,
    Arqueiro,
    Bardo,
    Guerreiro,
    Mago,
    InimigoPadrao
};

enum class HabilidadeID
{
    Nenhuma,
    Determinacao,
    EstrategiaArcana,
    FlashingLights,
    OnSight,
    ThroughTheWire
};

class SistemaPersonagem;
class Item;

class ClasseBase 
{
public:
    // Destrutor virtual para garantir a destruicao polimorfica correta das subclasses
    virtual ~ClasseBase() {}

    // INFORMACOES DA CLASSE
    virtual std::string obterNomeClasse() const = 0;
    virtual TipoClasse obterTipoClasse() const = 0;
    virtual std::vector<std::string> obterAparenciaClasseMenu() const = 0;
    virtual Atributos obterAtributosClasse() const = 0;
    virtual std::vector<std::unique_ptr<Item>> obterEquipamentoClasse() const = 0;
 
    virtual std::string obterNomePassivaClasse() const = 0;
    virtual std::string obterDescricaoPassivaClasse() const = 0;
    virtual std::string obterRecargaHabilidadeClasse() const = 0;

    // HABILIDADE DA CLASSE
    virtual std::string obterNomeHabilidadeClasse() const = 0;
    virtual std::string obterDescricaoHabilidadeClasse() const = 0;
    virtual void usarHabilidadeClasse(SistemaPersonagem* usuario, std::vector<SistemaPersonagem*>& inimigos) = 0;
    virtual TipoAtaque obterTipoAtaque() const = 0;
    virtual bool habilidadeConsomeTurno() const = 0;

    // PASSIVAS DE CLASSE
    virtual int processarCuraPassivaBardo(int curaBase) const { return curaBase; }
    virtual double processarMultiplicadorBuffPassivaBardo(double multBase) const { return multBase; }
    virtual int processarPenalidadeArmaduraPassivaArqueiro(int penalidadeBase) const { return penalidadeBase; }
    virtual int aplicarPenalidadeLentidaoPassivaArqueiro(int destrezaAtual) const { return destrezaAtual / 2; }
    virtual int reverterPenalidadeLentidaoPassivaArqueiro(int destrezaAtual) const { return destrezaAtual * 2; }

    // PROCESSAMENTO DE DANO (POLIMORFISMO)
    virtual void executarAtaqueComPassivaDaClasse(SistemaPersonagem* atacante, SistemaPersonagem* defensor, int danoBase, int danoPerfurante, std::vector<std::unique_ptr<SistemaPersonagem>>& inimigos, std::function<void(SistemaPersonagem*, SistemaPersonagem*, int, int)> aplicarDano) {
        bool isAtacanteJogador = true;
        for (const auto& ini : inimigos) {
            if (ini.get() == atacante) { isAtacanteJogador = false; break; }
        }

        if (atacante->obterTipoAtaque() == TipoAtaque::AREA && isAtacanteJogador && !inimigos.empty()) {
            int danoDividido = std::max(1, danoBase / static_cast<int>(inimigos.size()));
            int perfuranteDividido = danoPerfurante / static_cast<int>(inimigos.size());
            std::cout << atacante->obterNome() << " desfere um ataque em area!\n";
            for (auto& ini : inimigos) {
                aplicarDano(atacante, ini.get(), danoDividido, perfuranteDividido);
            }
        } else if (defensor != nullptr) {
            std::cout << atacante->obterNome() << " ataca " << defensor->obterNome() << "!\n";
            aplicarDano(atacante, defensor, danoBase, danoPerfurante);
        }
    }

};