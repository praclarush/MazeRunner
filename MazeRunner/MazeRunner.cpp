/* ---------------------------------------------------------------------------
**
** File: main.cpp
** Description: Direct3D project Chapters 3 & 4
**
** Author: Nathan Bremmer
** -------------------------------------------------------------------------*/

#include "DirectX.h"
#include <sstream>
#include <fstream>
#include <vector>

using namespace std;

//screen settings
const string APPTITLE = "Maze Runner";
const int SCREENW = 1024;
const int SCREENH = 768;

//settings for the scroller
const int TILEWIDTH = 64;
const int TILEHEIGHT = 64;
const int MAPWIDTH = 20;
const int MAPHEIGHT = 20;

//scrolling window size
const int BUFFERW = SCREENW * 2;
const int BUFFERH = SCREENH * 2;
const int WINDOWWIDTH = (SCREENW / TILEWIDTH) * TILEWIDTH;
const int WINDOWHEIGHT = (SCREENH / TILEHEIGHT) * TILEHEIGHT;

//Note(Nathan): while not a const, this array needs to be accessible to everything so it goes at the top
int MAPDATA[MAPWIDTH*MAPHEIGHT] = {
	1, 2, 32, 2, 2, 2, 2, 2, 2, 32, 2, 2, 2, 32, 2, 2, 2, 32, 2, 3,
	11, 139, 32, 72, 72, 72, 118, 72, 72, 32, 72, 118, 72, 32, 72, 118, 72, 32, 72, 13,
	11, 72, 32, 72, 32, 32, 72, 32, 32, 32, 72, 22, 72, 32, 72, 32, 32, 32, 72, 13,
	11, 72, 72, 72, 118, 32, 72, 72, 72, 72, 72, 32, 72, 32, 72, 72, 72, 72, 118, 13,
	32, 32, 32, 72, 32, 32, 32, 32, 32, 32, 32, 32, 72, 72, 72, 32, 32, 32, 72, 13,
	11, 72, 32, 72, 72, 72, 72, 72, 118, 72, 72, 72, 72, 32, 72, 32, 118, 32, 72, 13,
	11, 118, 32, 72, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 72, 32, 72, 32, 72, 13,
	11, 72, 32, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 118, 72, 32, 72, 32, 72, 13,
	11, 72, 32, 32, 32, 32, 72, 32, 32, 32, 72, 32, 32, 32, 32, 32, 72, 32, 32, 32,
	11, 118, 72, 72, 72, 72, 72, 72, 118, 32, 72, 72, 72, 72, 72, 72, 72, 72, 72, 13,
	32, 32, 32, 32, 32, 72, 32, 32, 72, 32, 32, 32, 32, 72, 72, 72, 32, 72, 118, 44,
	11, 72, 32, 118, 72, 72, 72, 32, 72, 32, 72, 118, 32, 32, 72, 32, 32, 32, 32, 32,
	11, 72, 32, 72, 32, 32, 72, 32, 72, 72, 72, 32, 32, 72, 118, 72, 32, 72, 118, 44,
	11, 72, 72, 72, 32, 72, 72, 32, 72, 32, 72, 72, 32, 72, 32, 72, 72, 72, 72, 13,
	11, 72, 32, 72, 32, 118, 72, 32, 72, 32, 72, 118, 72, 72, 32, 72, 72, 32, 72, 13,
	11, 72, 32, 32, 32, 72, 72, 72, 72, 32, 32, 32, 32, 32, 32, 32, 72, 32, 72, 13,
	11, 72, 32, 72, 32, 32, 32, 32, 72, 32, 72, 72, 72, 72, 72, 72, 72, 32, 72, 13,
	32, 32, 32, 72, 32, 72, 72, 72, 118, 72, 72, 72, 72, 32, 32, 32, 32, 32, 118, 13,
	11, 72, 118, 72, 72, 72, 44, 118, 44, 72, 32, 118, 72, 72, 72, 118, 72, 72, 72, 13,
	21, 22, 22, 22, 44, 22, 32, 22, 32, 22, 32, 22, 22, 22, 22, 22, 22, 22, 22, 23 };

enum GAME_STATES
{
	StartScreen = 0,
	MainMenu = 1,
	OptionMenu = 2,
	ScoreBoard = 3,
	Credits = 4,
	Running = 5,
	Paused = 6,
	Exited = 7,
	Dead = 8,
	Win = 9

};

