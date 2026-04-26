#pragma once
#include <string>

class Personagem; // Forward declaration

class EfeitoStatus {
protected:
    std::string nome;
    int turnosRestantes;
public:
    EfeitoStatus(const std::string& n, int t) : nome(n), turnosRestantes(t) {}
    virtual ~EfeitoStatus() = default;
    
    std::string obterNome() const { return nome; }
    int obterTurnosRestantes() const { return turnosRestantes; }
    void decrementarTurno() { turnosRestantes--; }
    bool expirou() const { return turnosRestantes <= 0; }
    
    virtual void aoEntrar(Personagem* alvo) {}
    virtual void aplicarInicioTurno(Personagem* alvo) {}
    virtual void aoSair(Personagem* alvo) {}
    
    virtual int processarDanoRecebido(int dano) { return dano; }
    virtual bool impedeAcao() const { return false; }
};

class EfeitoAtordoamento : public EfeitoStatus {
public:
    EfeitoAtordoamento(int t) : EfeitoStatus("Atordoamento", t) {}
    bool impedeAcao() const override { return true; }
};

class EfeitoSugaSangue : public EfeitoStatus {
private:
    Personagem* atacante;
public:
    EfeitoSugaSangue(int t, Personagem* atk) : EfeitoStatus("SugaSangue", t), atacante(atk) {}
    void aplicarInicioTurno(Personagem* alvo) override;
};

class EfeitoLentidao : public EfeitoStatus {
public:
    EfeitoLentidao(int t) : EfeitoStatus("Lentidao", t) {}
    void aoEntrar(Personagem* alvo) override;
    void aoSair(Personagem* alvo) override;
};

class EfeitoFraqueza : public EfeitoStatus {
private:
    int forcaPerdida;
public:
    EfeitoFraqueza(int t) : EfeitoStatus("Fraqueza", t), forcaPerdida(0) {}
    void aoEntrar(Personagem* alvo) override;
    void aoSair(Personagem* alvo) override;
};

class EfeitoQuebraResistencia : public EfeitoStatus {
private:
    int resistenciaPerdida;
    int constituicaoPerdida;
public:
    EfeitoQuebraResistencia(int t) : EfeitoStatus("QuebraResistencia", t), resistenciaPerdida(0), constituicaoPerdida(0) {}
    void aoEntrar(Personagem* alvo) override;
    void aoSair(Personagem* alvo) override;
};

class EfeitoSangramento : public EfeitoStatus {
private:
    int danoPorTurno;
public:
    EfeitoSangramento(int t, int dano) : EfeitoStatus("Sangramento", t), danoPorTurno(dano) {}
    void aplicarInicioTurno(Personagem* alvo) override;
};

class EfeitoMetadeDano : public EfeitoStatus {
public:
    EfeitoMetadeDano(int t) : EfeitoStatus("MetadeDano", t) {}
    int processarDanoRecebido(int dano) override;
};

class EfeitoBuffAtributos : public EfeitoStatus {
public:
    EfeitoBuffAtributos(int t) : EfeitoStatus("BuffAtributos", t) {}
    void aoSair(Personagem* alvo) override;
};

class EfeitoInviolavel : public EfeitoStatus {
public:
    EfeitoInviolavel(int t) : EfeitoStatus("Inviolavel", t) {}
};

class EfeitoGritoGuerra : public EfeitoStatus {
private:
    int bonusForca;
    int bonusDestreza;
public:
    EfeitoGritoGuerra(int t, int f, int d) : EfeitoStatus("GritoDeGuerra", t), bonusForca(f), bonusDestreza(d) {}
    void aoEntrar(Personagem* alvo) override;
    void aoSair(Personagem* alvo) override;
};
