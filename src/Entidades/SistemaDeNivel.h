#pragma once

class SistemaDeNivel {
private:
    int nivel;
    int xpAtual;
    int xpParaSubir;

public:
    SistemaDeNivel(int nivelInicial = 1, int xpInicial = 0, int xpSubirInicial = 100) 
        : nivel(nivelInicial), xpAtual(xpInicial), xpParaSubir(xpSubirInicial) {}

    int obterNivel() const { return nivel; }
    int obterXpAtual() const { return xpAtual; }
    int obterXpParaSubir() const { return xpParaSubir; }

    void definirNivel(int novoNivel) { nivel = novoNivel; }
    void definirXpAtual(int novoXp) { xpAtual = novoXp; }
    void definirXpParaSubir(int novoXpParaSubir) { xpParaSubir = novoXpParaSubir; }

    void ganharXp(int valor) { xpAtual += valor; }
    bool podeSubirDeNivel() const { return xpAtual >= xpParaSubir; }
};