struct GAME_OPTIONS
{
	int MusicVolume;
	int sfxVolume;
	string characterImage;
	string enemyImage;
	string menuPointerImage;
	string menuBackgroundImage;
	string tileSet;
	string menuMusic;
	string mainFont;
	string menuSelectSound;
	string backgroundMusic;
	string walkingSound;

	GAME_OPTIONS()
	{
		//TODO(Nathan): below 50 the sound becomes to quite; Need to figure out a way to fix this
		MusicVolume = 75;
		sfxVolume = 80;
		characterImage = "images/player.png";
		enemyImage = "images/devil.png";
		menuPointerImage = "images/pointer.png";
		menuBackgroundImage = "images/MazeTitle.bmp";
		tileSet = "map/MazeMap.BMP";
		menuMusic = "sound/ThemeMenu.wav";
		menuSelectSound = "sound/MenuSelect.wav";
		backgroundMusic = "sound/ambient_menu.wav";
		walkingSound = "sound/Footstep_Dirt_01.wav";
		mainFont = "Arial";
	}
};

struct Credit
{
	string line;
	int x, y;

	Credit()
	{
		line = "";
		x = 0;
		y = 0;
	}
};

struct Character
{
	int spriteX;
	int spriteY;
	int startX;
	int startY;
	int startTileIndex;
	LPDIRECT3DTEXTURE9 sprite;

	Character()
	{
		spriteX = 0;
		spriteY = 0;
		startX = 0;
		startY = 0;
		startTileIndex = 0;
		sprite = NULL;
	}

	int getTile(int x, int y, int scrollx, int scrolly)
	{
		int tilex, tiley;

		tilex = (spriteX + x + scrollx) / TILEWIDTH;
		tiley = (spriteY + y + scrolly) / TILEHEIGHT;
		int tileIndex = MAPDATA[((tiley)* MAPWIDTH + (tilex))];
		return tileIndex;
	}
};

struct MENU_POINTER_LOCATION
{
	int x, y;
};



LPDIRECT3DSURFACE9 background = NULL;
LPDIRECT3DSURFACE9 screenBackground = NULL;
LPDIRECT3DSURFACE9 tiles = NULL;

LPDIRECT3DTEXTURE9 menuPointer = NULL;
LPDIRECT3DSURFACE9 scrollbuffer = NULL;
LPD3DXFONT fontArial24 = NULL;
LPD3DXFONT smallArial = NULL;



int ScrollX, ScrollY;
int SpeedX, SpeedY;
long start = 0;
long fps = 0;
int timer = 0;
int frame = 8;
int starttime = 0;

//this stuff was taken from chapter 14, as my sprite kept running off the screen and was really fast.
DWORD refresh = 0;
DWORD coretime = 0;
double corefps = 0.0;
double corecount = 0.0;
DWORD currenttime;

GAME_STATES game_state = StartScreen;
GAME_STATES prev_game_state = Exited;

MENU_POINTER_LOCATION menuPointerLocation;
GAME_OPTIONS options;

CSound *menuMusic = NULL;
CSound *menuSelect = NULL;
CSound *backgroundMusic = NULL;
CSound *walkingSound = NULL;

Character pc;
Character npc;

vector<Credit> CreditText;

const int STARTTILE = 139;
const int GOALTILE = 140;

bool isSolid(int tile)
{
	//Note(Nathan): for easy debug info
	bool ret = (tile != 72 && tile != 118 && tile != 139 && tile != 0);
	return ret;
}

bool isGoal(int tile)
{
	return (tile == 140);
}


//This was to be used with the mouse, but seeing as the mouse doesn't seem to want to work as the internet and 
//books say then I will for go mouse support past the first screen which will only require a single mouse click
//bool MouseClickInsideRect(RECT rect, POINT point)
//{
//	return PtInRect(&rect, point);
//}

//This function updates the scrolling position and speed
void UpdateScrollPosition()
{
	const int GAMEWORLDWIDTH = TILEWIDTH * MAPWIDTH;
	const int GAMEWORLDHEIGHT = TILEHEIGHT * MAPHEIGHT;

	//update horizontal scrolling position and speed
	ScrollX += SpeedX;

	if (ScrollX < 0)
	{
		ScrollX = 0;
		SpeedX = 0;
	}
	else if (ScrollX > GAMEWORLDWIDTH - WINDOWWIDTH)
	{
		ScrollX = GAMEWORLDWIDTH - WINDOWWIDTH;
		SpeedX = 0;
	}

	//update vertical scrolling position and speed
	ScrollY += SpeedY;
	if (ScrollY < 0)
	{
		ScrollY = 0;
		SpeedY = 0;
	}
	else if (ScrollY > GAMEWORLDHEIGHT - WINDOWHEIGHT)
	{
		ScrollY = GAMEWORLDHEIGHT - WINDOWHEIGHT;
		SpeedY = 0;
	}

}

