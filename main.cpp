#include "p8g/p8g.hpp"
#include <iostream>
#include <vector>
#include <utility>
#include <queue>
#include <map>
#include <algorithm>

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
        int indice;

    public:
        Agente( const std::pair<int, int> posicaoAtual, const std::pair<int, int> posicaoFinal, std::vector<std::pair<int, int>> caminho ) : posicaoAtual( posicaoAtual ), posicaoFinal( posicaoFinal), caminho( caminho ), indice( 0 ) {}

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
        void avancaPosicao(){
            indice++;
            posicaoAtual = caminho[ indice ];
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
        std::vector<Agente> getAgentes(){
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
 

        void adicionarAgente( const std::pair<int, int> origem, const std::pair<int, int> destino ){

            if( this->isValide( origem.first, origem.second ) && this->isValide( destino.first, destino.second ) &&  this->grid[ origem.second ][ origem.first ].getTipo() == tipoCelula::VAZIO && this->grid[ destino.second ][ destino.first ].getTipo() == tipoCelula::VAZIO) {
                Agente agente{ origem, destino, calcularCaminho( origem, destino ) };
                agentes.push_back( agente );
            }

        }

        void atualizarAgentes(){

            for( auto& agente : agentes ){

                if( agente.getPosicaoAtual() != agente.getPosicaoFinal() ){
                    agente.avancaPosicao();
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
int frameDelay = 20;
int frameCount = 0;

void p8g::draw() {

    frameCount++;

    if ( frameCount % frameDelay == 0) {
        navegacaoptr->atualizarAgentes();
    }

    background(220);
    for( int i=0; i<navegacaoptr->getTamLinha(); i++ ){
        for( int j=0; j<navegacaoptr->getTamColuna(); j++ ){
            if( navegacaoptr->getCelula( std::pair<int, int> {j, i}).getTipo() == tipoCelula::OBSTACULO ){
                fill(255, 0, 255);
            } else{
                fill(255, 0, 0);
            }
            int posicaox = TAMANHOCELULA * j;
            int posicaoj = TAMANHOCELULA * i;
            rect( posicaox, posicaoj, TAMANHOCELULA, TAMANHOCELULA );
        }
    }
    for( const auto& agente : navegacaoptr->getAgentes() ){

        std::pair<int, int> posicaoFinal = agente.getPosicaoFinal();
        int posicaoX = TAMANHOCELULA * posicaoFinal.first;
        int posicaoY = TAMANHOCELULA * posicaoFinal.second;

        fill(255, 10, 50);
        ellipse( posicaoX + 30, posicaoY + 30, 25, 25 ); 

        std::pair<int, int> posicaoAtual = agente.getPosicaoAtual();
        posicaoX = TAMANHOCELULA * posicaoAtual.first;
        posicaoY = TAMANHOCELULA * posicaoAtual.second;

        fill(255, 255, 0);
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