#include "AparenciaTerminal.h"
#include "Aparencia.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

// --- CORES DA PALETA C++ ---
std::string AparenciaTerminal::corPalavraChave() { return "\033[38;2;86;156;214m"; } // Azul (palavra chave)
std::string AparenciaTerminal::corTipo() { return "\033[38;2;78;201;176m"; }          // Turquesa (tipo)
std::string AparenciaTerminal::corString() { return "\033[38;2;214;157;133m"; }        // Laranja (string)
std::string AparenciaTerminal::corNumero() { return "\033[38;2;181;206;168m"; }        // Verde claro (numero)
std::string AparenciaTerminal::corComentario() { return "\033[38;2;96;139;78m"; }       // Verde escuro (comentario)
std::string AparenciaTerminal::corFuncao() { return "\033[38;2;220;220;170m"; }         // Amarelo claro (funcao)
std::string AparenciaTerminal::corVariavel() { return "\033[38;2;156;220;254m"; }       // Azul claro (variavel)
std::string AparenciaTerminal::corOperador() { return "\033[38;2;180;180;180m"; }       // Cinza (operador)
std::string AparenciaTerminal::corReset() { return "\033[0m"; }

// --- FUNCOES DE BARRA DE VIDA COM CARACTERES ASCII ---
std::string AparenciaTerminal::gerarBarraVidaASCII(double pct, int tamanho) {
    if (pct < 0.0) pct = 0.0;
    if (pct > 1.0) pct = 1.0;
    
    std::string barra;
    barra = corComentario() + "{";
    
    int qtdCheia = static_cast<int>(pct * tamanho);
    int qtdVazia = tamanho - qtdCheia;
    
    for (int i = 0; i < qtdCheia; ++i) {
        barra += corVariavel() + "=";
    }
    for (int i = 0; i < qtdVazia; ++i) {
        barra += corComentario() + "0";
    }
    
    barra += corReset() + "}";
    barra += corComentario() + " // ";
    barra += corNumero() + std::to_string(static_cast<int>(pct * 100)) + "%";
    barra += corReset();
    
    return barra;
}

std::string AparenciaTerminal::gerarBarraVidaIDE(double pct, int tamanho) {
    if (pct < 0.0) pct = 0.0;
    if (pct > 1.0) pct = 1.0;
    
    std::string barra;
    barra = corComentario() + "{";
    
    int qtdCheia = static_cast<int>(pct * tamanho);
    int qtdVazia = tamanho - qtdCheia;
    
    for (int i = 0; i < qtdCheia; ++i) {
        barra += corVariavel() + "=";
    }
    for (int i = 0; i < qtdVazia; ++i) {
        barra += corComentario() + "0";
    }
    
    barra += corReset() + "}";
    barra += corComentario() + " // " + std::to_string(static_cast<int>(pct * 100)) + "%";
    barra += corReset();
    
    return barra;
}

// --- ESTILIZACAO DE TEXTO EM ESTILO CODIGO ---
std::string AparenciaTerminal::estilizarComoCodigo(const std::string& texto, const std::string& tipo) {
    if (tipo == "string") {
        return corComentario() + "// " + corString() + "\"" + texto + "\"" + corReset();
    } else if (tipo == "tipo") {
        return corComentario() + "typedef " + corTipo() + texto + corReset();
    } else if (tipo == "numero") {
        return corComentario() + "const int " + corNumero() + texto + corReset();
    } else {
        return corComentario() + "/* " + texto + " */" + corReset();
    }
}

std::string AparenciaTerminal::estilizarComoTipo(const std::string& texto) {
    return corComentario() + "class " + corTipo() + texto + corReset();
}

std::string AparenciaTerminal::estilizarComoFuncao(const std::string& texto) {
    return corComentario() + "void " + corFuncao() + "(" + texto + ")" + corReset();
}

std::string AparenciaTerminal::estilizarComoComentario(const std::string& texto) {
    return corComentario() + "// " + texto + corReset();
}

std::string AparenciaTerminal::estilizarComoNumero(const std::string& texto) {
    return corComentario() + "int " + corNumero() + texto + corReset();
}