void DrawTile(LPDIRECT3DSURFACE9 source, int tileIndex, int width, int height, int columns, LPDIRECT3DSURFACE9 dest, int destx, int desty)
{
	int tilenum = MAPDATA[tileIndex];

	//Set PC and NPC home tiles 
	//if (tileIndex == pc.startTileIndex)
	//{
	//	pc.startX = destx;
	//	pc.startY = desty;
	//}

	//if (tileIndex == npc.startTileIndex)
	//{
	//	npc.startX = destx;
	//	npc.startY = desty;
	//}

	//create a RECT to describe the source image
	RECT r1;
	r1.left = (tilenum % columns) * width;
	r1.top = (tilenum / columns) * height;
	r1.right = r1.left + width;
	r1.bottom = r1.top + height;

	//set destination rect
	RECT r2 = { destx, desty, destx + width, desty + height };

	//draw the tile 
	d3ddev->StretchRect(source, &r1, dest, &r2, D3DTEXF_NONE);
}

//This function fills the tilebuffer with tiles representing
//the current scroll display based on scrollx/scrolly.
void DrawTiles()
{
	int tilex, tiley;
	int columns, rows;
	int x, y;
	int tileIndex;

	//calculate starting tile position
	tilex = ScrollX / TILEWIDTH;
	tiley = ScrollY / TILEHEIGHT;

	//calculate the number of columns and rows
	columns = WINDOWWIDTH / TILEWIDTH;
	rows = WINDOWHEIGHT / TILEHEIGHT;

	//draw tiles onto the scroll buffer surface
	for (y = 0; y <= rows; y++)
	{
		for (x = 0; x <= columns; x++)
		{
			//retrieve the tile number from this position
			tileIndex = ((tiley + y) * MAPWIDTH + (tilex + x));

			//draw the tile onto the scroll buffer
			DrawTile(tiles, tileIndex, TILEWIDTH, TILEHEIGHT, 16, scrollbuffer, x*TILEWIDTH, y*TILEHEIGHT);
		}
	}
}

//This function draws the portion of the scroll buffer onto the back buffer
//according to the current "partial tile" scroll position.
void DrawScrollWindow(bool scaled = false)
{
	//calculate the partial sub-tile lines to draw using modulus
	int partialx = ScrollX % TILEWIDTH;
	int partialy = ScrollY % TILEHEIGHT;

	//set dimensions of the source image as a rectangle
	RECT r1 = { partialx, partialy, partialx + WINDOWWIDTH - 1, partialy + WINDOWHEIGHT - 1 };

	//set the destination rectangle
	RECT r2;
	if (scaled) {
		//use this line for scaled display
		RECT r = { 0, 0, WINDOWWIDTH - 1, WINDOWHEIGHT - 1 };
		r2 = r;
	}
	else {
		//use this line for non-scaled display
		RECT r = { 0, 0, SCREENW - 1, SCREENH - 1 };
		r2 = r;
	}

	//draw the "partial tile" scroll window onto the back buffer
	d3ddev->StretchRect(scrollbuffer, &r1, backbuffer, &r2, D3DTEXF_NONE);
}

//Will pick a random index from the map array that is not a wall or sprite location
int GenerateRandomNumber(int spriteLocation)
{
	//TODO(Nathan) pick better variable names
	int validTileA = 72, validTileB = 118;
	int position = 0;
	int tileCount = MAPWIDTH*MAPHEIGHT;
	srand(GetTickCount());

	bool found = false;

	do {
		int testPosition = rand() % tileCount;
		int tileData = MAPDATA[testPosition];

		if (tileData == validTileA || tileData == validTileB)
		{
			if (tileData != spriteLocation)
			{
				position = testPosition;
				found = true;
			}
		}
	} while (!found);

	return position;
}

bool Menu_Init(HWND window)
{
	//load menu background
	LPDIRECT3DSURFACE9 menuBackground = NULL;
	menuBackground = LoadSurface(options.menuBackgroundImage);
	if (!menuBackground) return false;

	HRESULT result =
		d3ddev->CreateOffscreenPlainSurface(
		SCREENW,
		SCREENH,
		D3DFMT_X8R8G8B8,
		D3DPOOL_DEFAULT,
		&screenBackground,
		NULL);

	if (result != D3D_OK) return false;

	d3ddev->StretchRect(menuBackground, NULL, screenBackground, NULL, D3DTEXF_NONE);

	menuBackground->Release();

	menuPointer = LoadTexture(options.menuPointerImage);

	if (!menuPointer)
	{
		MessageBox(window, "Error loading menu pointer", "Error", 0);
		return false;
	}

	return true;
}

