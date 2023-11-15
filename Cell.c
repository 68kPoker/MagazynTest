
#include "Cell.h"

BYTE shifts[ DIRS ] = { 0, -WIDTH, 1, WIDTH, -1 };
WORD score, keys;

void setCell( struct Cell *c, UBYTE kind, UBYTE dir )
{
    c->kind = kind;
    c->dir = dir;
    c->mode = ACTIVE;
    c->frame = 0;    
}

void moveCell( struct Cell *src, BYTE shift, BOOL discard )
{
    struct Cell *dest = src + shift;

    *dest = *src;

    if( discard )
    {
        setCell( src, FLOOR, NONE );
    }

    dest->dist = START_DIST;
    dest->mode = MOVING;

    if( shift > 0 )
    {
        dest->visited = TRUE;
    }
}

void moveObject( struct Cell *c )
{
    if( c->dist > 0 )
    {
        c->dist -= SPEED;
    }

    if( c->dir == 0 )
    {
        c->mode = ACTIVE;
    }
}

void moveSetCell( struct Cell *c, BYTE shift, UBYTE kind, UBYTE dir, BOOL discard )
{
    setCell( c + shift, kind, dir );

    if( discard )
    {
        setCell( c, FLOOR, NONE );
    }

    if( shift > 0 )
    {
        c[ shift ].visited = TRUE;
    }
}

void handleCell( struct Cell *c, void ( *handlers[ KINDS ] )( struct Cell *c ) )
{
    void ( *handler )( struct Cell *c ) = handlers + c->kind;

    if( !c->visited && handler )
    {
        handler( c );
    }

    c->visited = FALSE;
}

void handleBoard( struct Cell( *c )[ WIDTH ] )
{
    static void ( *handlers[ KINDS ] )( struct Cell *c ) =
    {
        NULL,
        NULL,
        handleHero,
        NULL,
        handleBox,
        NULL,
        handleBullet,
        handleFire,
        handleSkull,
        NULL,
        NULL,
        NULL
    };
    WORD x, y;

    for( y = 0; y < HEIGHT; y++ )
    {
        for( x = 0; x < WIDTH; x++ )
        {
            handleCell( &c[ y ][ x ], handlers );
        }
    }
}

void handleBullet( struct Cell *c )
{
    BYTE shift = shifts[ c->dir ];
    struct Cell *dest = c + shift;

    if( c->mode == MOVING )
    {
        moveObject( c );
    }
    
    if( c->mode == ACTIVE )
    {        
        switch( dest->kind )
        {
        case FLOOR:
            moveCell( c, shift, TRUE );
            break;
        case HERO:
        case SKULL:
        case DEBRIS:            
            moveSetCell( c, shift, FIRE, NONE, TRUE );
            break;
        default:
            setCell( c, FIRE, NONE );
            break;
        }
    }
}

void handleFire( struct Cell *c )
{
    if( c->frame < FIRE_FRAMES )
    {
        c->frame++;
    }
    else
    {
        setCell( c, FLOOR, NONE );
    }
}

void handleSkull( struct Cell *c )
{
    BYTE shift = shifts[ c->dir ];
    struct Cell *dest = c + shift;

    switch( dest->kind )
    {
    case FLOOR:
        moveCell( c, shift, TRUE );
        break;
    case BULLET:
    case HERO:
        moveSetCell( c, shift, FIRE, NONE, TRUE );
        break;
    default:
        c->dir = opposite( c->dir );
        break;
    }
}

void handleHero( struct Cell *c )
{
    BYTE shift = shifts[ c->dir ];
    struct Cell *dest, *past;

    if( c->mode == MOVING )
    {
        moveObject( c );
    }

    if( c->mode == ACTIVE )
    {
        if( shift )
        {
            dest = c + shift;

            switch( dest->kind )
            {
            case FLOOR:
                moveCell( c, shift, TRUE );
                break;
            case CHERRY:
                score += CHERRY_SCORE;
                break;
            case KEY:
                keys++;
                break;
            case DOOR:
                if( keys > 0 )
                {
                    keys--;
                    setCell( dest, FLOOR, NONE );
                    break;
                }
                break;
            case BOX:
                past = dest + shift;
                if( dest->mode == ACTIVE && past->kind == FLOOR )
                {
                    moveCell( dest, shift, FALSE );
                    moveCell( c, shift, TRUE );
                }
                break;
            case SKULL:
            case BULLET:
                moveSetCell( c, shift, FIRE, NONE, TRUE );
                break;
            default:
                break;
            }
        }
    }
}

void handleBox( struct Cell *c )
{
    if( c->mode == MOVING )
    {
        moveObject( c );
    }
}
