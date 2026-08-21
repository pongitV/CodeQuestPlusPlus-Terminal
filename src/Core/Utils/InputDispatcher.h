#pragma once

#include <functional>
#include <unordered_map>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

class InputDispatcher {
public:
    using Action = std::function<void()>;

    void registerAction(int key, Action action) {
        if (action) actions[key] = std::move(action);
    }

    bool execute(int key) const {
        auto it = actions.find(key);
        if (it != actions.end()) {
            it->second();
            return true;
        }
        return false;
    }

    // [PT-BR] Verifica todas as teclas registradas; executa a primeira correspondencia encontrada
    // [EN-US] Checks all registered keys; executes the first match found
    using ActionWithReturn = std::function<char()>;
    struct PollEntry {
        int key;
        // [PT-BR] Retorna '\0' para continuar, qualquer outro valor para retornar
        // [EN-US] Returns '\0' to continue, any other value to return
        ActionWithReturn action;
    };

    void registerPoll(int key, ActionWithReturn action) {
        pollActions.push_back({key, std::move(action)});
    }

    char poll() const {
        for (const auto& e : pollActions) {
            if (GetAsyncKeyState(e.key) & 0x8000) {
                char result = e.action();
                if (result != '\0') return result;
            }
        }
        return '\0';
    }

    void clear() {
        actions.clear();
        pollActions.clear();
    }

private:
    std::unordered_map<int, Action> actions;
    std::vector<PollEntry> pollActions;
};
