#include "GoL.h"
#include <sf2d.h>
#include <sfil.h>
#include <cstdlib> //For rand
#include <math.h>

GoL::GoL(){
	randomize();
}

int GoL::getAlive(){
	return alive;
}

int GoL::getDead(){
	return dead;
}

u32 GoL::getPosColor(int x, int y){
	u32 ret=RGBA8(255,255,255,255);
	if (board[x][y] == 1){
		ret=RGBA8(0,0,0,255);
	}
	return ret;
}

void GoL::setPosAlive(int x, int y){
	board[x][y]=1;
	//sf2d_set_pixel(tex_map, x, y, RGBA8(0,0,0,255));
}

int GoL::getAliveNeighbors(int x, int y){
	int ret=0;
	for (int i=0; i<3; i++){
		for (int j=0; j<3; j++){
			if (i != 1 || j != 1){
				if (getPosAlive(x-1+i, y-1+j) == 1)
					ret++;
			}
		}
	}
	return ret;
}

int GoL::getPosAlive(int x, int y){
	if (x >= 0 && x < 320 && y >= 0 && y < 240){
		return board[x][y];
	}else{
		return 2;
	}
}

void GoL::update(){
	alive=0;
	dead=0;
	for (int i=0; i < 320; i++){
		for (int j=0; j < 240; j++){
			nextBoard[i][j]=board[i][j];
			if (board[i][j] == 1){
				if (getAliveNeighbors(i,j) > 3 or getAliveNeighbors(i,j) < 2){
					nextBoard[i][j]=0; //It dies
					//sf2d_set_pixel(tex_map, i, j, RGBA8(255,255,255,255));
				}
			}else if (getAliveNeighbors(i,j) == 3){
				nextBoard[i][j]=1;
				//sf2d_set_pixel(tex_map, i, j, RGBA8(0,0,0,255));
			}
		}
	}
	for (int i=0; i < 320; i++){
		for (int j=0; j < 240; j++){
			board[i][j]=nextBoard[i][j];
			if (board[i][j])
				alive++;
			else
				dead++;
		}
	}
}

void GoL::randomize(){
	for (int i=0; i < 320; i++){
		for (int j=0; j < 240; j++){
			board[i][j] = round( rand() % 2 );
			if (board[i][j] == 1){
				//sf2d_set_pixel(tex_map, i, j, RGBA8(0,0,0,255));
			}else{
				//sf2d_set_pixel(tex_map, i, j, RGBA8(255,255,255,255));
			}
		}
	}
}

void GoL::loadPreset(string fileLocation){
	sf2d_texture *tex_preset = sfil_load_PNG_file(fileLocation.c_str(), SF2D_PLACE_RAM);
	u32 pixel;

	int width=tex_preset->width;
	int height=tex_preset->height;
	for (int i=0; i<width; i++){
		for (int j=0; j<height; j++){
			pixel = sf2d_get_pixel(tex_preset, i, j);
			if (pixel != (u32)RGBA8(255,255,255,255)){
				board[i][j]=1;
				//sf2d_set_pixel(tex_map, i, j, RGBA8(0,0,0,255));
			}
		}
	}

	sf2d_free_texture(tex_preset);
}

void GoL::randomPreset(){
	for (int i=0; i < 320; i++){
		for (int j=0; j < 240; j++){
			board[i][j]=0;
			sf2d_set_pixel(tex_map, i, j, RGBA8(255,255,255,255));
		}
	}
	int rnd=rand() % 4;
	loadPreset("romfs:/presets/" + to_string(rnd) + ".png");
}

sf2d_texture *GoL::getMapTexture(){
	//return tex_map;
}