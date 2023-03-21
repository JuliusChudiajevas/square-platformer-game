#include <math.h>

#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <vector>

class Objektas {
   protected:
    float posx, posy;
    float sizex, sizey;
    char type;
    unsigned int color;

   public:
    Objektas() {
        posx = 0.0f;
        posy = 0.0f;
        sizey = 0.0f;
        sizex = 0.0f;
        color = 0x550033;
        type = 'a';
    }
    Objektas( float x, float y, float sizex, float sizey ) {
        posx = x;
        posy = y;
        this->sizex = sizex;
        this->sizey = sizey;
        type = 'a';
        color = 0x550033;
    }
    ~Objektas() {
    }
    virtual void initializeColor() {
    }

    unsigned int getColor() {
        return color;
    }
    void setObjektas( float x, float y, float sizex, float sizey ) {
        posx = x;
        posy = y;
        this->sizex = sizex;
        this->sizey = sizey;
    }
    float const getPosX() {
        return posx;
    }
    float const getPosY() {
        return posy;
    }
    float const getSizeX() {
        return sizex;
    }
    float const getSizeY() {
        return sizey;
    }
    char const getType() {
        return type;
    }
};

class Enemy : public Objektas {
   private:
   public:
    Enemy( float x, float y ) {
        type = 'E';
        color = 0xAA0033;
        posx = x;
        posy = y;
        sizex = 0.04;
        sizey = 0.1;
    }
    ~Enemy() {
    }
    void initializeColor() override {
        color = 0xAA0033;
    }
};

class Counter {
   protected:
    int count;

   public:
    Counter() {
        count = 0;
    }
    ~Counter() {
        std::ofstream rf;
        rf.open( "deathCount.txt" );
        rf << count;
        rf.close();
    }
    virtual void operator++() {  // operator override
        count = count + 1;
    }
    virtual void operator--() {
        count = count - 1;
    }
    int const getCount() {
        return count;
    }
};


struct Position {
    float x, y;
};

class DeathCounter : public Counter {
   private:
    int logSize;
    std::set<float> posID;
    std::map<float, Position> pos;

   public:
    DeathCounter() {
        logSize = 4;
    }
    DeathCounter( int logS ) {
        logSize = logS;
    }
    void addPoint( float x, float y ) {
        count++;
        std::set<float>::iterator closestElement;
        std::set<float>::iterator newestElement;
        float tempID = float( std::round( ( x - y ) * 100 ) ) / 100.0;
        Position tempPos;
        tempPos.x = x;
        tempPos.y = y;
        if ( posID.find( tempID ) == posID.end() ) {  // jei tokio elemento nera
            posID.insert( tempID );                   // idedam i seta
            pos[ tempID ] = tempPos;                  // idedam i mapa
            newestElement = posID.find( tempID );
        }
        if ( posID.size() > logSize ) {  // jei per didelis istrinam elementa
            for ( auto itr = posID.begin(); itr != posID.end(); itr++ ) {
                float min = 0;
                if ( std::abs( *itr ) > min && itr != newestElement ) {
                    min = std::abs( *itr );
                    closestElement = itr;
                }
            }
            pos.erase( *closestElement );
            posID.erase( closestElement );
            closestElement = posID.begin();
        }
    }
    std::map<float, Position> const getPositionMap() {
        return pos;
    }
};

class Player : public Objektas {
   private:
    float gravity;
    float fallSpeed;
    float speed;
    bool touchedGround;

