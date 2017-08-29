#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

int size = 80;
int sw = 80*8, sh = 80*8;
int flip = 0;
int castle[2] = {3,3};

int board[8][8];
SDL_Surface *piece[12];

void drawrect(SDL_Surface *surface, int x, int y, int w, int h, Uint32 c)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = 3;
	SDL_FillRect(surface, &rect, c);

	rect.y = y+h-3;
	SDL_FillRect(surface, &rect, c);

	rect.y = y;
	rect.w = 3;
	rect.h = h;
	SDL_FillRect(surface, &rect, c);

	rect.x = x+w-3;
	SDL_FillRect(surface, &rect, c);
}

void drawboard(SDL_Surface *surface)
{
	int x,y,xx,yy;
	Uint32 dark = SDL_MapRGB(surface->format,181,136,99);
	Uint32 light = SDL_MapRGB(surface->format,240,217,181);

	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = sw;
	rect.h = sh;
	SDL_FillRect(surface, &rect, light);

	rect.w = size;
	rect.h = size;

	for (y=0; y<8; y++){
	for (x=y&1^1; x<8; x+=2){
		rect.x = x*size;
		rect.y = y*size;
		SDL_FillRect(surface, &rect, dark);
	}
	}

	for (y=0; y<8; y++) {
	for (x=0; x<8; x++) {
		if (flip) {
			xx=7-x;
			yy=y;
		}else{
			xx=x;
			yy=7-y;
		}
		if (board[x][y]) {
			rect.x = xx*size;
			rect.y = yy*size;
			SDL_BlitSurface(piece[board[x][y]-1],NULL,surface,&rect);
		}
	}
	}
}

