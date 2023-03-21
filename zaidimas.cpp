#include "utils.cpp"
#define isdDown( b ) ( input->buttons[ b ].down )
#define pressed( b ) ( input->buttons[ b ].down && input->buttons[ b ].changed )
#define released( b ) ( !input->buttons[ b ].down && input->buttons[ b ].changed )

internal void simulateGame( Input *input, const float &deltaTime, std::vector<Player *> &p,
                            std::vector<Objektas *> &ptrToAllObjects, DeathCounter &deathCounter ) {
    p[ 0 ]->update( deltaTime, ptrToAllObjects, deathCounter );
    if ( isdDown( ButtonLeft ) ) {
        p[ 0 ]->moveLeft( deltaTime );
    }
    if ( isdDown( ButtonRight ) ) {
        p[ 0 ]->moveRight( deltaTime );
    }
    if ( pressed( ButtonUp ) ) {
        p[ 0 ]->jump( deltaTime );
    }
}