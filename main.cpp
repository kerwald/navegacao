#include "p8g/p8g.hpp"
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cmath>
#include <cstdlib> 
#include <ctime>   


namespace Config {
    constexpr int LARGURA_TELA = 1280;
    constexpr int ALTURA_TELA = 720;
    constexpr int TAMANHO_CELULA = 40;
    constexpr int COLUNAS = LARGURA_TELA / TAMANHO_CELULA;
    constexpr int LINHAS = ALTURA_TELA / TAMANHO_CELULA;
    constexpr float ATRASO_MOVIMENTO = 0.1f; // Velocidade do agente
}

// --- ESTRUTURAS AUXILIARES ---
struct Ponto {
    int x, y;
    bool operator==(const Ponto& outro) const { return x == outro.x && y == outro.y; }
    bool operator!=(const Ponto& outro) const { return !(*this == outro); }
    bool ehValido() const { return x >= 0 && x < Config::COLUNAS && y >= 0 && y < Config::LINHAS; }
};

struct Cor { float r, g, b; };

enum class TipoCelula { VAZIO, PAREDE };
enum class ModoFerramenta { PAREDE, NOVO_AGENTE };

// ALGORITMO DE NAVEGAÇÃO (BFS) 
class BuscadorCaminho {

    public:
        static std::vector<Ponto> encontrarCaminho( const Ponto& inicio, const Ponto& fim, const TipoCelula grade[Config::COLUNAS][Config::LINHAS]) {

            if ( !inicio.ehValido() || !fim.ehValido() ) return {};

            if ( grade[ inicio.x ][ inicio.y ] == TipoCelula::PAREDE || grade[ fim.x ][ fim.y ] == TipoCelula::PAREDE ) return {};

            std::queue<Ponto> fronteira;
            fronteira.push( inicio );

            std::vector<std::vector<Ponto>> veioDe( Config::COLUNAS, std::vector<Ponto>( Config::LINHAS, {-1, -1}) );
            std::vector<std::vector<bool>> visitado( Config::COLUNAS, std::vector<bool>( Config::LINHAS, false ) );

            visitado[ inicio.x ][ inicio.y ] = true;
            const int dx[] = {0, 0, -1, 1};
            const int dy[] = {-1, 1, 0, 0};
            bool encontrou = false;

            while ( !fronteira.empty() ) {
                Ponto atual = fronteira.front();
                fronteira.pop();

                if (atual == fim) { encontrou = true; break; }

                for (int i = 0; i < 4; i++) {
                    Ponto proximo = { atual.x + dx[i], atual.y + dy[i] };
                    if ( proximo.ehValido() && !visitado[proximo.x][proximo.y] && grade[ proximo.x][proximo.y] != TipoCelula::PAREDE) {
                        visitado[proximo.x][proximo.y] = true;
                        veioDe[proximo.x][proximo.y] = atual;
                        fronteira.push(proximo);
                    }
                }
            }

            std::vector<Ponto> caminho;
            if (encontrou) {
                Ponto p = fim;
                while (p != inicio) {
                    caminho.push_back(p);
                    p = veioDe[p.x][p.y];
                }
                std::reverse(caminho.begin(), caminho.end());
            }
            return caminho;
        }
};


class Agente {

    private:
        Ponto posicaoAtual;
        Ponto destino;
        std::vector<Ponto> caminho;
        size_t indiceCaminho;
        float timer;
        Cor cor;

    public:
        Agente( Ponto inicio, Ponto fim ) 
            : posicaoAtual( inicio ), destino( fim ), indiceCaminho( 0 ), timer( 0.0f ) 
        {
            cor = { (float)(rand()%200+55), (float)(rand()%200+55), (float)(rand()%200+55) };
        }

        void setCaminho( const std::vector<Ponto>& c ) { caminho = c; indiceCaminho = 0; timer = 0; }
        Ponto getPos() const { return posicaoAtual; }
        Ponto getDest() const { return destino; }

        // Faz o agente transitar
        void atualizar(float dt) {
            if (caminho.empty() || indiceCaminho >= caminho.size()) return;
            timer += dt;
            if (timer >= Config::ATRASO_MOVIMENTO) {
                posicaoAtual = caminho[indiceCaminho++];
                timer = 0;
            }
        }

        void desenhar() const {
            using namespace p8g;
            // Desenha destino
            stroke( cor.r, cor.g, cor.b ); 
            noFill(); 
            strokeWeight( 3 );
            rect( destino.x * Config::TAMANHO_CELULA + 5, destino.y * Config::TAMANHO_CELULA + 5, Config::TAMANHO_CELULA - 10, Config::TAMANHO_CELULA - 10 );
            strokeWeight(1);

            // Desenha agente
            fill( cor.r, cor.g, cor.b ); 
            float cx = posicaoAtual.x * Config::TAMANHO_CELULA + Config::TAMANHO_CELULA/2.0f;
            float cy = posicaoAtual.y * Config::TAMANHO_CELULA + Config::TAMANHO_CELULA/2.0f;
            ellipse(cx, cy, Config::TAMANHO_CELULA * 0.7f, Config::TAMANHO_CELULA * 0.7f);

        }
};


class SistemaGrid {
    
    private:
        TipoCelula grade[ Config::COLUNAS ][ Config::LINHAS ];
        std::vector<Agente> agentes;    
        ModoFerramenta ferramenta;
        
        // Variável para armazenar o primeiro clique
        Ponto tempInicio; 

    public:
        SistemaGrid() : ferramenta( ModoFerramenta::NOVO_AGENTE ), tempInicio{ -1, -1 } {
            srand( time( 0 ) );
            limparTudo();
        }