int ep=0, epx, epy;
int valid(int x0, int y0, int x1, int y1)
{
	int c0, c1;
	int piece;
	int v;

	if (x0==x1 && y0==y1) return 0;

	c0 = board[x0][y0]>6;
	c1 = board[x1][y1]>6;

	if (board[x1][y1] && c0==c1) return 0;

	v = 0;

	int cx, cy, cx0, cy0, cap;
	int x,y;

	if (c0==0) piece = board[x0][y0]; else piece = board[x0][y0]-6;
	switch (piece) {
	//pawn
	case 1:
		if (c0==0 && y1>y0 || c0==1 && y1<y0) {
			if (x0==x1){
				//pawn double move
				if (abs(y1-y0)==2 && board[x1][y1]==0) {
					if (c0==0 && y0==1 && board[x0][y0+1]==0){
						epx = x0;
						epy = y0-1;
						v=3;
					}else if (c0==1 && y0==6 && board[x0][y0-1]==0) {
						epx = x0;
						epy = y0+1;
						v=3;
					}
				}else if (abs(y1-y0)==1) {
					if (board[x1][y1]==0) v=1;
				}
			}else if (abs(x1-x0)==1 && abs(y1-y0)==1){
				if (board[x1][y1]!=0) {
					v=1;
				}
				if (ep) {
					if (x1==epx && y1==epy){
						v=4; //pissant capture
						ep = 0;
					}
				}
			}
		}

		break;
	//bishop
	case 2:
		if (abs(x1-x0)==abs(y1-y0)) {
			v = 1;
			if (x1>x0) cx = 1; else cx = -1;
			if (y1>y0) cy = 1; else cy = -1;
			cx0 = x0;
			cy0 = y0;
			cap = 0;
			while (cx0!=x1 && cy0!=y1){
				cx0 += cx;
				cy0 += cy;
				if(cap) {
					v = 0;
					break;
				}
				if (board[cx0][cy0]) {
					if ((board[cx0][cy0]>6)==c0){
						v = 0;
						break;
					}else{
						cap = 1;
					}
				}
			}
		}
		break;
	//rook
	case 4:
		if (x0==x1 || y0==y1){
			v = 1;
			if (x0==x1) {
				cx = 0;
				if (y1>y0) cy = 1; else cy = -1;
			}else{
				if (x1>x0) cx = 1; else cx = -1;
				cy = 0;
			}
			cx0 = x0;
			cy0 = y0;
			cap = 0;
			while (cx0!=x1 || cy0!=y1){
				cx0 += cx;
				cy0 += cy;
				if(cap) {
					v = 0;
					break;
				}
				if (board[cx0][cy0]) {
					if ((board[cx0][cy0]>6)==c0){
						v = 0;
						break;
					}else{
						cap = 1;
					}
				}
			}
		}
		break;
	//queen
	case 5:
		if (abs(x1-x0)==abs(y1-y0)) {
			v = 1;
			if (x1>x0) cx = 1; else cx = -1;
			if (y1>y0) cy = 1; else cy = -1;
			cx0 = x0;
			cy0 = y0;
			cap = 0;
			while (cx0!=x1 && cy0!=y1){
				cx0 += cx;
				cy0 += cy;
				if(cap) {
					v = 0;
					break;
				}
				if (board[cx0][cy0]) {
					if ((board[cx0][cy0]>6)==c0){
						v = 0;
						break;
					}else{
						cap = 1;
					}
				}
			}
		}else if (x0==x1 || y0==y1) {
			v = 1;
			if (x0==x1) {
				cx = 0;
				if (y1>y0) cy = 1; else cy = -1;
			}else{
				if (x1>x0) cx = 1; else cx = -1;
				cy = 0;
			}
			cx0 = x0;
			cy0 = y0;
			cap = 0;
			while (cx0!=x1 || cy0!=y1){
				cx0 += cx;
				cy0 += cy;
				if(cap) {
					v = 0;
					break;
				}
				if (board[cx0][cy0]) {
					if ((board[cx0][cy0]>6)==c0){
						v = 0;
						break;
					}else{
						cap = 1;
					}
				}
			}
		}
		break;
	//king
	case 6:
		if (abs(x1-x0)<=1 && abs(y1-y0)<=1) {
			v = 1;
		}else if (castle[c0]){ //castle
			if (c0==0 && y1==0) { //white
				if (x1==6 && castle[c0]&1) { //king side
					if (board[5][y1]==0 && board[6][y1]==0 && (castle[c0]&1)) {
						v=10;
						for (y=0;y<8;y++) for (x=0;x<8;x++)
							if (board[x][y] && (board[x][y]>6)!=c0 && valid(x,y,5,y1)) v=0;
					}
				}else if (x1==2 && castle[c0]&2) { //queen side
					if (board[1][y1]==0 && board[2][y1]==0 &&board[3][y1]==0 && (castle[c0]&2)) {
						v=11;
						for (y=0;y<8;y++) for (x=0;x<8;x++)
							if (board[x][y] && (board[x][y]>6)!=c0 && (valid(x,y,1,y1) || valid(x,y,3,y1)) ) v=0;
					}
				}
			}else if (c0==1 && y1==7) { //black
				if (x1==6 && castle[c0]&1) { //king side
					if (board[5][y1]==0 && board[6][y1]==0 && (castle[c0]&1)) {
						v=12;
						for (y=0;y<8;y++) for (x=0;x<8;x++)
							if (board[x][y] && (board[x][y]>6)!=c0 && valid(x,y,5,y1)) v=0;
					}
				}else if (x1==2 && castle[c0]&2) { //queen side
					if (board[1][y1]==0 && board[2][y1]==0 &&board[3][y1]==0 && (castle[c0]&2)) {
						v=13;
						for (y=0;y<8;y++) for (x=0;x<8;x++)
							if (board[x][y] && (board[x][y]>6)!=c0 && (valid(x,y,1,y1) || valid(x,y,3,y1)) ) v=0;
					}
				}
			}
		}
		break;
	//knight
	case 3:
		if (abs(x1-x0)==2 && abs(y1-y0)==1) v = 1;
		else if (abs(x1-x0)==1 && abs(y1-y0)==2) v = 1;
		break;
	default:
		v = 1;
	}

	//does move expose check?
	int kx,ky;
	int p;
	if (v){
		//do move
		p = board[x1][y1];
		board[x1][y1] = board[x0][y0];
		board[x0][y0] = 0;

		//find king
		for (y=0;y<8;y++) {
		for (x=0;x<8;x++) {
			if (c0==0&&board[x][y]==6 || c0==1&&board[x][y]==12) {
				kx = x;
				ky = y;
				break;
			}
		}
		}

		//find captors
		for (y=0;y<8;y++) {
		for (x=0;x<8;x++) {
			if (board[x][y] && (board[x][y]>6)!=c0 &&
				valid(x,y,kx,ky)==2) {
				v = 0;
				break;
			}
		}
		}

		//undo move
		board[x0][y0] = board[x1][y1];
		board[x1][y1] = p;
	}

	if (v==1 && board[x1][y1]) v=2;

	return v;
}

