#pragma once

#include <string>

enum class EfeitoID {
    Nenhum,
    Sangramento,
    Lentidao,
    Fraqueza,
    QuebraResistencia,
    RodaAdaptacao,
    Inviolavel,
    MetadeDano,
    SugaSangue,
    Atordoamento,
    BuffAtributos,
    GritoDeGuerra
};

class Personagem; // Forward declaration

class EfeitoStatus {
protected:
    EfeitoID id;
    std::string nome;
    int turnosRestantes;
public:
    EfeitoStatus(EfeitoID id, const std::string& nomeEfeito, int turnosDuracao) : id(id), nome(nomeEfeito), turnosRestantes(turnosDuracao) {}
    virtual ~EfeitoStatus() = default;
    
    EfeitoID obterID() const { return id; }
    std::string obterNome() const { return nome; }
    int obterTurnosRestantes() const { return turnosRestantes; }
    void decrementarTurno() { turnosRestantes--; }
    bool expirou() const { return turnosRestantes <= 0; }
    
    virtual void aoEntrar(Personagem* /*alvo*/) {}
    virtual void aplicarInicioTurno(Personagem* /*alvo*/) {}
    virtual void aoSair(Personagem* /*alvo*/) {}
    
    virtual int processarDanoRecebido(int dano) { return dano; }
    virtual bool impedeAcao() const { return false; }
};

class EfeitoAtordoamento : public EfeitoStatus {
public:
    EfeitoAtordoamento(int turnosDuracao) : EfeitoStatus(EfeitoID::Atordoamento, "Atordoamento", turnosDuracao) {}
    bool impedeAcao() const override { return true; }
};

class EfeitoSugaSangue : public EfeitoStatus {
private:
    Personagem* atacante;
public:
    EfeitoSugaSangue(int turnosDuracao, Personagem* personagemAtacante) : EfeitoStatus(EfeitoID::SugaSangue, "SugaSangue", turnosDuracao), atacante(personagemAtacante) {}
    void aplicarInicioTurno(Personagem* alvo) override;
};

class EfeitoLentidao : public EfeitoStatus {
public:
    EfeitoLentidao(int turnosDuracao) : EfeitoStatus(EfeitoID::Lentidao, "Lentidao", turnosDuracao) {}
    void aoEntrar(Personagem* alvo) override;
    void aoSair(Personagem* alvo) override;
};

class EfeitoFraqueza : public EfeitoStatus {
private:
    int forcaPerdida;
public:
    EfeitoFraqueza(int turnosDuracao) : EfeitoStatus(EfeitoID::Fraqueza, "Fraqueza", turnosDuracao), forcaPerdida(0) {}
    void aoEntrar(Personagem* alvo) override;
    void aoSair(Personagem* alvo) override;
};

class EfeitoQuebraResistencia : public EfeitoStatus {
private:
    int resistenciaPerdida;
    int constituicaoPerdida;
public:
    EfeitoQuebraResistencia() : EfeitoStatus(EfeitoID::QuebraResistencia, "QuebraResistencia", 9999), resistenciaPerdida(0), constituicaoPerdida(0) {}
    void aoEntrar(Personagem* alvo) override;
    void aoSair(Personagem* alvo) override;
    void aplicarInicioTurno(Personagem* alvo) override;
};

class EfeitoSangramento : public EfeitoStatus {
private:
    int danoPorTurno;
public:
    EfeitoSangramento(int turnosDuracao, int dano) : EfeitoStatus(EfeitoID::Sangramento, "Sangramento", turnosDuracao), danoPorTurno(dano) {}
    void aplicarInicioTurno(Personagem* alvo) override;
};

class EfeitoMetadeDano : public EfeitoStatus {
public:
    EfeitoMetadeDano(int turnosDuracao) : EfeitoStatus(EfeitoID::MetadeDano, "MetadeDano", turnosDuracao) {}
    int processarDanoRecebido(int dano) override;
};

class EfeitoBuffAtributos : public EfeitoStatus {
public:
    EfeitoBuffAtributos(int turnosDuracao) : EfeitoStatus(EfeitoID::BuffAtributos, "BuffAtributos", turnosDuracao) {}
    void aoSair(Personagem* alvo) override;
};

class EfeitoInviolavel : public EfeitoStatus {
public:
    EfeitoInviolavel(int turnosDuracao) : EfeitoStatus(EfeitoID::Inviolavel, "Inviolavel", turnosDuracao) {}
};

class EfeitoGritoGuerra : public EfeitoStatus {
private:
    int bonusForca;
    int bonusDestreza;
public:
    EfeitoGritoGuerra(int turnosDuracao, int forcaBonus, int destrezaBonus) : EfeitoStatus(EfeitoID::GritoDeGuerra, "GritoDeGuerra", turnosDuracao), bonusForca(forcaBonus), bonusDestreza(destrezaBonus) {}
    void aoEntrar(Personagem* alvo) override;
    void aoSair(Personagem* alvo) override;
};

class EfeitoRodaAdaptacao : public EfeitoStatus {
private:
    int bForca = 0;
    int bDestreza = 0;
    int bResistencia = 0;
    int bConstituicao = 0;
    int bInteligencia = 0;
    int bSabedoria = 0;
public:
    EfeitoRodaAdaptacao() : EfeitoStatus(EfeitoID::RodaAdaptacao, "Adaptacao Divina", 9999) {}
    bool impedeAcao() const override { return false; }
    void aplicarInicioTurno(Personagem* alvo) override;
    void aoSair(Personagem* alvo) override;
    void adaptar(Personagem* alvo, Personagem* inimigo);
};
