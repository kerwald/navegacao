#include "p8g/p8g.hpp"
#include <iostream>
#include <vector>
#include <utility>
#include <queue>
#include <map>
#include <algorithm>
#include <random>

#define LARGURA 1280
#define ALTURA 720
#define TAMANHOCELULA 60

using namespace p8g;

enum class tipoCelula : uint8_t { 
    OBSTACULO, VAZIO
};


class Agente {
    private:
        std::pair<int, int> posicaoAtual;
        std::pair<int, int> posicaoFinal;
        std::vector< std::pair <int, int> > caminho;
        size_t indice;
        float progresso = 0.0f;
        float velocidade = 0.07f;
        float visualX;
        float visualY;

    public:
        Agente( const std::pair<int, int> posicaoAtual, const std::pair<int, int> posicaoFinal, std::vector<std::pair<int, int>> caminho ) : posicaoAtual( posicaoAtual ), posicaoFinal( posicaoFinal), caminho( caminho ), indice( 0 ), visualX( (float) posicaoAtual.first ),  visualY( (float) posicaoAtual.second ) {}

        std::pair<int, int> getPosicaoAtual() const{
            return posicaoAtual;
        }
        void setPosicaoAtual( std::pair<int, int> posicaoAtual ){
            this->posicaoAtual = posicaoAtual;
        }
        std::pair<int, int> getPosicaoFinal() const{
            return posicaoFinal;
        }
        std::vector< std::pair <int, int> > getCaminho(){
            return caminho;
        }

        std::pair< float, float> getVisual(){
            std::pair< float, float> visual = { visualX, visualY };
            return visual;
        }

        void atualizarPosicao() {
            
            if ( indice + 1 < caminho.size() ) {
                if ( progresso >= 1.0f ) {
                    indice++;
                    posicaoAtual = caminho[indice];
                    progresso = 0.0f; 
                }

                // LERP
                std::pair<int, int> proximo = caminho[ indice + 1 ];
                visualX = (float)posicaoAtual.first + ((float)proximo.first - (float)posicaoAtual.first) * progresso;
                visualY = (float)posicaoAtual.second + ((float)proximo.second - (float)posicaoAtual.second) * progresso;

                progresso += velocidade; 

            } else {
                visualX = (float)posicaoFinal.first;
                visualY = (float)posicaoFinal.second;
            }
        }

};

class Celula{

    private:
        tipoCelula tipo;

    public:
        Celula() : tipo( tipoCelula::VAZIO ){};
        Celula( tipoCelula tipo  ) : tipo( tipo ) {};

        void alterarTipo( tipoCelula tipo ){
            this->tipo = tipo;
        }
        tipoCelula getTipo( ){
            return tipo;
        }

};


class Navegacao{
    private:
        const int tamColuna;
        const int tamLinha;
        std::vector< std::vector<Celula> > grid;
        std::vector<Agente> agentes;
    public:
        Navegacao( ) : tamColuna( LARGURA/TAMANHOCELULA), tamLinha( ALTURA/TAMANHOCELULA) {
            grid.resize( tamLinha, std::vector<Celula>( tamColuna ) );
        };

        bool isValide( int x, int y ){

            if( y >= 0 && y <  tamLinha && x >= 0 && x < tamColuna ){
                return true;
            } else{
                return false;
            }

        }

        int getTamLinha(){
            return tamLinha;
        }

        int getTamColuna(){
            return tamColuna;
        }
        std::vector<Agente>& getAgentes(){
            return agentes;
        }

        bool temAgente( std::pair<int, int> posicao ){
            for( const auto& agente : agentes ){
                if( agente.getPosicaoAtual() == posicao || agente.getPosicaoFinal() == posicao ){
                    return true;
                }
            }
            return false;
        }
 

        bool adicionarAgente( const std::pair<int, int> origem, const std::pair<int, int> destino ){

            if( this->isValide( origem.first, origem.second ) && this->isValide( destino.first, destino.second ) &&  this->grid[ origem.second ][ origem.first ].getTipo() == tipoCelula::VAZIO && this->grid[ destino.second ][ destino.first ].getTipo() == tipoCelula::VAZIO) {
                Agente agente{ origem, destino, calcularCaminho( origem, destino ) };
                agentes.push_back( agente );
                return true;
            }
            return false;

        }

        void adicionarAgentes( int numeroDeAgentes ){

            static std::random_device rd; 
            static std::mt19937 gen(rd());

            std::pair<int, int> origem;
            std::pair<int, int> destino;
            std::uniform_int_distribution<> distrLinha( 0 , tamLinha - 1 ); 
            std::uniform_int_distribution<> distrColuna( 0 , tamColuna - 1 ); 
            for( int i=0; i<numeroDeAgentes; i++ ){
                bool ctrl = false;
                while( ctrl == false){
                    origem = { distrLinha(gen), distrColuna(gen) };
                    destino = { distrColuna(gen), distrColuna(gen) }; 
                    ctrl = adicionarAgente( origem, destino );
                }
            }
            
        }