void drawcircf(SDL_Surface *surface, int x, int y, int r, Uint32 c)
{
	int x0=r,y0=0,e=0;

	SDL_Rect rect;
	rect.h = 1;

	while (y0<x0) {
		rect.x = x-y0;
		rect.y = y-x0;
		rect.w = y0*2;
		SDL_FillRect(surface, &rect, c);

		//rect.x = x-y0;
		rect.y = y+x0;
		rect.w = y0*2;
		SDL_FillRect(surface, &rect, c);

		rect.x = x-x0;
		rect.y = y-y0;
		rect.w = x0*2;
		SDL_FillRect(surface, &rect, c);

		//rect.x = x-x0;
		rect.y = y+y0;
		rect.w = x0*2;
		SDL_FillRect(surface, &rect, c);

		if (e<=0){
			y0++;
			e+=y0*2;
		}else{
			x0--;
			e-=x0*2;
		}
	}
}

void highlight(SDL_Surface *surface, int x0, int y0)
{
	int x,y,xx,yy;
	int v;
	Uint32 c;

	SDL_Rect rect;
	rect.w = 10;
	rect.h = 10;

	for (y=0;y<8;y++) {
	for (x=0;x<8;x++) {
		v = valid(x0,y0,x,y);
		if (v) {
			if (flip) {
				xx=7-x;
				yy=y;
			}else{
				xx=x;
				yy=7-y;
			}
			//rect.x = (xx+0.5)*size - 5;
			//rect.y = (yy+0.5)*size - 5;
			//SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format,85,126,56));

			if(v==2 || v==4) c = SDL_MapRGB(surface->format,180,0,0);
			else c = SDL_MapRGB(surface->format,85,126,56);
			drawcircf(surface, (xx+0.5)*size, (yy+0.5)*size, 10, c);
		}
	}
	}
}

void start()
{
	int x,y;

	for (y=0;y<8;y++) for (x=0;x<8;x++) board[x][y]=0;
	board[0][0]=4; //a1
	board[1][0]=3; //b1
	board[2][0]=2;
	board[3][0]=5;
	board[4][0]=6;
	board[5][0]=2;
	board[6][0]=3;
	board[7][0]=4;
	board[0][7]=4+6;
	board[1][7]=3+6;
	board[2][7]=2+6;
	board[3][7]=5+6;
	board[4][7]=6+6;
	board[5][7]=2+6;
	board[6][7]=3+6;
	board[7][7]=4+6;
	for (x=0;x<8;x++) {
		board[x][1] = 1;
		board[x][6] = 1+6;
	}
}

