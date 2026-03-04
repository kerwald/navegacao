#include "p8g/p8g.hpp"
#include <iostream>
#include <vector>
#include <utility>

#define LARGURA 1280
#define ALTURA 720
#define TAMANHOCELULA 60

using namespace p8g;

class Agente{
    private:
        std::pair<int, int> posicaoInicial;
        std::pair<int, int> posicaoFinal;
    public:
        Agente( const int x, const int y ) : posicaoInicial( x, y ), posicaoFinal( -1, -1 ) 
        {}
        Agente() : posicaoInicial( -1, -1 ), posicaoFinal( -1, -1 ) 
        {}
        std::pair<int, int> getInicio(){
            return posicaoInicial;
        }
        void setPosicaoInicial( std::pair<int, int> posicaoInicial ){
            this->posicaoInicial = posicaoInicial;
        }
        std::pair<int, int> getPosicaoFinal(){
            return posicaoFinal;
        }
        void setPosicaoFinal( std::pair<int, int> posicaoFinal ){
            this->posicaoFinal = posicaoFinal;
        }

};

struct Celula{
    Agente agente;
    bool isFree;
    Celula( Agente agente  ) : agente( agente ), isFree( false ) {};
    Celula() : isFree( true ) {};
    Agente getAgente(){
        return agente;
    }
    bool getIsFree(){
        return isFree;
    }
};

class Navegacao{
    private:
        const int coluna;
        const int linha;
        std::vector< std::vector<Celula> > grid;
    public:
        Navegacao( ) : coluna( LARGURA/TAMANHOCELULA), linha( ALTURA/TAMANHOCELULA) {
            grid.resize( linha, std::vector<Celula>( coluna ) );
        };

        bool isValide( int x, int y ){

            if( x >= 0 && x <  linha && y >= 0 && y < coluna ){
                return true;
            } else{
                return false;
            }

        }
        int getLinha(){
            return linha;
        }
        int getColuna(){
            return coluna;
        }
        Celula getCelula( const int x, const int y ){
            Celula celula = grid[x][y];
            return celula;
        }
        bool adicionarAgente( const int posicaoX, const int posicaoY ){
            if( this->isValide( posicaoX, posicaoY ) &&  this->getCelula( posicaoX, posicaoY ).getIsFree() ) {
                Agente agente{ posicaoX, posicaoY };
                Celula celula{ agente };
                grid[posicaoX][posicaoY] = celula;
            }
        }
        bool adicionarPosicaoFinal( const std::pair<int, int> origem, const std::pair<int, int> destino ){
       
            grid[ origem.first ][ origem.second ].agente.setPosicaoFinal( destino );

        }


};

Navegacao *navegacaoptr;
int ctrl = 0;
int atualX = -1;
int atualY = -1;

void p8g::draw() {
    background(220);
    bool color = false;
    for( int i=0; i<navegacaoptr->getLinha(); i++ ){
        for( int j=0; j<navegacaoptr->getColuna(); j++ ){
            if( color == false ){
                fill(255, 0, 255);
                color = true;
            } else{
                fill(255, 0, 0);
                color = false;
            }
            int posicaox = TAMANHOCELULA * j;
            int posicaoj = TAMANHOCELULA * i;
            rect( posicaox, posicaoj, TAMANHOCELULA, TAMANHOCELULA );
            if( navegacaoptr->getCelula( i, j ).getIsFree() == false  ){
                fill(255, 255, 0);
                ellipse( posicaox + 30, posicaoj + 30, 25, 25 );
            }
        }
    }
}

void p8g::keyPressed() {}
void p8g::keyReleased() {}
void p8g::mouseMoved() {}
void p8g::mousePressed() {

    if( ctrl == 0 ){
        atualX = mouseX/TAMANHOCELULA;
        atualY = mouseY/TAMANHOCELULA;
        navegacaoptr->adicionarAgente( atualX, atualY );
        ctrl++;
    } else if( ctrl == 1 ){
        std::pair<int, int> inicial( atualX, atualY );
        std::pair<int, int> final( mouseX/TAMANHOCELULA, mouseY/TAMANHOCELULA );
        navegacaoptr->adicionarPosicaoFinal( inicial, final );
        ctrl = 0;
    }

}
void p8g::mouseReleased() {}
void p8g::mouseWheel( float delta ) {}

int main() {
    Navegacao navegacao;
    navegacaoptr = &navegacao;
    run( LARGURA, ALTURA, "Navegacao" );
}