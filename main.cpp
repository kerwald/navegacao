#include "p8g/p8g.hpp"
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

const int SCREEN_W = 1280;
const int SCREEN_H = 720;
const int CELL_SIZE = 40;

const int COLS = SCREEN_W / CELL_SIZE; 
const int ROWS = SCREEN_H / CELL_SIZE; 

struct Ponto { 
    int x;
    int y; 
    bool operator==(const Ponto& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Ponto& other) const { return !(*this == other); }
}; 

enum Celula {
    OCUPADO, 
    LIVRE,
    INICIAL,  
    FINAL     
};

struct Agente{
        int col;
        int row;
        std::vector<Ponto> caminho; 
        size_t indiceCaminho;    
        float tempoMovimento;
};

struct Final{
        int col;
        int row;
};

Agente agente{ -1, -1, {}, 0, 0.0f };
Final final{ -1, -1 };
Celula map[COLS][ROWS];

std::vector<Ponto> encontrarCaminho( Ponto inicio, Ponto fim ) {

    if ( inicio.x == -1 || fim.x == -1 ) return {};

    std::queue<Ponto> fila;
    fila.push(inicio);

    Ponto pais[COLS][ROWS];
    bool visitado[COLS][ROWS];

    for(int i=0; i<COLS; i++)
        for(int j=0; j<ROWS; j++) {
            visitado[i][j] = false;
            pais[i][j] = {-1, -1};
        }

    visitado[inicio.x][inicio.y] = true;
    bool encontrou = false;

    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};

    while(!fila.empty()) {
        Ponto atual = fila.front();
        fila.pop();

        if ( atual ==  fim ) {
            encontrou = true;
            break;
        }

        for(int i=0; i<4; i++) {
            int nx = atual.x + dx[i];
            int ny = atual.y + dy[i];

            if ( nx >= 0 && nx < COLS && ny >= 0 && ny < ROWS ) {
                if (!visitado[nx][ny] && map[nx][ny] != OCUPADO) {
                    visitado[nx][ny] = true;
                    pais[nx][ny] = atual; 
                    fila.push({nx, ny});
                }
            }
        }
    }

    std::vector<Ponto> caminho;
    if (encontrou) {
        Ponto atual = fim;
        while ( atual != inicio) {
            caminho.push_back(atual);
            atual = pais[atual.x][atual.y];
        }
        std::reverse(caminho.begin(), caminho.end());
    }
    return caminho;
}

void recalcularRota() {
    if (agente.col != -1 && final.col != -1) {
        Ponto inicio = {agente.col, agente.row};
        Ponto fim = {final.col, final.row};
        
        agente.caminho = encontrarCaminho(inicio, fim);
        agente.indiceCaminho = 0; 
        agente.tempoMovimento = 0; // Reseta o timer
        
        if (agente.caminho.empty()) {
            std::cout << "Caminho bloqueado ou impossivel!" << std::endl;
        } else {
            std::cout << "Rota calculada: " << agente.caminho.size() << " passos." << std::endl;
        }
    }
}

void p8g::draw() {

    background(0.9); 

    // Lógica de Movimento (Animação)
    if (!agente.caminho.empty() && agente.indiceCaminho < agente.caminho.size()) {

        agente.tempoMovimento += 1.0f / 60.0f; 

        if (agente.tempoMovimento > 0.1f) {
            Ponto proximoPasso = agente.caminho[agente.indiceCaminho];

            if(map[agente.col][agente.row] == INICIAL) {
                 map[agente.col][agente.row] = LIVRE;
            }
            // Move o agente
            agente.col = proximoPasso.x;
            agente.row = proximoPasso.y;
            
            // Atualiza o mapa visualmente para INICIAL (onde o agente está)
            map[agente.col][agente.row] = INICIAL;

            agente.indiceCaminho++;
            agente.tempoMovimento = 0.0f;
        }
    }


    for (int i = 0; i < COLS; i++) {
        for (int j = 0; j < ROWS; j++) {
            
            switch (map[i][j]) {
                case LIVRE:   fill(245, 245, 245); break;
                case OCUPADO: fill(50, 50, 60);    break;
                case INICIAL: fill(255, 128, 0);   break;
                case FINAL:   fill(65, 105, 225);  break;
            }
            stroke(200 / 255.0); 
            rect(i * CELL_SIZE, j * CELL_SIZE, CELL_SIZE, CELL_SIZE);
        }
    }

  
    if (!agente.caminho.empty()) {
        fill(255, 255, 0, 100); // Amarelo com transparência 
        
        for (const auto& p : agente.caminho) {
            if (p.x == final.col && p.y == final.row) continue;
            
            rect(p.x * CELL_SIZE + 5, p.y * CELL_SIZE + 5, CELL_SIZE - 10, CELL_SIZE - 10);
        }
    }

    // Desenho do Agente (Círculo sobreposto)
    if (agente.col != -1 && agente.row != -1) {
        fill(0, 0, 0); // Preto ou cor de destaque
        float cx = agente.col * CELL_SIZE + CELL_SIZE / 2.0f;
        float cy = agente.row * CELL_SIZE + CELL_SIZE / 2.0f;
        ellipse(cx, cy, CELL_SIZE * 0.6f, CELL_SIZE * 0.6f);
    }
}

void p8g::keyPressed() {}
void p8g::keyReleased() {}
void p8g::mouseMoved() {}

void p8g::mousePressed() {

    const int tmpCol = mouseX / CELL_SIZE;
    const int tmpRow = mouseY / CELL_SIZE;

    if ( tmpCol < 0 || tmpCol >= COLS || tmpRow < 0 || tmpRow >= ROWS) return;

    bool mudouAlgo = false; // Flag para saber se precisamos recalcular

    switch ( keyCode )
    {
    case 79: // "O" ocupado
        if( map[tmpCol][tmpRow] == LIVRE ){
            map[tmpCol][tmpRow] = OCUPADO;
            mudouAlgo = true; 
        } else if (map[tmpCol][tmpRow] == OCUPADO) {
             map[tmpCol][tmpRow] = LIVRE; 
             mudouAlgo = true;
        }
        break;
    case 73: // "I" inicial
        if( agente.col != -1 ) map[agente.col][agente.row] = LIVRE; 
        
        map[tmpCol][tmpRow] = INICIAL;
        agente.col = tmpCol;
        agente.row = tmpRow;
        mudouAlgo = true;
        break;
    case 70:  // "F" final
        if( final.col != -1 ) map[final.col][final.row] = LIVRE; 
        
        map[tmpCol][tmpRow] = FINAL;
        final.col = tmpCol;
        final.row = tmpRow;
        mudouAlgo = true;
        break;  
    default:
        break;
    }

    // Se alterou start, fim ou colocou parede recalcula a rota
    if (mudouAlgo) {
        recalcularRota();
    }
}

void p8g::mouseReleased() {}
void p8g::mouseWheel( float delta ) {}

int main() {
    for( int i=0; i<COLS; i++ ){
        for( int j=0; j<ROWS; j++ ){
            map[i][j] = LIVRE;
        }
    }
    {
        using namespace p8g;
        run( SCREEN_W, SCREEN_H, "Navegacao", true );
    }
    return 0;
}