int main(int argc, char *argv[])
{
	SDL_Window *window;
	SDL_Surface *surface, *surface2;
	SDL_Event event;

	int end = 0, key, redraw = 1;

	SDL_Init(SDL_INIT_EVERYTHING);

	piece[ 0] = IMG_Load("pieces/merida/80/WhitePawn.png");
	piece[ 1] = IMG_Load("pieces/merida/80/WhiteBishop.png");
	piece[ 2] = IMG_Load("pieces/merida/80/WhiteKnight.png");
	piece[ 3] = IMG_Load("pieces/merida/80/WhiteRook.png");
	piece[ 4] = IMG_Load("pieces/merida/80/WhiteQueen.png");
	piece[ 5] = IMG_Load("pieces/merida/80/WhiteKing.png");
	piece[ 6] = IMG_Load("pieces/merida/80/BlackPawn.png");
	piece[ 7] = IMG_Load("pieces/merida/80/BlackBishop.png");
	piece[ 8] = IMG_Load("pieces/merida/80/BlackKnight.png");
	piece[ 9] = IMG_Load("pieces/merida/80/BlackRook.png");
	piece[10] = IMG_Load("pieces/merida/80/BlackQueen.png");
	piece[11] = IMG_Load("pieces/merida/80/BlackKing.png");

	int x,y,i,v,xx,yy;

	start();

	window = SDL_CreateWindow("",
			SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,sw,sh,
			SDL_WINDOW_RESIZABLE);

	surface = SDL_GetWindowSurface(window);
	surface2 = SDL_CreateRGBSurface(0, sw, sh, 32, 0, 0, 0, 0);

	drawboard (surface2);
	SDL_BlitSurface (surface2, 0, surface, 0);
	SDL_UpdateWindowSurface(window);

	int sel = 0, selx, sely;
	int player = 0;
	int freemove = 0;
	int epp=0;

	while(!end) {
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_QUIT:
				end = 1;
				break;
			case SDL_WINDOWEVENT:
				SDL_UpdateWindowSurface(window);
				break;

			case SDL_KEYDOWN:
				key = event.key.keysym.sym;
				if(key == 27) {
					end = 1;
				}else if(key == 'f') {
					flip = !flip;
					sel = 0;
					drawboard (surface2);
					SDL_BlitSurface (surface2, 0, surface, 0);
					SDL_UpdateWindowSurface(window);
				}else if(key == 'n') {
					sel = 0;
					player = 0;
					start();
					drawboard (surface2);
					SDL_BlitSurface (surface2, 0, surface, 0);
					SDL_UpdateWindowSurface(window);
				}else if(key == 'm') {
					freemove ^= 1;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				//printf("%d\n", event.button.button);
				if (flip) {
					x = 7-(event.button.x/size);
					y = (event.button.y/size);
				} else {
					x = (event.button.x/size);
					y = 7-(event.button.y/size);
				}
				//printf("%d,%d\n",x,y);

				if (sel) {
					printf("%c%d\n", 'a'+x, y);

					sel = 0;

					v = valid(selx,sely,x,y);
					if (v) {
						player ^= 1;

						//disable castling
						if (board[selx][sely]==6) castle[0]=0; 
						if (board[selx][sely]==12) castle[1]=0; 
						if (castle[0]&1 && selx==7 && sely==0) castle[0] ^= 1;
						if (castle[0]&2 && selx==0 && sely==0) castle[0] ^= 2;
						if (castle[1]&1 && selx==7 && sely==7) castle[1] ^= 1;
						if (castle[1]&2 && selx==0 && sely==7) castle[1] ^= 2;

						//castle
						if (v==10) {
							board[7][0]=0;
							board[5][0]=4;
						}else if (v==11){
							board[0][0]=0;
							board[3][0]=4;
						}else if (v==12){
							board[7][7]=0;
							board[5][7]=4+6;
						}else if (v==13){
							board[0][7]=0;
							board[3][7]=4+6;
						}

						if (v==3) {
							ep=1;
							epx=x;
							if(player==0) epy = y+1; else epy = y-1;
						}

						if (board[selx][sely]==1&&y==7 || board[selx][sely]==7&&y==0){
							board[x][y] = board[selx][sely]+4;
						}else{
							if (v==4) {
								if (player==0) board[x][y+1]=0; else board[x][y-1]=0;
							}
							board[x][y] = board[selx][sely];
						}

						board[selx][sely] = 0;

						drawboard (surface2);

					}

					SDL_BlitSurface (surface2, 0, surface, 0);
					SDL_UpdateWindowSurface(window);
				}else if (board[x][y] && player==(board[x][y]>6)) {
					selx = x;
					sely = y;
					//printf("%d,%d\n",x,y);

					sel = 1;
					drawrect(surface, (event.button.x/size)*size,
							(event.button.y/size)*size, size, size,
							SDL_MapRGB(surface->format,85,126,56));
					int epp=ep,eppx=epx,eppy=epy;
					highlight(surface, selx, sely);
					ep = epp;
					epx=eppx;
					epy=eppy;
					SDL_UpdateWindowSurface(window);
				}
				break;
			case SDL_MOUSEWHEEL:
				//printf("%d\n", event.wheel.y);
				break;
			}
			SDL_FlushEvent(event.type);
		}
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

