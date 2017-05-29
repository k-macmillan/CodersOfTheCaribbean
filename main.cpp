
/*
NOTES:


*/


#include <iostream>
#include <string>
#include <vector>   // may optimze out
#include <algorithm>
#include <climits>

using namespace std;

int _turn;

// Basic map unit. Coordinates exist as X,Y but are better suited to X,Y,Z. Most
// calculations will utilize XYZ.
struct Cube
{
    Cube () {}
    Cube (int X, int Y) : x( X - (Y - (Y & 1)) / 2), z(Y), Xo(X), Yo(Y) {y = -x-z;}
    Cube (int X, int Y, int Z) : x(X), y(Y), z(Z), Xo(X + (Z - (Z & 1)) / 2), Yo(Z) {}
    int x = INT_MAX;
    int y = INT_MAX;
    int z = INT_MAX;

    int Xo = INT_MAX; // odd-r
    int Yo = INT_MAX; // odd-r
};

inline bool operator==(const Cube& lhs, const Cube& rhs){ return lhs.Xo == rhs.Xo && lhs.Yo == rhs.Yo; };
inline bool operator!=(const Cube& lhs, const Cube& rhs){ return !operator==(lhs,rhs); };

struct Ship
{
    Ship() {}
    Ship(int X, int Y, int ID, int Direction, int Rum, int Speed) : loc(Cube(X,Y)), id(ID), dir(Direction), rum(Rum), speed(Speed) {}
    Cube loc;

    int dir;
    int id = -1;
    int mine_avail;      // Last turn a mine was dropped
    int fire_avail;      // Last turn a shot was fired (can only fire every other)
    int rum;
    int speed;
};
vector<Ship> _my_ships;
vector<Ship> _en_ships;

struct Barrel
{
    Barrel() {}
    Barrel(int X, int Y, int Rum, int ID) : loc(Cube(X,Y)), rum(Rum), id(ID) {}
    Cube loc;

    int id = -1;
    int rum;
    bool enroute = false;
    bool marked = false;    // marked for destruction...cannon ball incoming
};
vector<Barrel> _barrels;

struct Cannonball
{
    Cannonball() {}
    Cannonball(int X, int Y, int Impact, int Turn, int ID) : loc(Cube(X,Y)), impact(Impact), turn(Turn), id(ID) {}
    Cube loc;

    int turn;
    int impact;
    int id = -1;
    Ship* origin = nullptr;     // Primarily keeping track of this to identify which ship cannot fire
};
vector<Cannonball> _cbs;

struct Mine
{
    Mine() {}
    Mine(int X, int Y, int ID) : loc(Cube(X,Y)), id(ID) {}
    Cube loc;

    int id = -1;
};
vector<Mine> _mines;





int main()
{
    _turn = 0;


    while (1) {
        vector<Ship> my_ships,en_ships;
        int myShipCount; // the number of remaining ships
        cin >> myShipCount; cin.ignore();
        int entityCount; // the number of entities (e.g. ships, mines or cannonballs)
        cin >> entityCount; cin.ignore();
        for (int i = 0; i < entityCount; i++) {
            int entityId;
            string entityType;
            int x;
            int y;
            int arg1;
            int arg2;
            int arg3;
            int arg4;
            cin >> entityId >> entityType >> x >> y >> arg1 >> arg2 >> arg3 >> arg4; cin.ignore();
            bool found = false;
            if (entityType == "SHIP")
            {
                Ship new_ship(x,y, entityId,arg1,arg3,arg2);                
                if (arg4 == 1)
                {
                    for (unsigned int j = 0; j < _my_ships.size(); ++j)
                    {
                        if (_my_ships[j].id == new_ship.id)
                        {
                            found = true;
                            _my_ships[j].loc = new_ship.loc;
                            _my_ships[j].dir = new_ship.dir;
                            _my_ships[j].rum = new_ship.rum;
                            _my_ships[j].speed = new_ship.speed;
                        }
                    }
                    if (!found)
                        _my_ships.emplace_back(new_ship);

                }
                else
                {
                    for (unsigned int j = 0; j < _en_ships.size(); ++j)
                    {
                        if (_en_ships[j].id == new_ship.id)
                        {
                            found = true;
                            _en_ships[j].loc = new_ship.loc;
                            _en_ships[j].dir = new_ship.dir;
                            _en_ships[j].rum = new_ship.rum;
                            _en_ships[j].speed = new_ship.speed;
                        }
                    }
                    if (!found)
                        _en_ships.emplace_back(new_ship);
                }
            }
            else if (entityType == "BARREL")
            {
                Barrel new_barrel(x,y,arg1,entityId);
                for (unsigned int j = 0; j < _barrels.size(); ++j)
                {
                    if (_barrels[j].id == new_barrel.id)
                    {
                        found = true;
                        _barrels[j].loc = new_barrel.loc;
                        _barrels[j].rum = new_barrel.rum;
                    }
                }
                if (!found)
                    _barrels.emplace_back(new_barrel);                
            }
            else if (entityType == "CANNONBALL")
            {
                Cannonball new_cb(x,y,arg2,_turn,entityId);
                for (unsigned int j = 0; j < _cbs.size(); ++j)
                    if (_cbs[j].id == new_cb.id)
                        found = true;

                if (!found)
                {
                    for (unsigned int j = 0; j < _en_ships.size(); ++j)
                        if (arg2 == _en_ships[j].id)
                            new_cb.origin = &_en_ships[j];     // Update now or later??? (ship that launched)
                    _cbs.emplace_back(new_cb);                    
                }
            }
            else if (entityType == "MINE")
            {
                Mine new_mine(x,y,entityId);
                for (unsigned int j = 0; j < _mines.size(); ++j)
                    if (_mines[j].id == new_mine.id)
                        found = true;

                if (!found)
                    _mines.emplace_back(new_mine);                    
            }

        }
        for (int i = 0; i < myShipCount; i++) {

            // Write an action using cout. DON'T FORGET THE "<< endl"
            // To debug: cerr << "Debug messages..." << endl;

            cout << "MOVE 11 10" << endl; // Any valid action, such as "WAIT" or "MOVE x y"
        }
    }



    return 0;
}