#include "Perspectiva/IVisaoRenderer.h"
class RendererProvider { public: static IVisaoRenderer* get() { return instancia; } static void set(IVisaoRenderer* r) { instancia = r; } private: static IVisaoRenderer* instancia; };