        void limparTudo() {
            for(int x=0; x<Config::COLUNAS; x++) 
                for(int y=0; y<Config::LINHAS; y++) 
                    grade[x][y] = TipoCelula::VAZIO;
            agentes.clear();
            tempInicio = { -1, -1 };
        }

        void gerarAgentesAleatorios( int quantidade ) {
            int criados = 0;
            int tentativas = 0;
            while( criados < quantidade && tentativas < 1000 ) {
                Ponto inicio = { rand() % Config::COLUNAS, rand() % Config::LINHAS };
                Ponto fim = { rand() % Config::COLUNAS, rand() % Config::LINHAS };
                
                if( grade[ inicio.x ][ inicio.y ] != TipoCelula::PAREDE && 
                    grade[fim.x][fim.y] != TipoCelula::PAREDE && inicio != fim ) {
                    
                    Agente novo( inicio, fim );
                    novo.setCaminho( BuscadorCaminho::encontrarCaminho( inicio, fim, grade ) );
                    agentes.push_back( novo );
                    criados++;

                }
                tentativas++;
            }
            std::cout << criados << " agentes gerados." << std::endl;
        }

        void atualizar( float dt ) {
            for( auto& a : agentes ) a.atualizar( dt );
        }

        void desenhar() {

            using namespace p8g;
            background(0.9f);

            // Desenha Grade e Paredes
            for( int x=0; x<Config::COLUNAS; x++) {
                for( int y=0; y<Config::LINHAS; y++ ) {

                    ( grade[x][y] == TipoCelula::PAREDE ) ? fill(50, 50, 60) :
                    fill(245, 245, 245);
                    stroke(200/255.0f);
                    rect( x*Config::TAMANHO_CELULA, y*Config::TAMANHO_CELULA, Config::TAMANHO_CELULA, Config::TAMANHO_CELULA);
                }
            }

            // Desenha destaque se estiver selecionando inicio
            if ( tempInicio.ehValido() ) {
                fill(0, 255, 255, 100); // Azul claro transparente
                rect( tempInicio.x*Config::TAMANHO_CELULA, tempInicio.y*Config::TAMANHO_CELULA, Config::TAMANHO_CELULA, Config::TAMANHO_CELULA );
            }

            // Desenha Agentes
            for( const auto& a : agentes ) a.desenhar();
        }

        // --- INTERAÇÃO COM MOUSE ---
        void click( int mx, int my ) {
            Ponto cel = { mx / Config::TAMANHO_CELULA, my / Config::TAMANHO_CELULA };
            if ( !cel.ehValido() ) return;

            if ( ferramenta == ModoFerramenta::PAREDE ) {
                // Define pontos ocupados e livres
                grade[ cel.x ][ cel.y ] = ( grade[ cel.x ][ cel.y ] == TipoCelula::VAZIO ) ? TipoCelula::PAREDE : TipoCelula::VAZIO;
                recalcularRotas(); // Se colocar parede em cima de rota, recalcula
            } else if ( ferramenta == ModoFerramenta::NOVO_AGENTE ) {
                // Lógica de 2 cliques: Origem -> Destino
                if ( !tempInicio.ehValido() ) {
                    // Clique 1: Origem
                    if ( grade[ cel.x ][ cel.y ] != TipoCelula::PAREDE ) {
                        tempInicio = cel;
                        std::cout << "Origem definida. Clique no destino." << std::endl;
                    }

                } else {
                    // Clique 2: Destino
                    if ( grade[cel.x][cel.y] != TipoCelula::PAREDE ) {
                        Agente novo( tempInicio, cel );
                        novo.setCaminho( BuscadorCaminho::encontrarCaminho( tempInicio, cel, grade ) );
                        agentes.push_back(novo);
                        tempInicio = {-1, -1}; // Reseta para o próximo
                        std::cout << "Agente criado!" << std::endl;
                    }
                }
            }
        }

        void tecla(int k) {
            if ( k == 'P' || k == 'p' ) { 
                ferramenta = ModoFerramenta::PAREDE; 
                std::cout << "Modo: PAREDE" << std::endl; 
            }
            if ( k == 'A' || k == 'a') { 
                ferramenta = ModoFerramenta::NOVO_AGENTE; 
                tempInicio={-1,-1}; 
                std::cout << "Modo: AGENTE (Clique Origem -> Clique Destino)" << std::endl; 
            }
            if ( k == 'R' || k == 'r' ) gerarAgentesAleatorios(5);
            if ( k == 'C' || k == 'c' ) limparTudo();
        }

        void recalcularRotas() {
            for(auto& a : agentes) 
                a.setCaminho( BuscadorCaminho::encontrarCaminho( a.getPos(), a.getDest(), grade ) );
        }
};

SistemaGrid* app = nullptr;

void p8g::draw() { 
    if( app ){
        app->atualizar(1.0f/60.0f); 
        app->desenhar(); 
    } 
}
void p8g::mousePressed() { 
    if( app ) 
        app->click( p8g::mouseX, p8g::mouseY ); 
}
void p8g::keyPressed() { if( app ) app->tecla( p8g::keyCode ); }
void p8g::keyReleased(){}
void p8g::mouseMoved(){}
void p8g::mouseReleased(){}
void p8g::mouseWheel( float ){}

int main() {
    app = new SistemaGrid();
    {
        using namespace p8g;
        p8g::run( Config::LARGURA_TELA, Config::ALTURA_TELA, "Navegacao", true );
    }
    delete app;
    return 0;
}