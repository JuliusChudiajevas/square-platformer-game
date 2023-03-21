#include "RenderState.h"
#include "testObjektas.cpp"
#include "utils.cpp"

extern RenderState renderState;

internal void clearScreen( unsigned int color = 0x0 ) {
    FillMemory( renderState.memory, renderState.widht * renderState.height * sizeof( unsigned int ), color );
    // unsigned int *pixel = (unsigned int *)renderState.memory;
    // for (int y = 0; y < renderState.widht; y++) {
    // for (int x = 0; x < renderState.height; x++) {
    //*pixel = color;
    // pixel++;
    //}
    //}
}

internal void drawPixels( int x0, int y0, int x1, int y1, unsigned int color ) {
    x0 = clamp( x0, 0, renderState.widht );
    x1 = clamp( x1, 0, renderState.widht );
    y0 = clamp( y0, 0, renderState.height );
    y1 = clamp( y1, 0, renderState.height );

    for ( int y = y0; y < y1; y++ ) {
        unsigned int *pixel = (unsigned int *)renderState.memory + x0 + y * renderState.widht;
        for ( int x = x0; x < x1; x++ ) {
            *pixel = color;
            pixel++;
        }
    }
}

internal void drawRectangle( float startX, float startY, float sizeX, float sizeY, unsigned int color ) {
    startX *= renderState.height;
    startY *= renderState.height;
    sizeX *= renderState.height;
    sizeY *= renderState.height;

    int x0 = startX;
    int x1 = startX + sizeX;
    int y0 = startY;
    int y1 = startY + sizeY;
    drawPixels( x0, y0, x1, y1, color );
}

internal void drawRectangle( Objektas *o ) {
    int startX = o->getPosX() * renderState.height;
    int startY = o->getPosY() * renderState.height;
    int sizeX = o->getSizeX() * renderState.height;
    int sizeY = o->getSizeY() * renderState.height;
    unsigned int color = o->getColor();
    int x0 = startX;
    int x1 = startX + sizeX;
    int y0 = startY;
    int y1 = startY + sizeY;
    drawPixels( x0, y0, x1, y1, color );
}


internal void renderGame( std::vector<Player *> const &p, std::vector<Objektas *> const &ptrToAllObj,
                          std::map<float, Position> const &deathPosMap ) {
    clearScreen( 0xD2D2D2 );
    // render player
    drawRectangle( p[ 0 ] );
    // object render
    for ( int i = 0; i < ptrToAllObj.size(); i++ ) {
        drawRectangle( ptrToAllObj[ i ] );
    }
    // draw death pos particle
    for ( auto itr = deathPosMap.begin(); itr != deathPosMap.end(); itr++ ) {
        drawRectangle( itr->second.x, itr->second.y, 0.03, 0.03, 0x27E400 );
    }
}
