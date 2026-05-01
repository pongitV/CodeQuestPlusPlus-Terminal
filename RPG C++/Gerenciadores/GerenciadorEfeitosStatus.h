#pragma once

#include <string>

enum class EfeitoID {
    Nenhum,
    Sangramento,
    Lentidao,
    Fraqueza,
    QuebraResistencia,
    Inviolavel,
    MetadeDano,
    SugaSangue,
    Atordoamento,
    BuffAtributos,
    GritoDeGuerra
};

class SistemaPersonagem; // Forward declaration

class EfeitoStatus {
protected:
    EfeitoID id;
    std::string nome;
    int turnosRestantes;
public:
    EfeitoStatus(EfeitoID id, const std::string& n, int t) : id(id), nome(n), turnosRestantes(t) {}
    virtual ~EfeitoStatus() = default;
    
    EfeitoID obterID() const { return id; }
    std::string obterNome() const { return nome; }
    int obterTurnosRestantes() const { return turnosRestantes; }
    void decrementarTurno() { turnosRestantes--; }
    bool expirou() const { return turnosRestantes <= 0; }
    
    virtual void aoEntrar(SistemaPersonagem* alvo) {}
    virtual void aplicarInicioTurno(SistemaPersonagem* alvo) {}
    virtual void aoSair(SistemaPersonagem* alvo) {}
    
    virtual int processarDanoRecebido(int dano) { return dano; }
    virtual bool impedeAcao() const { return false; }
};

class EfeitoAtordoamento : public EfeitoStatus {
public:
    EfeitoAtordoamento(int t) : EfeitoStatus(EfeitoID::Atordoamento, "Atordoamento", t) {}
    bool impedeAcao() const override { return true; }
};

class EfeitoSugaSangue : public EfeitoStatus {
private:
    SistemaPersonagem* atacante;
public:
    EfeitoSugaSangue(int t, SistemaPersonagem* atk) : EfeitoStatus(EfeitoID::SugaSangue, "SugaSangue", t), atacante(atk) {}
    void aplicarInicioTurno(SistemaPersonagem* alvo) override;
};

class EfeitoLentidao : public EfeitoStatus {
public:
    EfeitoLentidao(int t) : EfeitoStatus(EfeitoID::Lentidao, "Lentidao", t) {}
    void aoEntrar(SistemaPersonagem* alvo) override;
    void aoSair(SistemaPersonagem* alvo) override;
};

class EfeitoFraqueza : public EfeitoStatus {
private:
    int forcaPerdida;
public:
    EfeitoFraqueza(int t) : EfeitoStatus(EfeitoID::Fraqueza, "Fraqueza", t), forcaPerdida(0) {}
    void aoEntrar(SistemaPersonagem* alvo) override;
    void aoSair(SistemaPersonagem* alvo) override;
};

class EfeitoQuebraResistencia : public EfeitoStatus {
private:
    int resistenciaPerdida;
    int constituicaoPerdida;
public:
    EfeitoQuebraResistencia(int t) : EfeitoStatus(EfeitoID::QuebraResistencia, "QuebraResistencia", t), resistenciaPerdida(0), constituicaoPerdida(0) {}
    void aoEntrar(SistemaPersonagem* alvo) override;
    void aoSair(SistemaPersonagem* alvo) override;
};

class EfeitoSangramento : public EfeitoStatus {
private:
    int danoPorTurno;
public:
    EfeitoSangramento(int t, int dano) : EfeitoStatus(EfeitoID::Sangramento, "Sangramento", t), danoPorTurno(dano) {}
    void aplicarInicioTurno(SistemaPersonagem* alvo) override;
};

class EfeitoMetadeDano : public EfeitoStatus {
public:
    EfeitoMetadeDano(int t) : EfeitoStatus(EfeitoID::MetadeDano, "MetadeDano", t) {}
    int processarDanoRecebido(int dano) override;
};

class EfeitoBuffAtributos : public EfeitoStatus {
public:
    EfeitoBuffAtributos(int t) : EfeitoStatus(EfeitoID::BuffAtributos, "BuffAtributos", t) {}
    void aoSair(SistemaPersonagem* alvo) override;
};

class EfeitoInviolavel : public EfeitoStatus {
public:
    EfeitoInviolavel(int t) : EfeitoStatus(EfeitoID::Inviolavel, "Inviolavel", t) {}
};

class EfeitoGritoGuerra : public EfeitoStatus {
private:
    int bonusForca;
    int bonusDestreza;
public:
    EfeitoGritoGuerra(int t, int f, int d) : EfeitoStatus(EfeitoID::GritoDeGuerra, "GritoDeGuerra", t), bonusForca(f), bonusDestreza(d) {}
    void aoEntrar(SistemaPersonagem* alvo) override;
    void aoSair(SistemaPersonagem* alvo) override;
};
