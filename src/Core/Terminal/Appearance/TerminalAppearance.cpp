#include "Core/Terminal/Appearance/TerminalAppearance.h"
#include "Core/Terminal/Appearance/Appearance.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>

// CORES DA PALETA C++

// Azul (palavra chave)
std::string TerminalAppearance::colorKeyword() { return "\033[38;2;86;156;214m"; }

// Turquesa (tipo)
std::string TerminalAppearance::colorType() { return "\033[38;2;78;201;176m"; }

// Laranja (string)
std::string TerminalAppearance::colorString() { return "\033[38;2;214;157;133m"; }

// Verde claro (numero)
std::string TerminalAppearance::colorNumber() { return "\033[38;2;181;206;168m"; }

// Verde escuro (comentario)
std::string TerminalAppearance::colorComment() { return "\033[38;2;96;139;78m"; }

// Amarelo claro (funcao)
std::string TerminalAppearance::colorFunction() { return "\033[38;2;220;220;170m"; }

// Azul claro (variavel)
std::string TerminalAppearance::colorVariable() { return "\033[38;2;156;220;254m"; }

// Cinza (operador)
std::string TerminalAppearance::colorOperator() { return "\033[38;2;180;180;180m"; }

std::string TerminalAppearance::colorReset() { return "\033[0m"; }

// FUNCOES DE BARRA DE VIDA COM CARACTERES ASCII
std::string TerminalAppearance::generateLifeBarASCII(double pct, int size) {
    if (pct < 0.0) pct = 0.0;
    if (pct > 1.0) pct = 1.0;
    
    std::string bar;
    bar = colorComment() + "{";
    
    int qtyFull = static_cast<int>(pct * size);
    int qtyEmpty = size - qtyFull;
    
    for (int i = 0; i < qtyFull; ++i) {
        bar += colorVariable() + "=";
    }
    for (int i = 0; i < qtyEmpty; ++i) {
        bar += colorComment() + "0";
    }
    
    bar += colorReset() + "}";
    bar += colorComment() + " // ";
    bar += colorNumber() + std::to_string(static_cast<int>(pct * 100)) + "%";
    bar += colorReset();
    
    return bar;
}

std::string TerminalAppearance::generateHealthBarIDE(double pct, int size) {
    if (pct < 0.0) pct = 0.0;
    if (pct > 1.0) pct = 1.0;
    
    std::string bar;
    bar = colorComment() + "{";
    
    int qtyFull = static_cast<int>(pct * size);
    int qtyEmpty = size - qtyFull;
    
    for (int i = 0; i < qtyFull; ++i) {
        bar += colorVariable() + "=";
    }
    for (int i = 0; i < qtyEmpty; ++i) {
        bar += colorComment() + "0";
    }
    
    bar += colorReset() + "}";
    bar += colorComment() + " // " + std::to_string(static_cast<int>(pct * 100)) + "%";
    bar += colorReset();
    
    return bar;
}

// ESTILIZACAO DE TEXTO EM ESTILO CODIGO
std::string TerminalAppearance::styleAsCode(const std::string& text, const std::string& type) {
    if (type == "string") {
        return colorComment() + "// " + colorString() + "\"" + text + "\"" + colorReset();
    } else if (type == "tipo") {
        return colorComment() + "typedef " + colorType() + text + colorReset();
    } else if (type == "numero") {
        return colorComment() + "const int " + colorNumber() + text + colorReset();
    } else {
        return colorComment() + "/* " + text + " */" + colorReset();
    }
}

std::string TerminalAppearance::styleAsType(const std::string& text) {
    return colorComment() + "class " + colorType() + text + colorReset();
}

std::string TerminalAppearance::styleAsFunction(const std::string& text) {
    return colorComment() + "void " + colorFunction() + "(" + text + ")" + colorReset();
}

std::string TerminalAppearance::styleAsComment(const std::string& text) {
    return colorComment() + "// " + text + colorReset();
}

std::string TerminalAppearance::styleAsNumber(const std::string& text) {
    return colorComment() + "int " + colorNumber() + text + colorReset();
}