bool Map_init(HWND window)
{
	//load the tile images
	tiles = LoadSurface(options.tileSet);

	//MAPDATA = MAPDATA_ORIGINAL;

	if (!tiles) return false;

	ScrollX = 0;
	ScrollY = 0;

	//create the scroll buffer surface in memory, slightly bigger than the screen
	const int SCROLLBUFFERWIDTH = SCREENW + TILEWIDTH * 2;
	const int SCROLLBUFFERHEIGHT = SCREENH + TILEHEIGHT * 2;

	HRESULT result = d3ddev->CreateOffscreenPlainSurface(SCROLLBUFFERWIDTH, SCROLLBUFFERHEIGHT, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &scrollbuffer, NULL);

	//Set goal
	int goalTileLocation = GenerateRandomNumber(0);
	MAPDATA[goalTileLocation] = GOALTILE;

	//Set start
	/*int StartTileLocation = GenerateRandomNumber(0);
	pc.startTileIndex = StartTileLocation;
	MAPDATA[goalTileLocation] = STARTTILE;*/

	//set Enemy Location
	int StartEnemyLocation = GenerateRandomNumber(0);
	npc.startTileIndex = StartEnemyLocation;


	if (result != S_OK)return false;

	pc.sprite = LoadTexture(options.characterImage);
	//npc.sprite = LoadTexture(options.enemyImage);

	if (!pc.sprite) return false;
	//if (!npc.sprite) return false;

	pc.spriteX = 64;
	pc.spriteY = 64;



	return true;
}

bool Game_Init(HWND window)
{
	Direct3D_Init(window, SCREENW, SCREENH, false);
	DirectInput_Init(window);
	DirectSound_Init(window);

	if (!Menu_Init(window)) return false;

	//Create pointer to the back buffer
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);

	//Create Font
	fontArial24 = MakeFont(options.mainFont, 50);
	smallArial = MakeFont(options.mainFont, 25);

	//load audio files
	menuMusic = LoadSound(options.menuMusic);
	menuSelect = LoadSound(options.menuSelectSound);
	backgroundMusic = LoadSound(options.backgroundMusic);
	walkingSound = LoadSound(options.walkingSound);

	game_state = StartScreen;

	return true;
}

