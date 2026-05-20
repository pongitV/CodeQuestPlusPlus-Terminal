#pragma once

#include "ClasseBase.h"

class Item;

class Mago : public ClasseBase
{
private:
    TipoAtaque tipoAtaqueAtual = TipoAtaque::UNICO;

public:
    // INFORMACOES DA CLASSE
    std::string obterNomeClasse() const override;
    TipoClasse obterTipoClasse() const override { return TipoClasse::Mago; }
    const std::vector<std::string>& obterAparenciaClasseMenu() const override;
    Atributos obterAtributosClasse() const override;
    std::vector<std::unique_ptr<Item>> obterEquipamentoClasse() const override;

    // PASSIVA DA CLASSE
    std::string obterNomePassivaClasse() const override;
    std::string obterDescricaoPassivaClasse() const override;

    // HABILIDADE DA CLASSE
    std::string obterRecargaHabilidadeClasse() const override;
    std::string obterNomeHabilidadeClasse() const override;
    std::string obterDescricaoHabilidadeClasse() const override;
    void usarHabilidadeClasse(Personagem* personagemUsuario, std::vector<Personagem*>& listaDeInimigos) override;

protected:
    // PROCESSAMENTO DE DANO 
    int processarDanoPreAtaque(Personagem* atacante, Personagem* defensor, int danoBase, bool isAtacanteJogador, size_t qtdInimigos) override;
    void processarDanoPosAtaque(Personagem* atacante, Personagem* alvoAtual, Personagem* defensorPrincipal, int danoBase, int danoPerfurante, const std::function<void(Personagem*, Personagem*, int, int)>& aplicarDano, bool isAtacanteJogador, bool isArea, bool& ativouPassiva) override;
};
