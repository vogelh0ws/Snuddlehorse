#include <SDL/SDL.h> 
#include <stdlib.h>
#include <stdio.h>

#define MEADOW_COLOR 0x339933

const int FLIP_VERTICAL = 1;
const int FLIP_HORIZONTAL = 2;

typedef enum
{
	left,
	right,
	upright,
	upleft,
	downright,
	downleft,
} Direction;

typedef struct 
{
	int		xpos;
	int		ypos;
	SDL_Surface*	pSurfRight;
	SDL_Surface*	pSurfLeft;
} Pferd;

SDL_Surface* m_screen;
SDL_Surface* m_background;

int get_pixel32( SDL_Surface *surface, int x, int y )
{
	int *pixels = (int*) surface->pixels;

	return pixels[ (y * surface->w ) + x];
}

void put_pixel32( SDL_Surface *surface, int x, int y, int pixel )
{
	int *pixels = (int*) surface->pixels;

	pixels[ (y * surface->w ) + x ] = pixel;
}

SDL_Surface *flip_surface( SDL_Surface *surface, int flags )
{
	//TODO copying colorkey does not work
	int pixel;
	SDL_Surface *flipped;

	if (surface->flags & SDL_SRCCOLORKEY)
	{
		flipped = SDL_CreateRGBSurface( SDL_SWSURFACE , surface->w, surface->h, surface->format->BitsPerPixel, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, 0 );
	}
	else
	{
		flipped = SDL_CreateRGBSurface( SDL_SWSURFACE, surface->w, surface->h, surface->format->BitsPerPixel, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask );
	}


	if (SDL_MUSTLOCK( surface ))
	{
		SDL_LockSurface( surface );
	}

	for ( int x = 0, rx = flipped->w - 1; x < flipped->w; x++, rx-- )
	{
		for ( int y = 0, ry = flipped->h - 1; y < flipped->h; y++, ry-- )
		{
			pixel = get_pixel32( surface, x, y );
			if( ( flags & FLIP_VERTICAL ) && ( flags & FLIP_HORIZONTAL ) )
			{
				put_pixel32( flipped, rx, ry, pixel );
			}
			else if ( flags & FLIP_HORIZONTAL )
			{
				put_pixel32( flipped, rx, y, pixel );
			}
			else if ( flags & FLIP_VERTICAL )
			{
				put_pixel32( flipped, x, ry, pixel );
			}
		}
	}

	if ( SDL_MUSTLOCK( surface ) )
	{
		SDL_UnlockSurface( surface );
	}
	
	if( surface->flags & SDL_SRCCOLORKEY )
	{
		SDL_SetColorKey( flipped, SDL_RLEACCEL | SDL_SRCCOLORKEY, surface->format->colorkey );
	}

	return flipped;

}

Pferd* createPferd( int xpos, int ypos, char* bmpFilename )
{
	int colorkey;

	Pferd* pNewPferd = malloc( sizeof(Pferd) );

	pNewPferd->xpos = xpos;
	pNewPferd->ypos = ypos;

	pNewPferd->pSurfRight = SDL_DisplayFormatAlpha( SDL_LoadBMP ( bmpFilename ) );
	
	colorkey = SDL_MapRGB( pNewPferd->pSurfRight->format, 0xFF, 0x00, 0x00 );
	SDL_SetColorKey( pNewPferd->pSurfRight, SDL_SRCCOLORKEY, colorkey );

	pNewPferd->pSurfLeft = flip_surface( pNewPferd->pSurfRight, FLIP_HORIZONTAL|SDL_SRCCOLORKEY );

	return pNewPferd;
}

void destroyPferd( Pferd* pPferd )
{
	free( pPferd );
}

void movePferd( Pferd* pPferd, Direction direct, int dist )
{
	SDL_Rect pferdPos;
	SDL_Rect zeroRect;

	zeroRect.x = 0;
	zeroRect.y = 0;

	while ( dist > 0 )
	{
		switch (direct) 
		{
			case left:
				pPferd->xpos -= 20;
				break;
			case right:
				pPferd->xpos += 20;
				break;
			case upright:
				pPferd->ypos -= 10;
				pPferd->xpos += 10;
				break;
			case upleft:
				pPferd->ypos -= 10;
				pPferd->xpos -= 10;
				break;
			case downright:
				pPferd->ypos += 10;
				pPferd->xpos += 10;
				break;
			case downleft:
				pPferd->ypos += 10;
				pPferd->xpos -= 10;
				break;
		}
		dist -= 20;
		
		pferdPos.x = pPferd->xpos;
		pferdPos.y = pPferd->ypos;

		// SDL_FillRect( m_screen, &m_screen->clip_rect, MEADOW_COLOR );
		SDL_BlitSurface( m_background, NULL, m_screen, &zeroRect );
		
		switch (direct)
		{
			case left:
			case upleft:
			case downleft:
				SDL_BlitSurface( pPferd->pSurfLeft, NULL, m_screen, &pferdPos );
				break;
			case right:
			case upright:
			case downright:
				SDL_BlitSurface( pPferd->pSurfRight, NULL, m_screen, &pferdPos );
				break;
			default:
				SDL_BlitSurface( pPferd->pSurfLeft, NULL, m_screen, &pferdPos );
		}
		
		SDL_Flip( m_screen );
		SDL_Delay( 250 );
	}
	
}

void wanderPferd( Pferd* pPferd, int times )
{
	int distance;

	while (times)
	{
		distance = ( rand() % 500 );
		if ( ((pPferd->xpos + distance + pPferd->pSurfRight->w) < m_screen->w ) &&
				((pPferd->ypos + distance + pPferd->pSurfRight->h) < m_screen->h ) )
		{
			movePferd( pPferd, downright, distance );
			times--;
		}
		else if( ((pPferd->xpos + distance + pPferd->pSurfRight->w) < m_screen->w ) &&
				((pPferd->ypos - distance ) > 0 ) )
		{
			movePferd( pPferd, upright, distance );
			times--;
		}
		else if( ((pPferd->xpos - distance) > 0 ) &&
				((pPferd->ypos + distance + pPferd->pSurfRight->h) < m_screen->h ) )
		{
			movePferd( pPferd, downleft, distance );
			times--;
		}
		else if( ((pPferd->xpos - distance) > 0 ) &&
				((pPferd->ypos - distance ) > 0 ) )
		{
			movePferd( pPferd, upleft, distance );
			times--;
		}
		else if ( (pPferd->xpos + distance + pPferd->pSurfRight->w) < m_screen->w )
		{
			movePferd( pPferd, right, distance );
			times--;
		}  
		else if ( (pPferd->xpos - distance) > 0 )
		{
			movePferd( pPferd, left, distance );
			times--;
		}
	}	
}

int main( int argc, char* args[] ) 
{ 
	Pferd* pSPferd;
	//Start SDL 
	SDL_Init( SDL_INIT_EVERYTHING ); 
	
	m_screen = SDL_SetVideoMode( 800, 600, 32, SDL_SWSURFACE );
	m_background = SDL_LoadBMP( "background.bmp" );
	
	pSPferd = createPferd( 400, 300, "pferd.bmp" );

	wanderPferd( pSPferd, 5 );

	SDL_Delay( 2000 );

	//Quit SDL 
	SDL_Quit(); 
	
	return 0; 
}