void Draw_Game(HWND window)
{
	int pcTileLocation = pc.getTile(0, 0, ScrollX, ScrollY);
	bool areSpritsSetup = true;

	if (game_state != prev_game_state)
	{
		menuMusic->Stop();
		LoopSoundVolume(backgroundMusic, options.MusicVolume);
				
		if (!Map_init(window)) return;

		prev_game_state = game_state; 		
	}
	timer++;
	if (Key_Down(DIK_LEFT) || controllers[0].sThumbLX < -5000)
	{
		pcTileLocation = pc.getTile(11, 0, ScrollX, ScrollY);

		//Note(Nathan): we allow the pc to move left or right even though they are in the top border tile, this give more room to move, without needing to be pixel perfect
		if (isGoal(pcTileLocation))
		{
			game_state = Win;
		}

		if (!isSolid(pcTileLocation) || (pcTileLocation == 2 || pcTileLocation == 22))
		{
			ScrollX -= 1;
			pc.spriteX -= 1;
			Sprite_Animate(frame, 4, 7, 1, starttime, 120);
			PlaySound(walkingSound, options.sfxVolume);
		}
	}
	else if (Key_Down(DIK_RIGHT) || controllers[0].sThumbLX > 5000)
	{
		pcTileLocation = pc.getTile(32, 0, ScrollX, ScrollY);
		if (isGoal(pcTileLocation))
		{
			game_state = Win;
		}

		if (!isSolid(pcTileLocation) || (pcTileLocation == 2 || pcTileLocation == 22))
		{
			ScrollX += 1;
			pc.spriteX += 1;
			Sprite_Animate(frame, 8, 11, 1, starttime, 120);
			PlaySound(walkingSound, options.sfxVolume);
		}
	}
	else if (Key_Down(DIK_DOWN) || controllers[0].sThumbLY < -5000)
	{
		pcTileLocation = pc.getTile(0, 64, ScrollX, ScrollY);
		if (isGoal(pcTileLocation))
		{
			game_state = Win;
		}

		if (!isSolid(pcTileLocation) || (pcTileLocation == 11 || pcTileLocation == 13))
		{
			ScrollY += 1;
			pc.spriteY += 1;
			Sprite_Animate(frame, 0, 3, 1, starttime, 120);
			PlaySound(walkingSound, options.sfxVolume);
		}
	}
	else if (Key_Down(DIK_UP) || controllers[0].sThumbLY > 5000)
	{
		pcTileLocation = pc.getTile(0, 32, ScrollX, ScrollY);

		if (isGoal(pcTileLocation))
		{
			game_state = Win;
		}

		if (!isSolid(pcTileLocation) || (pcTileLocation == 11 || pcTileLocation == 13))
		{
			ScrollY -= 1;
			pc.spriteY -= 1;
			Sprite_Animate(frame, 12, 15, 1, starttime, 120);
			PlaySound(walkingSound, options.sfxVolume);
		}
	}

	//update the scrolling view
	UpdateScrollPosition();

	if (d3ddev->BeginScene())
	{
		DrawTiles();

		DrawScrollWindow();

		spriteobj->Begin(D3DXSPRITE_ALPHABLEND);

		Sprite_Draw_Frame(pc.sprite, pc.spriteX, pc.spriteY, frame, 32, 48, 4);
		//Sprite_Draw_Frame(npc.sprite, npc.spriteX, npc.spriteY, frame, 32, 48, 4);

		std::ostringstream oss;
		oss << "FPS = " << (fps * 4) << ", Timer = " << (timer / 60);
		FontPrint(smallArial, 0, 0, oss.str());

		spriteobj->End();

		//stop rendering
		d3ddev->EndScene();
		d3ddev->Present(NULL, NULL, NULL, NULL);
	}

	if ((KEY_DOWN(VK_ESCAPE)) || (controllers[0].wButtons & XINPUT_GAMEPAD_START))
	{
		game_state = Paused;
		backgroundMusic->Stop();
		Sleep(150);
	}
}

void Display_MainMenu(HWND window)
{
	spriteobj->Begin(D3DXSPRITE_ALPHABLEND);

	if (game_state != prev_game_state)
	{
		menuPointerLocation.x = 325;
		menuPointerLocation.y = 420;
		prev_game_state = game_state;
		LoopSoundVolume(menuMusic, options.MusicVolume);
	}

	if (d3ddev->BeginScene())
	{
		d3ddev->StretchRect(screenBackground, NULL, backbuffer, NULL, D3DTEXF_NONE);

		FontPrint(fontArial24, 375, 400, "Start");
		FontPrint(fontArial24, 375, 450, "Options");
		FontPrint(fontArial24, 375, 500, "ScoreBoard");
		FontPrint(fontArial24, 375, 550, "Credits");
		FontPrint(fontArial24, 375, 600, "Exit");

		Sprite_Draw_Frame(menuPointer, menuPointerLocation.x, menuPointerLocation.y, 0, 32, 32, 1);

		//stop rendering
		spriteobj->End();
		d3ddev->EndScene();
		d3ddev->Present(NULL, NULL, NULL, NULL);
	}

	if ((KEY_DOWN(VK_RETURN)) || (controllers[0].wButtons & XINPUT_GAMEPAD_A))
	{
		if (menuPointerLocation.y == 420)
			game_state = Running;
		else if (menuPointerLocation.y == 470)
			game_state = OptionMenu;
		else if (menuPointerLocation.y == 520)
			game_state = ScoreBoard;
		else if (menuPointerLocation.y == 570)
			game_state = Credits;
		else if (menuPointerLocation.y == 620)
			game_state = Exited;

		PlaySound(menuSelect, options.sfxVolume);
		menuMusic->Stop();
		Sleep(150);
	}
	if ((KEY_DOWN(VK_DOWN)) || (controllers[0].wButtons & XINPUT_GAMEPAD_DPAD_DOWN)) //TODO(Nathan): work out thumb stick
	{
		if (menuPointerLocation.y < 620)
		{
			menuPointerLocation.y += 50;
		}
		else
		{
			menuPointerLocation.y = 420;
		}

		PlaySound(menuSelect, options.sfxVolume);

		Sleep(150);
	}

	if ((KEY_DOWN(VK_UP)) || (controllers[0].wButtons & XINPUT_GAMEPAD_DPAD_UP)) //(Nathan): work out thumb stick
	{
		if (menuPointerLocation.y > 420)
		{
			menuPointerLocation.y -= 50;
		}
		else
		{
			menuPointerLocation.y = 620;
		}

		PlaySound(menuSelect, options.sfxVolume);

		Sleep(150);
	}

}

