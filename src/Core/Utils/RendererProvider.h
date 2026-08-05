#include "UI/PerspectiveRenderer.h"
class RendererProvider { public: static PerspectiveRenderer* get() { return instance; } static void set(PerspectiveRenderer* r) { instance = r; } private: static PerspectiveRenderer* instance; };
