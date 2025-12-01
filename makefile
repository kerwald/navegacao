# Variáveis de Configuração
CXX = g++

# CXXFLAGS: Adicionamos -Ip8g para cabeçalhos e -MMD -MP para dependências automáticas.
# -MMD: Gera o arquivo de dependência (.d) junto com o objeto.
# -MP: Adiciona targets "phony" para cabeçalhos, prevenindo erros ao deletar.
CXXFLAGS = -std=c++11 -Wall -Ip8g -MMD -MP

# LDFLAGS: Adicionamos -Lp8g para que o linker procure bibliotecas na pasta 'p8g'.
LDFLAGS = -Lp8g

LIBS = -lp8g++
TARGET = main.exe

# Encontra automaticamente todos os arquivos .cpp na pasta.
SRCS = $(wildcard *.cpp)

# A lista de objetos é gerada a partir da lista de fontes
OBJS = $(SRCS:.cpp=.o)

# Regra padrão: compila o alvo principal
.PHONY: all
all: $(TARGET)

# Regra para linkar e criar o executável final
$(TARGET): $(OBJS)
	@echo "Ligando objetos para criar $(TARGET)..."
	$(CXX) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LIBS)
	@echo "Projeto compilado com sucesso! Executavel criado: $(TARGET)"

# Regra para compilar os arquivos fonte (.cpp) em arquivos objeto (.o)
%.o: %.cpp
	@echo "Compilando $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regra para executar o programa
.PHONY: run
run: all
	./$(TARGET)

# Regra para limpar os arquivos gerados (versão para Windows)
.PHONY: clean
clean:
	@echo "Limpando arquivos gerados..."
	del /f /q $(TARGET) $(OBJS) $(OBJS:.o=.d) 2>nul || (exit 0)
	@echo "Limpeza concluida."

# Inclui todos os arquivos de dependência (.d) gerados pelo -MMD.
# Isso garante que se um arquivo .h mudar, o .cpp correspondente será recompilado.
-include $(OBJS:.o=.d)