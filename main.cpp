#include "p8g/p8g.hpp"
#include <iostream>
#include <vector>

const int SCREEN_W = 1280;
const int SCREEN_H = 720;
const int CELL_SIZE = 40;

const int COLS = SCREEN_W / CELL_SIZE; // Será 32
const int ROWS = SCREEN_H / CELL_SIZE; // Será 18 -- 576 CÉLULAS

struct Ponto { int x, y; }; 

enum Celula {
    OCUPADO, 
    LIVRE,
    INICIAL,  
    FINAL     
};

struct Agente{
        int col;
        int row;
};
struct Final{
        int col;
        int row;
};

Agente agente{ -1, -1 };
Final final{ -1, -1 };
Celula map[COLS][ROWS];




void p8g::draw() {}
void p8g::keyPressed() {}
void p8g::keyReleased() {}
void p8g::mouseMoved() {}
void p8g::mousePressed() {

    const int tmpCol = mouseX / CELL_SIZE;
    const int tmpRow = mouseY / CELL_SIZE;

    switch ( keyCode )
    {
    case 79: // 79 == "O" ocupado
        if( map[tmpCol][tmpRow] == LIVRE  ){
            map[tmpCol][tmpRow] = OCUPADO;
        }
        break;
    case 73: // 73 == "I" inicial
        if( agente.col == -1 ){
            map[tmpCol][tmpRow] = INICIAL;
            agente.col = tmpCol;
            agente.row = tmpRow;
        } else{
            map[agente.col][agente.row] = LIVRE;
            map[tmpCol][tmpRow] = INICIAL;
            agente.col = tmpCol;
            agente.row = tmpRow;
        }
        break;
    case 70:  // 70 == "F" final
        if( final.col == -1 ){
            map[tmpCol][tmpRow] = FINAL;
            final.col = tmpCol;
            final.row = tmpRow;
        } else{
            map[final.col][final.row] = LIVRE;
            map[tmpCol][tmpRow] = FINAL;
            final.col = tmpCol;
            final.row = tmpRow;
        }
        
        break;  
    default:
        break;
    }
}
void p8g::mouseReleased() {}
void p8g::mouseWheel( float delta ) {}

int main() {

    {
        using namespace p8g;
        run( SCREEN_W, SCREEN_H, "Navegacao", true );
    }

    for( int i=0; i<COLS; i++ ){
        for( int j=0; j<ROWS; j++ ){
            map[i][j] = LIVRE;
        }
    }

    return 0;

}