// --- CONSTRUCAO DE CAIXAS EM ESTILO CODIGO ---
std::vector<std::string> AparenciaTerminal::criarCaixaCodigo(const std::vector<std::string>& conteudo, const std::string& titulo) {
    std::vector<std::string> caixa;
    
    std::string tituloLimpo = titulo;
    std::replace(tituloLimpo.begin(), tituloLimpo.end(), ' ', '_');
    
    caixa.push_back(corPalavraChave() + "struct " + corTipo() + tituloLimpo + " {");
    
    for (const auto& linha : conteudo) {
        // Tenta detectar se eh uma linha chave:valor
        size_t pos = linha.find(":");
        if (pos != std::string::npos) {
            std::string chave = linha.substr(0, pos);
            std::string valor = linha.substr(pos + 1);
            chave.erase(std::remove_if(chave.begin(), chave.end(), [](char c) { return c == ' '; }), chave.end());
            
            std::string linhaFormatada = "    " + corPalavraChave() + "auto " + corVariavel() + chave + corReset() + " = " + corNumero() + valor + corReset() + ";";
            
            // Adiciona padding para alinhar
            int maxComp = 0;
            for (const auto& l : conteudo) {
                size_t p = l.find(":");
                if (p != std::string::npos) {
                    std::string c = l.substr(0, p);
                    int compChave = Aparencia::obterComprimentoVisual(c);
                    if (compChave > maxComp) maxComp = compChave;
                }
            }
            int padding = maxComp + 4 - Aparencia::obterComprimentoVisual(chave);
            if (padding > 0) {
                linhaFormatada = "    " + corPalavraChave() + "auto " + corVariavel() + chave + std::string(padding, ' ') + corReset() + " = " + corNumero() + valor + corReset() + ";";
            }
            
            caixa.push_back(linhaFormatada);
        } else {
            caixa.push_back("    " + linha + ";");
        }
    }
    
    caixa.push_back("};");
    
    return caixa;
}

std::vector<std::string> AparenciaTerminal::criarCaixaDrop(const std::vector<std::string>& drops) {
    std::vector<std::string> caixa;
    
    caixa.push_back(corPalavraChave() + "class " + corTipo() + "DropReward" + " {");
    caixa.push_back(corPalavraChave() + "public:");
    
    for (const auto& drop : drops) {
        std::string cleanDrop = drop;
        // Limpa cores ANSI
        cleanDrop = Aparencia::removerCoresANSI(cleanDrop);
        
        std::string linha = "    " + corPalavraChave() + "auto " + corVariavel() + "drop = " + corString() + "\"" + cleanDrop + "\"" + corReset() + ";";
        caixa.push_back(linha);
    }
    
    caixa.push_back("};");
    
    return caixa;
}

// --- IMPRESSOES ESPECIFICAS DO TERMINAL ---
void AparenciaTerminal::imprimirTituloComoCodigo(const std::string& titulo) {
    std::string tituloIDE = titulo;
    std::replace(tituloIDE.begin(), tituloIDE.end(), ' ', '_');
    
    std::string linha = corPalavraChave() + "class " + corTipo() + tituloIDE + " {";
    Aparencia::imprimirCentralizado(linha);
}

void AparenciaTerminal::imprimirStatsComoStruct(const std::vector<std::pair<std::string, std::string>>& campos) {
    std::vector<std::string> linhas;
    
    linhas.push_back(corPalavraChave() + "struct " + corTipo() + "StatsCombate" + " {");
    
    for (const auto& [nome, valor] : campos) {
        std::string linha = "    " + corPalavraChave() + "int " + corVariavel() + nome + " = " + corNumero() + valor + ";";
        linhas.push_back(linha);
    }
    
    linhas.push_back("};");
    
    Aparencia::imprimirBlocoCentralizado(linhas);
}

void AparenciaTerminal::imprimirLogsCombate(const std::vector<std::string>& mensagens) {
    std::vector<std::string> log;
    
    log.push_back(corPalavraChave() + "namespace " + corTipo() + "CombateLog" + " {");
    
    for (const auto& msg : mensagens) {
        std::string cleanMsg = Aparencia::removerCoresANSI(msg);
        log.push_back("    " + corPalavraChave() + "auto " + corVariavel() + "msg = " + corString() + "\"" + cleanMsg + "\"" + corReset() + ";");
    }
    
    log.push_back("}");
    
    Aparencia::imprimirBlocoCentralizado(log);
}

void AparenciaTerminal::imprimirCaixaDrops(const std::vector<std::string>& drops) {
    std::vector<std::string> caixa = criarCaixaDrop(drops);
    Aparencia::imprimirBlocoCentralizado(caixa);
}

// --- UTILIDADES DE FORMATAO ---
std::string AparenciaTerminal::formatarVarNome(const std::string& nome) {
    return corVariavel() + nome + corReset();
}

std::string AparenciaTerminal::formatarTipo(const std::string& tipo) {
    return corTipo() + tipo + corReset();
}

std::string AparenciaTerminal::formatarValor(const std::string& valor) {
    return corNumero() + valor + corReset();
}

std::string AparenciaTerminal::formatarOperacao(const std::string& operacao) {
    return corOperador() + operacao + corReset();
}
