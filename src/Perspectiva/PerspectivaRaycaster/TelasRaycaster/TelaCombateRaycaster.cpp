#include "TelaCombateRaycaster.h"

void TelaCombateRaycaster::exibirLogoParaTelaDeCombate(const std::string&, bool) {}
void TelaCombateRaycaster::animarIntroducaoCombate(const std::string&, const std::vector<Personagem*>&, Personagem*) {}
std::vector<std::string> TelaCombateRaycaster::obterLinhasBarraDeStatusDoJogador(Personagem*, Cor, int, int, bool) { return {}; }
void TelaCombateRaycaster::exibirHordaDeInimigosLadoALado(const std::vector<Personagem*>&, Personagem*, int, bool, bool, bool, Item*, int, const std::vector<std::string>&) {}
void TelaCombateRaycaster::animarDanoNoInimigo(const std::string&, const std::vector<Personagem*>&, Personagem*, Personagem*, Personagem*, const std::vector<Personagem*>&, int) {}
void TelaCombateRaycaster::animarCuraNoInimigo(const std::string&, const std::vector<Personagem*>&, Personagem*, Personagem*, const std::vector<Personagem*>&, int) {}
void TelaCombateRaycaster::animarDanoNoJogador(const std::string&, const std::vector<Personagem*>&, Personagem*, Personagem*, const std::vector<Personagem*>&, bool, int) {}
void TelaCombateRaycaster::animarCuraNoJogador(const std::string&, const std::vector<Personagem*>&, Personagem*, Personagem*, const std::vector<Personagem*>&, int) {}
void TelaCombateRaycaster::animarMorteInimigo(const std::string&, const std::vector<Personagem*>&, Personagem*, Personagem*, const std::vector<Personagem*>&, const std::vector<std::string>&) {}
void TelaCombateRaycaster::atualizarTelaEstatica(const std::string&, const std::vector<Personagem*>&, Personagem*, const std::vector<Personagem*>&, bool) {}
void TelaCombateRaycaster::adicionarMensagemFixa(const std::string&) {}
void TelaCombateRaycaster::limparMensagensFixas() {}
void TelaCombateRaycaster::configurarContexto3D(bool, const std::vector<std::string>&, float, float, float, const std::string&) {}
void TelaCombateRaycaster::definirTurnoVisivel(int, const std::string&) {}
void TelaCombateRaycaster::selecionarHUDDeAliado(Personagem*, const std::vector<Personagem*>&) {}
int TelaCombateRaycaster::obterAcaoDoJogador(int, Personagem*, const std::vector<Personagem*>&, Personagem*, const std::vector<Personagem*>&) { return 0; }
int TelaCombateRaycaster::obterAlvoAtaque(const std::string&, const std::vector<Personagem*>&, Personagem*, const std::vector<Personagem*>&) { return 0; }
int TelaCombateRaycaster::obterAlvoItem(const std::string&, const std::vector<Personagem*>&, Personagem*, const std::vector<Personagem*>&) { return 0; }
int TelaCombateRaycaster::obterEscolhaDeEscudo(const std::string&, const std::vector<Item*>&) { return 0; }
void TelaCombateRaycaster::notificarInimigosMaisAgeis() {}
void TelaCombateRaycaster::notificarTurnoExtra(int, int) {}
void TelaCombateRaycaster::notificarDesprevencaoInventario() {}
void TelaCombateRaycaster::notificarSemEscudos(const std::string&) {}
void TelaCombateRaycaster::notificarDesequilibrioDefesa(const std::string&) {}
void TelaCombateRaycaster::notificarPosturaDefensiva(const std::string&, const std::string&) {}
void TelaCombateRaycaster::notificarAcaoInvalida() {}
void TelaCombateRaycaster::notificarCancelamentoItem() {}
void TelaCombateRaycaster::notificarRequisitoNaoAtendido(const std::string&) {}
