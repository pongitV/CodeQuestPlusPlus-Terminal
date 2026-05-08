#pragma once

#include "../Sistemas/SistemaPersonagem.h"
#include "../Inventario/Inventario.h"
#include "../Inventario/Item.h"
#include "../Inventario/FabricaItens.h"
#include "../Utilidades/Aparencia.h"
#include "../Telas/TelaCombate.h"
#include <memory>
#include <functional>
#include <iostream>
#include <algorithm>

enum class TipoClasse 
{
    Nenhum,
    Arqueiro,
    Bardo,
    Guerreiro,
    Mago
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
    virtual ~ClasseBase() {}

    // INFORMACOES DA CLASSE
    virtual std::string obterNomeClasse() const = 0;
    virtual TipoClasse obterTipoClasse() const = 0;
    virtual const std::vector<std::string>& obterAparenciaClasseMenu() const = 0;
    virtual Atributos obterAtributosClasse() const = 0;
    virtual std::vector<std::unique_ptr<Item>> obterEquipamentoClasse() const = 0;
 
    virtual std::string obterNomePassivaClasse() const = 0;
    virtual std::string obterDescricaoPassivaClasse() const = 0;
    virtual std::string obterRecargaHabilidadeClasse() const = 0;

    // HABILIDADE DA CLASSE
    virtual std::string obterNomeHabilidadeClasse() const = 0;
    virtual std::string obterDescricaoHabilidadeClasse() const = 0;
    virtual void usarHabilidadeClasse(SistemaPersonagem* personagemUsuario, std::vector<SistemaPersonagem*>& listaDeInimigos) = 0;
    virtual TipoAtaque obterTipoAtaque() const { return TipoAtaque::UNICO; }
    virtual bool habilidadeConsomeTurno() const { return true; }

protected:
    void notificarMensagemCombate(const std::string& msgComCor, const std::string& msgSemCor) const {
        std::string msgFinal = Aparencia::margemCombate() + msgComCor + "\n";
        std::cout << msgFinal;
        TelaCombate::adicionarMensagemFixa(msgFinal);
        Aparencia::registrarLogBatalha(msgSemCor);
    }

    bool verificarEReportarRecarga(SistemaPersonagem* personagemUsuario, int turnosRestantes, const std::string& nomeHabilidade) const {
        if (turnosRestantes > 0) {
            std::cout << "\n" << Aparencia::margemCombate() << Aparencia::cor(Cor::VERMELHO) << "[SISTEMA]: A habilidade " << nomeHabilidade << " esta em recarga (" << turnosRestantes << " turnos)!" << Aparencia::cor(Cor::RESET) << "\n";
            Aparencia::registrarLogBatalha("[SISTEMA]: A habilidade " + nomeHabilidade + " esta em recarga (" + std::to_string(turnosRestantes) + " turnos)!");
            Aparencia::aguardarEnter();
            personagemUsuario->definirHabilidadeCancelada(true);
            return true;
        }
        return false;
    }

public:
    // PASSIVAS DE CLASSE
    virtual int processarCuraPassivaBardo(int curaBase) const { return curaBase; }
    virtual double processarMultiplicadorBuffPassivaBardo(double multBase) const { return multBase; }
    virtual int processarPenalidadeArmaduraPassivaArqueiro(int penalidadeBase) const { return penalidadeBase; }
    virtual int aplicarPenalidadeLentidaoPassivaArqueiro(int destrezaAtual) const { return destrezaAtual / 2; }
    virtual int reverterPenalidadeLentidaoPassivaArqueiro(int destrezaAtual) const { return destrezaAtual * 2; }

    // PROCESSAMENTO DE DANO (POLIMORFISMO)
    virtual void executarAtaqueComPassivaDaClasse(SistemaPersonagem* atacante, SistemaPersonagem* defensor, int danoBase, int danoPerfurante, std::vector<std::unique_ptr<SistemaPersonagem>>& inimigos, const std::function<void(SistemaPersonagem*, SistemaPersonagem*, int, int)>& aplicarDano, bool isAtacanteJogador) {

        danoBase = processarDanoPreAtaque(atacante, defensor, danoBase, isAtacanteJogador, inimigos.size());

        bool isArea = atacante->obterTipoAtaque() == TipoAtaque::AREA && isAtacanteJogador && !inimigos.empty();

        if (isArea) {
            executarAtaqueArea(atacante, defensor, danoBase, danoPerfurante, inimigos, aplicarDano, isAtacanteJogador);
        } else {
            executarAtaqueUnico(atacante, defensor, danoBase, danoPerfurante, inimigos, aplicarDano, isAtacanteJogador);
        }
    }

protected:
    virtual void executarAtaqueArea(SistemaPersonagem* atacante, SistemaPersonagem* defensor, int danoBase, int danoPerfurante, std::vector<std::unique_ptr<SistemaPersonagem>>& inimigos, const std::function<void(SistemaPersonagem*, SistemaPersonagem*, int, int)>& aplicarDano, bool isAtacanteJogador) {
        std::string msgInfo = atacante->obterNome() + " desfere um ataque em area!";
        notificarMensagemCombate(msgInfo, msgInfo);
        int danoDividido = std::max(1, danoBase / static_cast<int>(inimigos.size()));
        int perfuranteDividido = danoPerfurante / static_cast<int>(inimigos.size());

        bool ativouPassiva = false;
        for (auto& inimigoAtual : inimigos) {
            aplicarDano(atacante, inimigoAtual.get(), danoDividido, perfuranteDividido);
            processarDanoPosAtaque(atacante, inimigoAtual.get(), defensor, danoBase, danoPerfurante, aplicarDano, isAtacanteJogador, true, ativouPassiva);
        }
    }

    virtual void executarAtaqueUnico(SistemaPersonagem* atacante, SistemaPersonagem* defensor, int danoBase, int danoPerfurante, std::vector<std::unique_ptr<SistemaPersonagem>>& inimigos, const std::function<void(SistemaPersonagem*, SistemaPersonagem*, int, int)>& aplicarDano, bool isAtacanteJogador) {
        if (defensor != nullptr) {
            std::string msgInfo = atacante->obterNome() + " ataca " + defensor->obterNome() + "!";
            notificarMensagemCombate(msgInfo, msgInfo);
            aplicarDano(atacante, defensor, danoBase, danoPerfurante);
        }

        bool ativouPassiva = false;
        for (auto& inimigoAtual : inimigos) {
            processarDanoPosAtaque(atacante, inimigoAtual.get(), defensor, danoBase, danoPerfurante, aplicarDano, isAtacanteJogador, false, ativouPassiva);
        }
    }

    virtual int processarDanoPreAtaque(SistemaPersonagem* atacante, SistemaPersonagem* defensor, int danoBase, bool isAtacanteJogador, size_t qtdInimigos) { return danoBase; }
    virtual void processarDanoPosAtaque(SistemaPersonagem* atacante, SistemaPersonagem* alvoAtual, SistemaPersonagem* defensorPrincipal, int danoBase, int danoPerfurante, const std::function<void(SistemaPersonagem*, SistemaPersonagem*, int, int)>& aplicarDano, bool isAtacanteJogador, bool isArea, bool& ativouPassiva) {}
};