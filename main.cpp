
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

void InFront(Cube &c, int dir);



class Ship
{
public:
    struct ShipVec
    {
        ShipVec() {}
        ShipVec(Cube Location, int Direction, int Speed) : loc(Location), dir(Direction), speed(Speed) {}
        Cube loc;
        int dir;
        int speed;
    };

    Ship() {}
    Ship(int X, int Y, int ID, int Direction, int Rum, int Speed) : id(ID), rum(Rum), vec(ShipVec(Cube(X,Y),Direction,Speed)) {}
    Ship(int ID, int Rum, ShipVec Ship_vector) : id(ID), rum(Rum), vec(Ship_vector) {}
    ShipVec vec;

    int id = -1;
    int mine_avail;      // Last turn a mine was dropped
    int fire_avail;      // Last turn a shot was fired (can only fire every other)
    int rum;

    vector<ShipVec> PossibleMoves()
    {
        vector<ShipVec> ret_val;

        Cube new_loc = vec.loc;
        int new_starboard_dir = vec.dir == 0 ? 5 : vec.dir - 1;
        int new_port_dir = vec.dir == 5 ? 0 : vec.dir + 1;


        if (vec.speed == 0)
        {
            ShipVec starboard(new_loc,new_starboard_dir, 0);
            ShipVec port(new_loc, new_port_dir, 0);

            InFront(new_loc, vec.dir);
            ShipVec faster(new_loc, vec.dir, 1);

            ret_val.emplace_back(starboard);
            ret_val.emplace_back(port);
            ret_val.emplace_back(faster);
        }
        else if (vec.speed == 1)
        {
            ShipVec slower(new_loc,vec.dir, 0);

            InFront(new_loc, vec.dir);
            ShipVec starboard(new_loc,new_starboard_dir, 1);
            ShipVec port(new_loc, new_port_dir, 1);

            InFront(new_loc, vec.dir);
            ShipVec faster(new_loc, vec.dir, 2);

            ret_val.emplace_back(slower);
            ret_val.emplace_back(starboard);
            ret_val.emplace_back(port);
            ret_val.emplace_back(faster);            
        }
        else
        {
            InFront(new_loc, vec.dir);
            ShipVec slower(new_loc,vec.dir, 1);

            InFront(new_loc, vec.dir);
            ShipVec starboard(new_loc,new_starboard_dir, 2);
            ShipVec port(new_loc, new_port_dir, 2);            
            
            ret_val.emplace_back(slower);
            ret_val.emplace_back(starboard);
            ret_val.emplace_back(port);
        }
        return ret_val;
    }

    vector<Cube> PossibleShots(int sim_turn)
    {
        // Cannot fire?
        if (sim_turn == fire_avail)
            return {};

        vector<Cube> ret_val;
        // Can be optimized:
        for (unsigned int i = -5 + vec.loc.x; i <= 5 + vec.loc.x; ++i)
            for (unsigned int j = -5 + vec.loc.y; j <= 5 + vec.loc.y; ++j)
                for (unsigned int k = -5 + vec.loc.z; k <= 5 + vec.loc.z; ++k)
                    if (i + j + k == 0)
                        ret_val.emplace_back(i,j,k);
    }

    Cube PossibleMine(int sim_turn)
    {
        if (sim_turn - mine_avail > 4)
            return {};
        else
        {
            Cube mine_drop = vec.loc;       // ship center
            int drop_dir = (vec.dir + 3) % 6;
            InFront(mine_drop, drop_dir);   // stern
            InFront(mine_drop, drop_dir);   // cell directly behind the ship
        }
    }
private:

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
                            _my_ships[j].vec.loc = new_ship.vec.loc;
                            _my_ships[j].vec.dir = new_ship.vec.dir;
                            _my_ships[j].vec.speed = new_ship.vec.speed;
                            _my_ships[j].rum = new_ship.rum;                            
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
                            _en_ships[j].vec.loc = new_ship.vec.loc;
                            _en_ships[j].vec.dir = new_ship.vec.dir;
                            _en_ships[j].vec.speed = new_ship.vec.speed;
                            _en_ships[j].rum = new_ship.rum;                            
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



void InFront(Cube &c, int dir)
{
    if (dir == 0)
    {
        c.x += 1;
        c.y -= 1;
    }
    else if (dir == 3)
    {
        c.x -= 1;
        c.y += 1;
    }
    else if (dir == 1)
    {
        c.x += 1;
        c.z -= 1;
    }
    else if (dir == 4)
    {
        c.x -= 1;
        c.z += 1;
    }
    else if (dir == 2)
    {
        c.y += 1;
        c.z -= 1;
    }
    else
    {
        c.y -= 1;
        c.z += 1;
    }
}

