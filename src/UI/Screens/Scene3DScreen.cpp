#include "UI/Screens/Scene3DScreen.h"
#include "Core/Utils/Appearance.h"

void ScreenScene3D::execute() {
    auto getBackgroundOpacity = [this](float opacity) {
        return this->composeBackgroundWithElements(opacity);
    };

    Appearance::cheerTransitionScene3D(
        getSoon(),
        getColorSoon(),
        getBackgroundInitial(),
        getBackgroundOpacity,
        getTargetY()
    );
}

void ScreenScene3D::superimposePanel(std::vector<std::string>& background, const std::vector<std::string>& art, int startY, int startX) const {
    for (size_t i = 0; i < art.size(); ++i) {
        int y = startY + i;
        if (y >= 0 && y < static_cast<int>(background.size())) {
            background[y] = Appearance::superimposePanelNaLineAnsi(background[y], art[i], startX);
        }
    }
}
