#pragma once

#include <vector>
#include <string>

class Scene3DScreen {
public:
    virtual ~Scene3DScreen() = default;

    void execute();

protected:
    virtual std::vector<std::string> getLogo() const = 0;
    virtual std::string getLogoColor() const = 0;

    virtual std::vector<std::string> getBackgroundInitial() const = 0;

    virtual std::vector<std::string> composeBackgroundWithElements(float opacity) const = 0;

    virtual int getTargetY() const { return 2; }

    void superimposePanel(std::vector<std::string>& background, const std::vector<std::string>& art, int startY, int startX) const;
};

// Apelido para compatibilidade retroativa
using ScreenScene3D = Scene3DScreen;
