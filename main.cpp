#include "p8g/p8g.hpp"
#include <iostream>
#include <vector>
#include <utility>

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
    public:
        Agente( ) : posicaoAtual( -1, -1 ), posicaoFinal( -1, -1 ) {}
        Agente( const std::pair<int, int> posicaoAtual, const std::pair<int, int> posicaoFinal ) : posicaoAtual( posicaoAtual ), posicaoFinal( posicaoFinal) {}

        std::pair<int, int> getPosicaoAtual() const{
            return posicaoAtual;
        }
        void setPosicaoAtual( std::pair<int, int> posicaoAtual ){
            this->posicaoAtual = posicaoAtual;
        }
        std::pair<int, int> getPosicaoFinal() const{
            return posicaoFinal;
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
                Agente agente{ origem, destino };
                agentes.push_back( agente );
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


};

Navegacao *navegacaoptr;
int ctrl = 0;
int atualX = -1;
int atualY = -1;

void p8g::draw() {
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
        std::pair<int, int> posicaoAtual = agente.getPosicaoAtual();
        int posicaoX = TAMANHOCELULA * posicaoAtual.first;
        int posicaoY = TAMANHOCELULA * posicaoAtual.second;

        fill(255, 255, 0);
        ellipse( posicaoX + 30, posicaoY + 30, 25, 25 ); 

        std::pair<int, int> posicaoFinal = agente.getPosicaoFinal();
        posicaoX = TAMANHOCELULA * posicaoFinal.first;
        posicaoY = TAMANHOCELULA * posicaoFinal.second;

        fill(255, 10, 50);
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