void Display_OptionsMenu(HWND window)
{
	if (game_state != prev_game_state)
	{
		menuPointerLocation.x = 150;
		menuPointerLocation.y = 470;
		LoopSoundVolume(menuMusic, options.MusicVolume);
		prev_game_state = game_state;
	}

	if (d3ddev->BeginScene())
	{
		spriteobj->Begin(D3DXSPRITE_ALPHABLEND);


		d3ddev->StretchRect(screenBackground, NULL, backbuffer, NULL, D3DTEXF_NONE);

		string musicVolume = string("-  ") + to_string(options.MusicVolume) + string("%  +");
		string sfxVolume = string("-  ") + to_string(options.sfxVolume) + string("%  +");

		FontPrint(fontArial24, 375, 400, "Options");
		//FontPrint(fontArial24, 375, 500, "Character Select");
		FontPrint(fontArial24, 200, 450, "Music Volume:");
		FontPrint(fontArial24, 540, 450, musicVolume);
		FontPrint(fontArial24, 200, 500, "Sound Volume:");
		FontPrint(fontArial24, 540, 500, sfxVolume);

		FontPrint(fontArial24, 200, 600, "ESC to return to Main Menu");

		Sprite_Draw_Frame(menuPointer, menuPointerLocation.x, menuPointerLocation.y, 0, 32, 32, 1);

		//stop rendering
		spriteobj->End();
		d3ddev->EndScene();
		d3ddev->Present(NULL, NULL, NULL, NULL);
	}

	if ((KEY_DOWN(VK_DOWN)) || (controllers[0].wButtons & XINPUT_GAMEPAD_DPAD_DOWN)) //(Nathan): work out thumb stick
	{
		if (menuPointerLocation.y == 470)
			menuPointerLocation.y = 520;
		else
			menuPointerLocation.y = 470;

		PlaySound(menuSelect, options.sfxVolume);

		Sleep(150);
	}

	if ((KEY_DOWN(VK_UP)) || (controllers[0].wButtons & XINPUT_GAMEPAD_DPAD_UP)) //(Nathan): work out thumb stick
	{
		if (menuPointerLocation.y == 470)
			menuPointerLocation.y = 520;
		else
			menuPointerLocation.y = 470;

		PlaySound(menuSelect, options.sfxVolume);

		Sleep(150);
	}

	if ((KEY_DOWN(VK_LEFT)) || (controllers[0].wButtons & XINPUT_GAMEPAD_DPAD_LEFT)) //(Nathan): work out thumb stick
	{
		if (menuPointerLocation.y == 470)
		{
			if (options.MusicVolume > 0)
			{
				options.MusicVolume -= 5;
				SetSoundVolume(menuMusic, options.MusicVolume);

			}

		}
		else
		{
			if (options.sfxVolume > 0)
				options.sfxVolume -= 5;
		}

		PlaySound(menuSelect, options.sfxVolume);

		Sleep(150);
	}

	if ((KEY_DOWN(VK_RIGHT)) || (controllers[0].wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)) //(Nathan): work out thumb stick
	{
		if (menuPointerLocation.y == 470)
		{
			if (options.MusicVolume < 100)
			{
				options.MusicVolume += 5;
				SetSoundVolume(menuMusic, options.MusicVolume);
			}
		}
		else
		{
			if (options.sfxVolume < 100)
				options.sfxVolume += 5;
		}

		PlaySound(menuSelect, options.sfxVolume);

		Sleep(150);
	}

	if ((KEY_DOWN(VK_ESCAPE)) || (controllers[0].wButtons & XINPUT_GAMEPAD_B))
	{
		game_state = MainMenu;

		PlaySound(menuSelect, options.sfxVolume);
		menuMusic->Stop();
		Sleep(150); //have to slow things down or it will go straight into game
	}
}

void Display_Scoreboard(HWND window)
{
	if (game_state != prev_game_state)
	{
		LoopSoundVolume(menuMusic, options.MusicVolume);
		prev_game_state = game_state;
	}

	if (d3ddev->BeginScene())
	{
		spriteobj->Begin(D3DXSPRITE_ALPHABLEND);

		d3ddev->StretchRect(screenBackground, NULL, backbuffer, NULL, D3DTEXF_NONE);

		FontPrint(fontArial24, 375, 500, "Scoreboard");
		FontPrint(fontArial24, 350, 600, "Coming Soon!");

		//stop rendering
		spriteobj->End();
		d3ddev->EndScene();
		d3ddev->Present(NULL, NULL, NULL, NULL);
	}

	if ((KEY_DOWN(VK_ESCAPE)) || (controllers[0].wButtons & XINPUT_GAMEPAD_B))
	{
		game_state = MainMenu;
		PlaySound(menuSelect, options.sfxVolume);
		menuMusic->Stop();
		Sleep(150); //have to slow things down or it will go straight into game
	}
}

