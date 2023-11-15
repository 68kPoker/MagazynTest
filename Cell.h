
#include <exec/types.h>

#define START_DIST   16
#define SPEED        2 /* Standard speed */

#define WIDTH        16
#define HEIGHT       16

#define FIRE_FRAMES  8

#define opposite( dir ) ( ( dir ) - 1 ) ^ 1

#define CHERRY_SCORE 1

enum Kind
{
    FLOOR,
    WALL,
    HERO,
    CHERRY,
    BOX,
    AMMO,
    BULLET,
    FIRE,
    DEBRIS,
    SKULL,
    KEY,
    DOOR,
    EXIT,
    KINDS
};

enum Mode
{
    ACTIVE,
    MOVING,
    SHOOT
};

enum Dir
{
    NONE,
    UP,
    RIGHT,
    DOWN,
    LEFT,
    DIRS
};

enum Action
{
    BLOCK,
    MOVE,
    DESTROY,
    PUSH
};

struct Cell
{
    UBYTE kind;
    UBYTE mode;
    UBYTE frame;
    UBYTE dir;
    UBYTE visited;
    UBYTE dist;
};

void setCell( struct Cell *c, UBYTE kind, UBYTE dir );
void moveCell( struct Cell *src, BYTE shift, BOOL discard );
void moveSetCell( struct Cell *c, BYTE shift, UBYTE kind, UBYTE dir, BOOL discard );
void moveObject( struct Cell *c );
void handleCell( struct Cell *c, void ( *handlers[ KINDS ] )( struct Cell *c ) );
void handleBoard( struct Cell( *c )[ WIDTH ] );
