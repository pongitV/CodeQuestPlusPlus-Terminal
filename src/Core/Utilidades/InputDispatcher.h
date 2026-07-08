#pragma once

#include <functional>
#include <unordered_map>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

class InputDispatcher {
public:
    using Acao = std::function<void()>;

    void registrar(int tecla, Acao acao) {
        if (acao) acoes[tecla] = std::move(acao);
    }

    bool executar(int tecla) const {
        auto it = acoes.find(tecla);
        if (it != acoes.end()) {
            it->second();
            return true;
        }
        return false;
    }

    // Poll all registered keys using GetAsyncKeyState; executes the first match found.
    using AcaoComRetorno = std::function<char()>;
    struct PollEntry {
        int tecla;
        AcaoComRetorno acao; // return '\\0' to continue, anything else to return
    };

    void registrarPoll(int tecla, AcaoComRetorno acao) {
        pollAcoes.push_back({tecla, std::move(acao)});
    }

    char poll() const {
        for (const auto& e : pollAcoes) {
            if (GetAsyncKeyState(e.tecla) & 0x8000) {
                char resultado = e.acao();
                if (resultado != '\0') return resultado;
            }
        }
        return '\0';
    }

    void limpar() {
        acoes.clear();
        pollAcoes.clear();
    }

private:
    std::unordered_map<int, Acao> acoes;
    std::vector<PollEntry> pollAcoes;
};