   public:
    Player( float x, float y ) {
        type = 'P';
        posx = x;
        posy = y;
        sizex = 0.05;
        sizey = 0.05;
        speed = 0.7;
        gravity = 1.5;
        fallSpeed = 0.0;
        touchedGround = 0;
        color = 0x0B54FA;
    }
    void update( const float deltaTime, std::vector<Objektas *> &ptrToAllObj, DeathCounter &deathCounter ) {
        float margin = 0.005;
        fallSpeed -= ( fallSpeed + ( gravity * gravity ) ) * deltaTime;

        for ( int i = 0; i < ptrToAllObj.size(); i++ ) {
            char objType = ptrToAllObj[ i ]->getType();
            float objx = ptrToAllObj[ i ]->getPosX();
            float objy = ptrToAllObj[ i ]->getPosY();
            float objSizeX = ptrToAllObj[ i ]->getSizeX();
            float objSizeY = ptrToAllObj[ i ]->getSizeY();
            if ( posy > objy + objSizeY ) {  // jeigu player yra virs objekto
                //	nieko
                continue;  // einam prie kito objekto
            }
            if ( posy + sizey < objy ) {  // jeigu player yra po objektu
                //	nieko
                continue;
            }
            if ( posx + sizex < objx ) {         // jeigu player neliecia objekto is kaires
                if ( posx > objx + objSizeX ) {  // jeigu player neliecia objekto is desines
                    // nieko
                    continue;
                }
            }
            // jeigu player isimuryja i objekta is kaires
            // jeigu player desinys apatinis taskas ieina i objekto kaire x vertrikalia linija
            //  BET apatinis taskas liecia tik objekto kairy sona
            // IR player desinys apatinis taskas yra auksciau objekto apacios
            // IR objekto apatinis desinys taskas yra zemiau objekto virsaus
            if ( posx + sizex >= objx && posx + sizex < objx + margin && posy + sizey > objy &&
                 posy < objy + objSizeY ) {
                if ( objType == 'E' ) {
                    deathCounter.addPoint( posx + sizex, posy );
                    posx = 0.2;
                    posy = 0.2;
                } else
                    posx = objx - sizex;
            }
            // jeigu player isimuryja i objekta is desines
            // JEIGU player kairys x taskas ieina i objekto desiny x taska
            // BET player kairys x taskas nera gyliau nei objekto siena
            // IR player desinys apatinis taskas yra auksciau objekto apacios
            // IR objekto apatinis desinys taskas yra zemiau objekto virsaus
            if ( posx <= objx + objSizeX && posx > objx + objSizeX - margin && posy + sizey > objy &&
                 posy < objy + objSizeY ) {
                if ( objType == 'E' ) {
                    deathCounter.addPoint( posx + sizex, posy );
                    posx = 0.2;
                    posy = 0.2;
                } else
                    posx = objx + objSizeX;
            }
            // jeigu player x linija patenka i objekto x linija arba objecto x linija patenka i player x linija
            if ( posx >= objx + margin && posx <= objx + objSizeX - margin ||
                 posx + sizex >= objx + margin && posx + sizex <= objx + objSizeX - margin ||
                 objx >= posx + margin && objx <= posx + sizex - margin ||
                 objx + objSizeX >= posx + margin && objx + objSizeX < posx + sizex - margin ) {
                // jeigu player isimuryja i objekta is virsaus
                // jeigu player y koodrinate yra zemiau objekto BET nezemiau nei objekto pavirsius
                if ( posy <= objy + objSizeY && posy >= objy + objSizeY - margin ) {
                    if ( objType == 'E' ) {
                        deathCounter.addPoint( posx + sizex, posy );
                        posx = 0.2;
                        posy = 0.2;
                    } else {
                        touchedGround = true;
                        posy = objy + objSizeY;
                        fallSpeed = 0.0;
                    }
                }
                // jeigu player isimuryja i objekta is apacios
                // jeigu player virsutine y koordinate didesne uz objekto apacia BET ne didesne nei objekto apacios
                // pavirs
                if ( posy + sizey >= objy && posy + sizey < objy + margin ) {
                    if ( objType == 'E' ) {
                        deathCounter.addPoint( posx + sizex, posy );
                        posx = 0.2;
                        posy = 0.2;
                    } else
                        posy = objy - sizey;
                    fallSpeed *= -1.0;
                    continue;
                }
            }
        }
        posy += fallSpeed * deltaTime;
    }
    void jump( float deltaTime ) {
        if ( touchedGround ) {
            posy += 0.0001;
            touchedGround = false;
            fallSpeed = 0.9;
        }
    }
    void moveDown( float deltaTime ) {
        posy -= speed * deltaTime;
    }
    void moveLeft( float deltaTime ) {
        posx -= speed * deltaTime;
    }
    void moveRight( float deltaTime ) {
        posx += speed * deltaTime;
    }
};