        void atualizarAgentes(){

            for( auto& agente : agentes ){

                if( agente.getPosicaoAtual() != agente.getPosicaoFinal() ){
                    agente.atualizarPosicao();
                }

            }

        }

        void adicionarParede( const std::pair<int, int> posicao ){
            if ( !this->temAgente( posicao ) && grid[posicao.second][posicao.first].getTipo() == tipoCelula::VAZIO ){
                grid[ posicao.second ][ posicao.first ].alterarTipo( tipoCelula::OBSTACULO );
            }
        }
        Celula& getCelula( std::pair<int, int> posicao ){
            return grid[ posicao.second ][ posicao.first ];
        }

        std::vector<std::pair<int, int>> calcularCaminho( std::pair<int, int> inicio, std::pair<int, int> destino ) {

            std::queue<std::pair<int, int>> fila;
            fila.push(inicio);

            std::map<std::pair<int, int>, std::pair<int, int>> pai;
            pai[inicio] = {-1, -1}; // Marcador de início

            bool encontrou = false;
            int dx[] = {0, 0, 1, -1}; 
            int dy[] = {1, -1, 0, 0};

            while (!fila.empty()) {
                std::pair<int, int> atual = fila.front();
                fila.pop();

                if (atual == destino) {
                    encontrou = true;
                    break;
                }

                for (int i = 0; i < 4; i++) {
                    int nx = atual.first + dx[i];
                    int ny = atual.second + dy[i];
                    std::pair<int, int> vizinho = {nx, ny};

                    if (isValide(nx, ny) && 
                        grid[ny][nx].getTipo() == tipoCelula::VAZIO && 
                        pai.find(vizinho) == pai.end()) {
                        
                        pai[vizinho] = atual;
                        fila.push(vizinho);
                    }
                }
            }

            std::vector<std::pair<int, int>> caminho;
            if (encontrou) {
                std::pair<int, int> passo = destino;
                while (passo != std::pair<int, int>{-1, -1}) {
                    caminho.push_back(passo);
                    passo = pai[passo];
                }
                std::reverse(caminho.begin(), caminho.end());
            }
            return caminho;
        }

};

Navegacao *navegacaoptr;
int ctrl = 0;
int atualX = -1;
int atualY = -1;

void p8g::draw() {

    navegacaoptr->atualizarAgentes();

    background( 30, 30, 35 );
    for( int i=0; i<navegacaoptr->getTamLinha(); i++ ){
        for( int j=0; j<navegacaoptr->getTamColuna(); j++ ){
            if( navegacaoptr->getCelula( std::pair<int, int> {j, i}).getTipo() == tipoCelula::OBSTACULO ){
                noStroke();
                fill( 100, 100, 110 );
            } else{
                stroke( 50 ); 
                fill( 40, 40, 45 );
            }
            int posicaox = TAMANHOCELULA * j;
            int posicaoj = TAMANHOCELULA * i;
            rect( posicaox, posicaoj, TAMANHOCELULA, TAMANHOCELULA );
        }
    }
    for( auto& agente : navegacaoptr->getAgentes() ){

        std::pair<int, int> posicaoFinal = agente.getPosicaoFinal();
        float posicaoX = TAMANHOCELULA * posicaoFinal.first;
        float posicaoY = TAMANHOCELULA * posicaoFinal.second;

        fill( 255, 100, 100 );
        ellipse( posicaoX + 30, posicaoY + 30, 25, 25 ); 

        std::pair<float, float> posicaoAtual = agente.getVisual();
        posicaoX = TAMANHOCELULA * posicaoAtual.first;
        posicaoY = TAMANHOCELULA * posicaoAtual.second;

        fill( 80, 200, 120 );
        ellipse( posicaoX + 30, posicaoY + 30, 25, 25 ); 

    }

}

void p8g::keyPressed() {

    switch ( keyCode )
    {
    case 79:
        /* O */

        atualX = mouseX/TAMANHOCELULA;
        atualY = mouseY/TAMANHOCELULA;
        navegacaoptr->adicionarParede( std::pair<int, int> { atualX, atualY } ); 
        
        break;
    case 65:
        /* A */
        if( ctrl == 0 ){
            atualX = mouseX/TAMANHOCELULA;
            atualY = mouseY/TAMANHOCELULA;
            ctrl++;
            break;
        } else if( ctrl >= 1 ){
            std::pair<int, int> inicial( atualX, atualY );
            std::pair<int, int> final( mouseX/TAMANHOCELULA, mouseY/TAMANHOCELULA );
            navegacaoptr->adicionarAgente( inicial, final );
            
            ctrl = 0;
            break;
        }
    case 77:
        /* M */
        navegacaoptr->adicionarAgentes( 5 );
        break;

    default:
        break;
    }

}

void p8g::keyReleased() {}
void p8g::mouseMoved() {}
void p8g::mousePressed() {}
void p8g::mouseReleased() {}
void p8g::mouseWheel( float delta ) {}

int main() {
    Navegacao navegacao;
    navegacaoptr = &navegacao;
    run( LARGURA, ALTURA, "Navegacao" );
}