// CONSTRUCAO DE CAIXAS EM ESTILO CODIGO
std::vector<std::string> TerminalAppearance::createBoxCode(const std::vector<std::string>& content, const std::string& title) {
    std::vector<std::string> box;
    
    std::string titleClean = title;
    std::replace(titleClean.begin(), titleClean.end(), ' ', '_');
    
    box.push_back(colorKeyword() + "struct " + colorType() + titleClean + " {");
    
    for (const auto& line : content) {
        // Tenta detectar se e uma linha chave:valor
        size_t post = line.find(":");
        if (post != std::string::npos) {
            std::string key = line.substr(0, post);
            std::string value = line.substr(post + 1);
            key.erase(std::remove_if(key.begin(), key.end(), [](char c) { return c == ' '; }), key.end());
            
            std::string lineFormatted = "    " + colorKeyword() + "auto " + colorVariable() + key + colorReset() + " = " + colorNumber() + value + colorReset() + ";";
            
            // Adiciona preenchimento para alinhar os campos
            int maxComp = 0;
            for (const auto& l : content) {
                size_t p = l.find(":");
                if (p != std::string::npos) {
                    std::string c = l.substr(0, p);
                    int compKey = Appearance::getVisualLength(c);
                    if (compKey > maxComp) maxComp = compKey;
                }
            }
            int padding = maxComp + 4 - Appearance::getVisualLength(key);
            if (padding > 0) {
                lineFormatted = "    " + colorKeyword() + "auto " + colorVariable() + key + std::string(padding, ' ') + colorReset() + " = " + colorNumber() + value + colorReset() + ";";
            }
            
            box.push_back(lineFormatted);
        } else {
            box.push_back("    " + line + ";");
        }
    }
    
    box.push_back("};");
    
    return box;
}

std::vector<std::string> TerminalAppearance::createBoxDrop(const std::vector<std::string>& drops) {
    std::vector<std::string> box;
    
    box.push_back(colorKeyword() + "class " + colorType() + "DropReward" + " {");
    box.push_back(colorKeyword() + "public:");
    
    for (const auto& drop : drops) {
        std::string cleanDrop = drop;
        // Remove codigos de escape ANSI
        cleanDrop = Appearance::removeANSIColors(cleanDrop);
        
        std::string line = "    " + colorKeyword() + "auto " + colorVariable() + "drop = " + colorString() + "\"" + cleanDrop + "\"" + colorReset() + ";";
        box.push_back(line);
    }
    
    box.push_back("};");
    
    return box;
}

// IMPRESSOES ESPECIFICAS DO TERMINAL
void TerminalAppearance::printTitleAsCode(const std::string& title) {
    std::string titleIDE = title;
    std::replace(titleIDE.begin(), titleIDE.end(), ' ', '_');
    
    std::string line = colorKeyword() + "class " + colorType() + titleIDE + " {";
    Appearance::printCentralized(line);
}

void TerminalAppearance::printStatsAsStruct(const std::vector<std::pair<std::string, std::string>>& fields) {
    std::vector<std::string> lines;
    
    lines.push_back(colorKeyword() + "struct " + colorType() + "StatsCombate" + " {");
    
    for (const auto& [name, value] : fields) {
        std::string line = "    " + colorKeyword() + "int " + colorVariable() + name + " = " + colorNumber() + value + ";";
        lines.push_back(line);
    }
    
    lines.push_back("};");
    
    Appearance::printBlockCentralized(lines);
}

void TerminalAppearance::printCombatLogs(const std::vector<std::string>& messages) {
    std::vector<std::string> log;
    
    log.push_back(colorKeyword() + "namespace " + colorType() + "CombateLog" + " {");
    
    for (const auto& msg : messages) {
        std::string cleanMsg = Appearance::removeANSIColors(msg);
        log.push_back("    " + colorKeyword() + "auto " + colorVariable() + "msg = " + colorString() + "\"" + cleanMsg + "\"" + colorReset() + ";");
    }
    
    log.push_back("}");
    
    Appearance::printBlockCentralized(log);
}

void TerminalAppearance::printBoxDrops(const std::vector<std::string>& drops) {
    std::vector<std::string> box = createBoxDrop(drops);
    Appearance::printBlockCentralized(box);
}

// UTILIDADES DE FORMATACAO
std::string TerminalAppearance::formatVarName(const std::string& name) {
    return colorVariable() + name + colorReset();
}

std::string TerminalAppearance::formatType(const std::string& type) {
    return colorType() + type + colorReset();
}

std::string TerminalAppearance::formatValue(const std::string& value) {
    return colorNumber() + value + colorReset();
}

std::string TerminalAppearance::formatOperation(const std::string& operation) {
    return colorOperator() + operation + colorReset();
}
