#include "ClasseBase.h"

#pragma once

class Item;

class Mago : public ClasseBase
{
private:
    TipoAtaque tipoAtaqueAtual = TipoAtaque::UNICO;

public:
    // Metodos específicos para a classe Mago
    std::string obterNomeClasse() const override;
    TipoClasse obterTipoClasse() const override { return TipoClasse::Mago; }
    const std::vector<std::string>& obterAparenciaClasseMenu() const override;
    Atributos obterAtributosClasse() const override;
    std::vector<std::unique_ptr<Item>> obterEquipamentoClasse() const override;

    std::string obterNomePassivaClasse() const override;
    std::string obterDescricaoPassivaClasse() const override;
    std::string obterRecargaHabilidadeClasse() const override;

    // Criação e Definição da habilidade do Mago: "Estratégia Arcana"
    void usarHabilidadeClasse(SistemaPersonagem* personagemUsuario, std::vector<SistemaPersonagem*>& listaDeInimigos) override;
    std::string obterNomeHabilidadeClasse() const override;
    std::string obterDescricaoHabilidadeClasse() const override;
    
    TipoAtaque obterTipoAtaque() const override;
    bool habilidadeConsomeTurno() const override;

protected:
    int processarDanoPreAtaque(SistemaPersonagem* atacante, SistemaPersonagem* defensor, int danoBase, bool isAtacanteJogador, size_t qtdInimigos) override;
    void processarDanoPosAtaque(SistemaPersonagem* atacante, SistemaPersonagem* alvoAtual, SistemaPersonagem* defensorPrincipal, int danoBase, int danoPerfurante, const std::function<void(SistemaPersonagem*, SistemaPersonagem*, int, int)>& aplicarDano, bool isAtacanteJogador, bool isArea, bool& ativouPassiva) override;

};