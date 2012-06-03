
#include <SDL/SDL.h> 
#include <stdlib.h>
#include <stdio.h>

#define DEBUG

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
	int		dist;
	Direction	direct;
	SDL_Surface*	pSurfRight;
	SDL_Surface*	pSurfLeft;
} Pferd;

typedef struct
{
	int	 	xpos;
	int		ypos;
	SDL_Surface* 	pSurf;
} Food;

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

void dumpSurface( SDL_Surface* pSurf )
{
	printf("		format->Amask:	 0x%X\n", pSurf->format->Amask);
	printf("		format->Ashift:	 0x%X\n", pSurf->format->Ashift);
}

Food* dropFood( int xpos, int ypos )
{
	int colorkey;
	char* bmpFilename = "carrot.bmp";

	Food* pFood = malloc( sizeof(Food) );

	pFood->xpos = xpos;
	pFood->ypos = ypos;

	pFood->pSurf = SDL_DisplayFormatAlpha( SDL_LoadBMP( bmpFilename ) );

	colorkey = SDL_MapRGB( pFood->pSurf->format, 0xFF, 0x00, 0x00 );
	SDL_SetColorKey( pFood->pSurf, SDL_RLEACCEL | SDL_SRCCOLORKEY, colorkey );

	pFood->pSurf->format->Amask  = 0; 

	return pFood;
}

void eatFood( Food* pFood )
{
	free( pFood );
}

Pferd* createPferd( int xpos, int ypos, char* bmpFilename )
{
	int colorkey;

	Pferd* pNewPferd = malloc( sizeof(Pferd) );

	pNewPferd->xpos   = xpos;
	pNewPferd->ypos   = ypos;
	pNewPferd->dist   = 0;
	pNewPferd->direct = left;

	pNewPferd->pSurfRight = SDL_DisplayFormatAlpha( SDL_LoadBMP ( bmpFilename ) );

	colorkey = SDL_MapRGB( pNewPferd->pSurfRight->format, 0xFF, 0x00, 0x00 );
	SDL_SetColorKey( pNewPferd->pSurfRight, SDL_RLEACCEL | SDL_SRCCOLORKEY, colorkey );

	pNewPferd->pSurfLeft = flip_surface( pNewPferd->pSurfRight, FLIP_HORIZONTAL|SDL_SRCCOLORKEY );

	pNewPferd->pSurfRight->format->Amask  = pNewPferd->pSurfLeft->format->Amask; 
	//pNewPferd->pSurfRight->format->Ashift = pNewPferd->pSurfLeft->format->Ashift; 

#ifdef DEBUG
	printf("SurfRight: \n");
	dumpSurface( pNewPferd->pSurfRight );

	printf("\nSurfLeft: \n");
	dumpSurface( pNewPferd->pSurfLeft );
#endif

	return pNewPferd;
}

void destroyPferd( Pferd* pPferd )
{
	free( pPferd );
}

void goPferd( Pferd* pPferd, Direction direct, int dist )
{
#ifdef DEBUG
	printf("	goPferd( 0x%X, %i, %i )\n", (int) pPferd, direct, dist);
#endif

	pPferd->dist   = dist;
	pPferd->direct = direct;
}

void movePferd( Pferd* pPferd )
{
#ifdef DEBUG
	printf("	movePferd( 0x%X )\n", (int) pPferd);
	printf("		pPferd->dist: %i\n", pPferd->dist);
#endif

	if ( pPferd->dist > 0 )
	{
		switch (pPferd->direct) 
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
		pPferd->dist -= 20;

	}

}

void wanderPferd( Pferd* pPferd )
{
	int distance;

	distance = ( rand() % 500 );
#ifdef DEBUG
	printf("	wanderPferd( 0x%X )\n", (int) pPferd);
	printf("		distance: %i\n", distance);
#endif
	if ( ((pPferd->xpos + distance + pPferd->pSurfRight->w) < m_screen->w ) &&
			((pPferd->ypos + distance + pPferd->pSurfRight->h) < m_screen->h ) )
	{
		goPferd( pPferd, downright, distance );
	}
	else if( ((pPferd->xpos + distance + pPferd->pSurfRight->w) < m_screen->w ) &&
			((pPferd->ypos - distance ) > 0 ) )
	{
		goPferd( pPferd, upright, distance );
	}
	else if( ((pPferd->xpos - distance) > 0 ) &&
			((pPferd->ypos + distance + pPferd->pSurfRight->h) < m_screen->h ) )
	{
		goPferd( pPferd, downleft, distance );
	}
	else if( ((pPferd->xpos - distance) > 0 ) &&
			((pPferd->ypos - distance ) > 0 ) )
	{
		goPferd( pPferd, upleft, distance );
	}
	else if ( (pPferd->xpos + distance + pPferd->pSurfRight->w) < m_screen->w )
	{
		goPferd( pPferd, right, distance );
	}  
	else if ( (pPferd->xpos - distance) > 0 )
	{
		goPferd( pPferd, left, distance );
	}
}	

