#pragma once
#include <exception>

class PerspectivaAlteradaException : public std::exception {
public:
    const char* what() const noexcept override {
        return "Perspectiva alterada pelo usuario.";
    }
};
