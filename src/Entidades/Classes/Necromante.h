#pragma once

#include "ClasseBase.h"

class Necromante : public ClasseBase {
public:
    // --- INFORMACOES DA CLASSE ---
    std::string obterNomeClasse() const override;
    TipoClasse obterTipoClasse() const override { return TipoClasse::NECROMANTE; }
    const std::vector<std::string>& obterAparenciaClasseMenu() const override;
    Atributos obterAtributosClasse() const override;
    std::vector<std::unique_ptr<Item>> obterEquipamentoClasse() const override;

    // --- HABILIDADE DA CLASSE ---
    std::string obterNomeHabilidadeClasse() const override;
    std::string obterDescricaoHabilidadeClasse() const override;
    std::string obterRecargaHabilidadeClasse() const override;
    void usarHabilidadeClasse(Combate* combate, Personagem* personagemUsuario, std::vector<Personagem*>& listaDeInimigos) override;

    // --- PASSIVA DA CLASSE ---
    std::string obterNomePassivaClasse() const override;
    std::string obterDescricaoPassivaClasse() const override;
    void executarAtaqueComPassivaDaClasse(Personagem* atacante, Personagem* defensor, int danoBase, int danoPerfurante, std::vector<std::unique_ptr<Personagem>>& listaDeInimigos, const std::function<void(Personagem*, Personagem*, int, int)>& aplicarDano, bool aplicarPassiva) override;
};