void Display_Credits(HWND window)
{
	if (game_state != prev_game_state)
	{
		LoopSoundVolume(menuMusic, options.MusicVolume);
		prev_game_state = game_state;
		int textStartingPoint = 800;

		if (CreditText.empty())
		{
			ifstream ifs("License.txt");
			string line;
			while (getline(ifs, line))
			{
				if (line.empty())
					continue;

				Credit temp;
				temp.line = line;
				temp.x = 100;
				temp.y = textStartingPoint;

				CreditText.push_back(temp);

				textStartingPoint += 50;
			}
		}
		else
		{
			for (auto text = CreditText.begin(); text != CreditText.end(); ++text)
			{
				text->y = textStartingPoint;
				textStartingPoint += 50;
			}
		}


	}

	if (d3ddev->BeginScene())
	{
		spriteobj->Begin(D3DXSPRITE_ALPHABLEND);


		d3ddev->StretchRect(screenBackground, NULL, backbuffer, NULL, D3DTEXF_NONE);

		for (auto text = CreditText.begin(); text != CreditText.end(); ++text)
		{
			text->y -= 3; // 6 is our magic number for fast, 1 for slow

			FontPrint(smallArial, text->x, text->y, text->line);
		}

		Credit temp = CreditText.back();

		if (temp.y < 0)
		{
			game_state = MainMenu;
		}

		//stop rendering
		spriteobj->End();
		d3ddev->EndScene();
		d3ddev->Present(NULL, NULL, NULL, NULL);

	}

	if ((KEY_DOWN(VK_ESCAPE)) || (controllers[0].wButtons & XINPUT_GAMEPAD_B))
	{
		game_state = MainMenu;
		PlaySound(menuSelect, options.sfxVolume);
		menuMusic->Stop();
		Sleep(150); //have to slow things down or it will go straight into game
	}
}

void Display_PauseScreen(HWND window)
{

	if (game_state != prev_game_state)
	{
		prev_game_state = game_state;
		menuPointerLocation.x = 325;
		menuPointerLocation.y = 520;
		LoopSoundVolume(menuMusic, options.MusicVolume);
	}

	if (d3ddev->BeginScene())
	{
		spriteobj->Begin(D3DXSPRITE_ALPHABLEND);


		d3ddev->StretchRect(screenBackground, NULL, backbuffer, NULL, D3DTEXF_NONE);

		FontPrint(fontArial24, 375, 450, "Paused");
		FontPrint(fontArial24, 375, 500, "Quit To Main Menu");
		FontPrint(fontArial24, 375, 550, "Quit To Windows");

		Sprite_Draw_Frame(menuPointer, menuPointerLocation.x, menuPointerLocation.y, 0, 32, 32, 1);


		//stop rendering
		spriteobj->End();
		d3ddev->EndScene();
		d3ddev->Present(NULL, NULL, NULL, NULL);
	}

	if ((KEY_DOWN(VK_RETURN)) || (controllers[0].wButtons & XINPUT_GAMEPAD_A))
	{
		if (menuPointerLocation.y == 520)
			game_state = MainMenu;
		else if (menuPointerLocation.y == 570)
			game_state = Exited;

		Sleep(150);
	}
	if ((KEY_DOWN(VK_DOWN)) || (controllers[0].wButtons & XINPUT_GAMEPAD_DPAD_DOWN)) //(Nathan): work out thumb stick
	{
		if (menuPointerLocation.y == 520)
			menuPointerLocation.y = 570;
		else
			menuPointerLocation.y = 520;

		Sleep(150);
	}

	if ((KEY_DOWN(VK_UP)) || (controllers[0].wButtons & XINPUT_GAMEPAD_DPAD_UP)) //(Nathan): work out thumb stick
	{
		if (menuPointerLocation.y == 520)
			menuPointerLocation.y = 570;
		else
			menuPointerLocation.y = 520;

		Sleep(150);
	}

	if ((KEY_DOWN(VK_ESCAPE)) || (controllers[0].wButtons & XINPUT_GAMEPAD_START)) //(Nathan): work out thumb stick
	{
		game_state = Running;
		PlaySound(menuSelect, options.sfxVolume);
		menuMusic->Stop();
		Sleep(150);
	}

}