void blitPferd( Pferd* pPferd ) 
{
	SDL_Rect pferdPos;

	pferdPos.x = pPferd->xpos;
	pferdPos.y = pPferd->ypos;

	switch (pPferd->direct)
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

}

void blitFood( Food* pFood )
{
	SDL_Rect foodPos;

	foodPos.x = pFood->xpos;
	foodPos.y = pFood->ypos;

	SDL_BlitSurface( pFood->pSurf, NULL, m_screen, &foodPos );
}

void getFood( Pferd* pPferd, Food* pFood )
{
	Direction direct;
	int 	  dist;

#ifdef DEBUG
	printf("	getFood( 0x%X, 0x%X )\n", (int) pPferd, (int) pFood);
#endif

	if ( pPferd->ypos > pFood->ypos )
	{
		if ( pPferd->xpos > pFood->xpos )
			direct = upleft;
		else
			direct = upright;

		dist   = pPferd->ypos - pFood->ypos;
	}
	else if ( pPferd->ypos < pFood->ypos )
	{
		if ( pPferd->xpos > pFood->xpos )
			direct = downleft;
		else
			direct = downright;

		dist   = pFood->ypos - pPferd->ypos;
	}
	else if ( pPferd->xpos > pFood->xpos )
	{
		direct = left;
		dist   = pPferd->xpos - pFood->xpos;
	}
	else if (pPferd->xpos < pFood->xpos )
	{
		direct = right;
		dist   = pFood->xpos - pPferd->xpos;
	}


	goPferd( pPferd, direct, dist );
}

int tryEat( Pferd* pPferd, Food* pFood )
{
	if ( (pPferd->xpos == pFood->xpos) && (pPferd->ypos == pFood->ypos) )
		return 1;
	else
		return 0;
}

int main( int argc, char* args[] ) 
{ 
	int 	  fExit 	 = 0;
	int	  fFoodAvailable = 0;
	int	  xfood, yfood;

	SDL_Event event;
	SDL_Rect  zeroRect;

	Pferd* 	  pSPferd; 
	Pferd*	  pLea;
	Food* 	  pFood;

	zeroRect.x = 0;
	zeroRect.y = 0;

	//Start SDL 
	SDL_Init( SDL_INIT_EVERYTHING ); 

	//atexit(SDL_Quit);

	SDL_WM_SetCaption("Snuddlehorse", "Snuddlehorse");

	m_screen = SDL_SetVideoMode( 800, 600, 32, SDL_SWSURFACE );
	m_background = SDL_LoadBMP( "background.bmp" );

	pLea	= createPferd( 600, 100, "lea.bmp" );
	pSPferd = createPferd( 400, 300, "pferd.bmp" );

	while ( !fExit )
	{
		if ( pSPferd->dist <= 0 )
		{
			if ( fFoodAvailable )
				getFood( pSPferd, pFood );
			else
				wanderPferd( pSPferd );
		}
		if ( pLea->dist <= 0 )
		{
			if ( fFoodAvailable )
				getFood( pLea, pFood );
			else
				wanderPferd( pLea );
		}

		if ( fFoodAvailable )
		{
			if ( tryEat( pSPferd, pFood ) || tryEat( pLea, pFood ) )
			{
				eatFood( pFood );
				fFoodAvailable = 0;
			}
		}

		// SDL_FillRect( m_screen, &m_screen->clip_rect, MEADOW_COLOR );
		SDL_BlitSurface( m_background, NULL, m_screen, &zeroRect );

		if ( fFoodAvailable )
			blitFood( pFood );

		movePferd( pLea );
		movePferd( pSPferd );

		blitPferd( pLea );
		blitPferd( pSPferd );

		SDL_Flip( m_screen );

		while(SDL_PollEvent(&event)) 
		{
			switch (event.type)
			{
				case SDL_MOUSEBUTTONDOWN:
					xfood = event.button.x - (event.button.x % 20);
					yfood = event.button.y - (event.button.y % 20);
#ifdef DEBUG
					printf("placing food at (%d,%d)\n", xfood, yfood);
#endif
					if ( !fFoodAvailable ) 
					{
						pFood = dropFood( xfood, yfood );
						fFoodAvailable = 1;
					}
					break;
				case SDL_KEYDOWN:
					if ( event.key.keysym.sym == SDLK_ESCAPE )
					{
						fExit = 1;
					}
					break;
				case SDL_QUIT:
					fExit = 1;
					break;
			}
		}

		SDL_Delay( 200 );
	}

	printf("quit\n");

	SDL_Quit(); 

	return 0; 
}

