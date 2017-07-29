#ifndef GOL_H
#define GOL_H
#include <3ds.h>
#include <stdio.h>
#include <sf2d.h>
#include <sfil.h>
#include <stdlib.h>
#include <string>

using namespace std;

class GoL{
	public:
		GoL();
		int getAlive();
		int getDead();
		u32 getPosColor(int x, int y);
		void setPosAlive(int x, int y);
		int getAliveNeighbors(int x, int y);
		int getPosAlive(int x, int y);
		void update();
		void loadPreset(string fileLocation);
		void randomize();
		void randomPreset();
		sf2d_texture *getMapTexture();
	private:
		int board[320][240];
		int nextBoard[320][240];
		int alive=0;
		int dead=0;
		sf2d_texture *tex_map = sfil_load_PNG_file("romfs:/template.png", SF2D_PLACE_VRAM);
};

#endif