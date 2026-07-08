#include "Perspectiva/RenderizadorPerspectiva.h"
class RendererProvider { public: static RenderizadorPerspectiva* get() { return instancia; } static void set(RenderizadorPerspectiva* r) { instancia = r; } private: static RenderizadorPerspectiva* instancia; };