void Display_StartScreen(HWND window)
{

	if (game_state != prev_game_state)
	{
		prev_game_state = game_state;
		LoopSoundVolume(menuMusic, options.MusicVolume);
	}

	//RECT rect;

	string text = "START GAME";

	////Mouse Crap
	//D3DCOLOR white = D3DCOLOR_XRGB(255, 255, 255);
	//TEXTMETRICA *tmet = new TEXTMETRICA();
	//fontArial24->GetTextMetricsA(tmet);
	//int widthOfString = text.length() + tmet->tmMaxCharWidth;
	//
	//rect.top = 500;
	//rect.left = 375;
	//rect.right = 0; //(rect.left - widthOfString);
	//rect.bottom = 0;  //(rect.top + 50);

	if (d3ddev->BeginScene())
	{
		spriteobj->Begin(D3DXSPRITE_ALPHABLEND);


		d3ddev->StretchRect(screenBackground, NULL, backbuffer, NULL, D3DTEXF_NONE);


		//fontArial24->DrawText(NULL, text.c_str(), text.length(), &rect, DT_CALCRECT, white);

		//attempt at mouse crap
		////print the text
		//fontArial24->DrawText(spriteobj, text.c_str(), text.length(), &rect, DT_LEFT, white);

		FontPrint(fontArial24, 375, 500, text);

		//was for mouse location information
		//stringstream ss;
		//ss << mouse_location.x;
		//FontPrint(fontArial24, 10, 10, ss.str());

		//stop rendering
		spriteobj->End();
		d3ddev->EndScene();
		d3ddev->Present(NULL, NULL, NULL, NULL);
	}

	//just get click

	if ((KEY_DOWN(VK_RETURN)) || (controllers[0].wButtons & XINPUT_GAMEPAD_A))
	{
		game_state = MainMenu;
		PlaySound(menuSelect, options.sfxVolume);
		menuMusic->Stop();
		Sleep(150); //have to slow things down or it will go straight into game
	}

	if (Mouse_Button(0))
	{
		game_state = MainMenu;
		PlaySound(menuSelect, options.sfxVolume);
		menuMusic->Stop();
		Sleep(150); //This one doesn't have a problem with speed but to keep things looking even
	}
}



void Game_Run(HWND window)
{
	if (!d3ddev) return;
	DirectInput_Update();
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 100), 1.0f, 0);

	//the timing stuff was taken from chapter 14, as my sprite kept running off the screen and was really fast.
	//get current ticks
	currenttime = timeGetTime();

	//calculate core frame rate
	corecount += 1.0;
	if (currenttime > coretime + 1000)
	{
		corefps = corecount;
		corecount = 0.0;
		coretime = currenttime;
	}

	fps = GetTickCount() - start;
	
	//if (currenttime > refresh + 14)
	if (fps >= 14)
	{
		
		refresh = currenttime;
		start = GetTickCount();

		switch (game_state)
		{
		case StartScreen:
			Display_StartScreen(window);
			break;
		case MainMenu:
			Display_MainMenu(window);
			break;
		case OptionMenu:
			Display_OptionsMenu(window);
			break;
		case ScoreBoard:
			Display_Scoreboard(window);
			break;
		case Credits:
			Display_Credits(window);
			break;
		case Running:
			Draw_Game(window);
			break;
		case Paused:
			Display_PauseScreen(window);
			break;
		case Exited:
			gameover = true;
			break;
		case Win:
			//TODO(Nathan): display win screen
			Display_PauseScreen(window);
			break;
		case Dead:
			//TODO(Nathan): display lose screen
			Display_MainMenu(window);
			break;
		default:
			//TODO(Nathan):log as something went really wrong.
			break;
		}
	}
}

void Game_End()
{
	if (screenBackground) screenBackground->Release();
	if (scrollbuffer) scrollbuffer->Release();
	if (tiles) tiles->Release();

	/*if (pc.sprite) pc.sprite->Release();
	if (npc.sprite) pc.sprite->Release();*/

	if (fontArial24) fontArial24->Release();
	if (menuPointer) menuPointer->Release();

	if (menuMusic) delete menuMusic;
	if (menuSelect) delete menuSelect;
	if (backgroundMusic) delete backgroundMusic;
	if (walkingSound) delete walkingSound;

	DirectInput_Shutdown();
	Direct3D_Shutdown();
}