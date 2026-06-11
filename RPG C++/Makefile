# Nome do executavel gerado
TARGET = JogoRPG.exe

# Compilador e flags de compilacao (-O2 adiciona otimizacoes de performance)
CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wextra -O2 -MMD -MP
LDFLAGS = -static -static-libgcc -static-libstdc++

# Funcao para buscar recursivamente todos os arquivos .cpp nas pastas
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# Lista de todos os arquivos fonte (.cpp) e objetos (.o) correspondentes
SRCS := $(call rwildcard,,*.cpp)

# Pasta onde os arquivos .o serao guardados (mantem o projeto limpo)
OBJ_DIR = obj
OBJS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

# Regra padrao a ser executada ao digitar o comando make
all: $(TARGET)

# Regra de linkagem: junta todos os .o para criar o .exe
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Regra de compilacao: compila cada .cpp em um .o dentro da pasta obj/
$(OBJ_DIR)/%.o: %.cpp
	@if not exist "$(subst /,\,$(@D))" mkdir "$(subst /,\,$(@D))"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regra de limpeza (comandos adaptados para Windows, o "-" ignora erros caso nao existam arquivos)
clean:
	-rmdir /S /Q $(OBJ_DIR) 2>nul
	-del /S /Q *.o 2>nul
	-del /Q $(TARGET) 2>nul

-include $(DEPS)
