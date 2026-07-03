#include "TelaPauseRaycaster.h"

int TelaPauseRaycaster::renderizarMenuPause() { return 0; }
int TelaPauseRaycaster::renderizarMenuConfiguracoes(Personagem*) { return 6; }
int TelaPauseRaycaster::renderizarMenuAparencia(Personagem*) { return 2; }
int TelaPauseRaycaster::renderizarMenuFundo(int) { return 6; }
int TelaPauseRaycaster::renderizarMenuSensibilidade(int, int) { return 2; }
bool TelaPauseRaycaster::renderizarConfirmacaoSalvar(Personagem*) { return false; }
