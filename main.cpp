#include <iostream>
#include <string>
#include <vector>   // may optimze out
#include <algorithm>
#include <climits>

using namespace std;

// Basic map unit. Coordinates exist as X,Y but are better suited to X,Y,Z. Most
// calculations will utilize XYZ.
struct Cube
{
    Cube () {}
    Cube (int X, int Y) : x( X - (Y - (Y & 1)) / 2), z(Y), Xo(X), Yo(Y) {y = -x-z;}
    Cube (int X, int Y, int Z) : x(X), y(Y), z(Z), Xo(X), Yo(Y) {}
    int x = INT_MAX;
    int y = INT_MAX;
    int z = INT_MAX;

    int Xo = INT_MAX; // odd-r
    int Yo = INT_MAX; // odd-r
};

inline bool operator==(const Cube& lhs, const Cube& rhs){ return lhs.Xo == rhs.Xo && lhs.Yo == rhs.Yo; };
inline bool operator!=(const Cube& lhs, const Cube& rhs){ return !operator==(lhs,rhs); };

int main()
{
    




    return 0;
}