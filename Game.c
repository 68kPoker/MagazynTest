
#include <intuition/screens.h>
#include <clib/intuition_protos.h>
#include <clib/graphics_protos.h>

#include "Cell.h"

#define DEPTH 5
#define FRAMES 400

struct Cell board[ HEIGHT ][ WIDTH ];

struct Screen *s;

BOOL animIn(struct RastPort *rp, struct Cell *c, WORD x, WORD y, BOOL drawn)
{
    WORD dist, dx, dy, sx, sy, ex, ey;

    if (c->anim)
    {
        if (!drawn)
        {
            SetAPen(rp, 0);
            RectFill(rp, x << 4, y << 4, (x << 4) + 15, (y << 4) + 15);
        }

        dist = -c->dist;

        dx = shifts[c->dir] % WIDTH;
        dy = shifts[c->dir] / WIDTH;
        sx = dx * dist;
        sy = dy * dist;
        ex = sx + 15;
        ey = sy + 15;

        if (sx < 0)
        {
            sx = 0;
        }
        if (ex > 15)
        {
            ex = 15;
        }

        if (sy < 0)
        {
            sy = 0;
        }
        if (ey > 15)
        {
            ey = 15;
        }

        sx += (x << 4);
        sy += (y << 4);
        ex += (x << 4);
        ey += (y << 4);

        SetAPen(rp, c->kind);

        if (sx <= ex && sy <= ey)
        {
            RectFill(rp, sx, sy, ex, ey);
        }

        return(TRUE);
    }
    return(FALSE);
}

BOOL animOut(struct RastPort *rp, struct Cell *c, WORD x, WORD y, WORD dir, BOOL drawn)
{
    WORD dist, dx, dy, sx, sy, ex, ey;
    WORD shift = shifts[dir];
    struct Cell *n = c + shift;

    if ((n->dir == dir && n->dist > 0 && !drawn) || (n->dir != dir && c->drawn && !drawn))
    {
        SetAPen(rp, 0);
        RectFill(rp, x << 4, y << 4, (x << 4) + 15, (y << 4) + 15);
        drawn = TRUE;
        c->drawn = FALSE;
    }

    if (n->dir == dir && n->dist > 0)
    {
        dist = 16 - n->dist;

        dx = shift % WIDTH;
        dy = shift / WIDTH;
        sx = dx * dist;
        sy = dy * dist;
        ex = sx + 15;
        ey = sy + 15;

        if (sx < 0)
        {
            sx = 0;
        }
        if (ex > 15)
        {
            ex = 15;
        }

        if (sy < 0)
        {
            sy = 0;
        }
        if (ey > 15)
        {
            ey = 15;
        }
#if 0
        printf("(%2d %2d) -> (%2d %2d)\n", sx, sy, ex, ey);
#endif
        sx += (x << 4);
        sy += (y << 4);
        ex += (x << 4);
        ey += (y << 4);

        SetAPen(rp, n->kind);

        if (sx <= ex && sy <= ey)
        {
            RectFill(rp, sx, sy, ex, ey);
            c->drawn = TRUE;
        }
    }
    return(drawn);
}

void animCell(WORD x, WORD y, struct Cell (*board)[WIDTH])
{
    struct Cell *c = &board[y][x];
    struct RastPort *rp = &s->RastPort;
    BOOL drawn = FALSE;

    drawn = animIn(rp, c, x, y, drawn);
    drawn = animOut(rp, c, x, y, UP, drawn);
    drawn = animOut(rp, c, x, y, LEFT, drawn);
    drawn = animOut(rp, c, x, y, RIGHT, drawn);
    drawn = animOut(rp, c, x, y, DOWN, drawn);
}

void animBoard(struct Cell (*c)[WIDTH])
{
    WORD x, y;

    for (y = 1; y < HEIGHT - 1; y++)
    {
        for (x = 1; x < WIDTH - 1; x++)
        {
            animCell(x, y, c);
            c[y][x].anim = FALSE;
        }
    }
}

int main( void )
{
    if (s = OpenScreenTags(NULL,
        SA_DisplayID, PAL_MONITOR_ID|LORES_KEY,
        SA_Depth, DEPTH,
        SA_Width, 320,
        SA_Height, 256,
        SA_Title, "Magazyn 2.0",
        SA_Interleaved, TRUE,
        TAG_DONE))
    {
        WORD i;
        struct Cell (*c)[WIDTH] = board;

        setCell(&c[8][1], WALL, NONE);
        setCell(&c[8][WIDTH - 2], WALL, NONE);
        setCell(&c[8][2], SKULL, RIGHT);

        setCell(&c[10][WIDTH - 3], DEBRIS, NONE);
        setCell(&c[9][WIDTH - 3], DEBRIS, NONE);
        setCell(&c[8][WIDTH - 3], DEBRIS, NONE);

        animBoard(board);

        for (i = 0; i < FRAMES; i++)
        {
            if ((i % 90) == 0)
            {
                setCell(&c[2][WIDTH - 3], BULLET, DOWN);
            }

            WaitTOF();
            handleBoard(board);
            animBoard(board);
        }
        CloseScreen(s);
    }

    return( 0 );
}
