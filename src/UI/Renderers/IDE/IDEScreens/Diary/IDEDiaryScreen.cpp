#include "UI/Renderers/IDE/IDEScreens/Diary/IDEDiaryScreen.h"
#include "UI/Renderers/IDE/IDETheme.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include <iostream>
#include <vector>

void IDEDiaryScreen::renderBackground() {
    Appearance::clearScreen();
}

void IDEDiaryScreen::displayHeader(int /*yInicial*/) {
    int width = Appearance::getTerminalWidth();
    std::vector<std::string> tabs = {
        "GameJournal.log",
        "QuestTrace.md"
    };
    std::cout << IDETheme::renderTabBar(tabs, 0, width) << "\n";
    std::cout << IDETheme::comment("// logs/GameJournal.log > Stream de logs em runtime do journal") << "\n\n";
}

void IDEDiaryScreen::renderBox(const std::vector<std::string>& lines, const std::string& title, Color /*corCaixa*/, int /*minY*/, int /*sobreposicaoYInicial*/) {
    int width = Appearance::getTerminalWidth();
    std::vector<std::string> boxLines;
    boxLines.push_back(IDETheme::comment("// --- " + title + " ---"));
    for (size_t i = 0; i < lines.size(); ++i) {
        boxLines.push_back(IDETheme::number(static_cast<int>(i + 1)) + " | " + lines[i]);
    }

    auto centered = IDETheme::centerBlock(boxLines, width);
    for (const auto& l : centered) std::cout << l << "\n";
    std::cout << "\n";
}

void IDEDiaryScreen::renderPopupMessage(const std::string& title, const std::vector<std::string>& text) {
    Appearance::clearScreen();
    int width = Appearance::getTerminalWidth();
    int height = Appearance::getTerminalHeight();

    std::vector<std::string> lines;
    lines.push_back(IDETheme::comment("// Evento de log em runtime: " + title));
    lines.push_back(IDETheme::keyword("struct ") + IDETheme::type("LogRecord") + IDETheme::punctuation(" {"));
    for (const auto& l : text) {
        lines.push_back("    " + IDETheme::type("const char*") + " " + IDETheme::variable("entry") + " = " + IDETheme::stringLiteral(l) + ";");
    }
    lines.push_back(IDETheme::punctuation("};"));

    std::vector<std::string> tabs = { "GameJournal.log", "QuestTrace.md" };
    auto editorView = IDETheme::renderEditorView(tabs, 0, "// logs/GameJournal.log", lines, width, height, "[ENTER] Retornar");

    for (const auto& l : editorView) std::cout << l << "\n";
    std::cout << "\033[J" << std::flush;
    InputControl::waitForEnter();
}

void IDEDiaryScreen::renderPopupInspectionWithArt(const std::string& title, const std::vector<std::string>& /*arte*/, const std::vector<std::string>& info, const std::string& subtitle) {
    Appearance::clearScreen();
    int width = Appearance::getTerminalWidth();
    int height = Appearance::getTerminalHeight();

    std::vector<std::string> lines;
    lines.push_back(IDETheme::comment("// Descriptor de quest: " + title));
    if (!subtitle.empty()) {
        lines.push_back(IDETheme::comment("// Subtítulo da quest: " + subtitle));
    }
    lines.push_back(IDETheme::keyword("class ") + IDETheme::type("QuestDescriptor") + IDETheme::punctuation(" {"));
    lines.push_back(IDETheme::keyword("public:"));
    for (const auto& l : info) {
        lines.push_back("    " + IDETheme::comment("// " + l));
    }
    lines.push_back(IDETheme::punctuation("};"));

    std::vector<std::string> tabs = { "QuestTrace.md", "GameJournal.log" };
    auto editorView = IDETheme::renderEditorView(tabs, 0, "// quests/QuestTrace.md", lines, width, height, "[ENTER] Retornar");

    for (const auto& l : editorView) std::cout << l << "\n";
    std::cout << "\033[J" << std::flush;
    InputControl::waitForEnter();
}
