#include <3ds.h>
#include <sf2d.h>
#include <sftd.h>
#include <sfil.h>

#include "GoL.h"
#include "OpenSans_ttf.h"

using namespace std;

GoL gol;
int timer=0;
int maxTimer=10;
bool paused=false;

touchPosition touch;
circlePosition cSPos;
circlePosition cPos;
float deadzone=15;

int mapX, mapY;

float scale=1.000;
int frameSkip=0;

int renderColors[5]={ (RGBA8(0,0,0,255)), (RGBA8(0,255,0,255)), (RGBA8(0,255,255,255)), (RGBA8(255,0,255,255)), (RGBA8(128,255,128,255))};
string renderColorsString[5]={"Default", "Green", "Blue-Green", "Pink", "Light Green"};
int curRenderColor=0;

int main(int argc, char **argv)
{
	//Inits
	romfsInit();

	sf2d_init();
	sf2d_set_clear_color(RGBA8(0x40, 0x40, 0x40, 0xFF));

	sf2d_texture *tex_a = sfil_load_PNG_file("romfs:/template.png", SF2D_PLACE_RAM);

	sftd_init();
	sftd_font *font = sftd_load_font_mem(OpenSans_ttf, OpenSans_ttf_size);
	sftd_draw_textf(font, 0, 0, RGBA8(255, 0, 0, 255), 50, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890:-.'!?()\"end"); //Hack to avoid blurry text!

	//Seed random number generator
	srand(time(NULL));

	while (aptMainLoop()){
		hidScanInput();
		hidTouchRead(&touch);
		hidCstickRead (&cSPos);
		hidCircleRead (&cPos);
		u32 kDown = hidKeysDown();
		u32 kHeld = hidKeysHeld();
		if (kDown & KEY_X){
			gol.randomize();
		}
		if (kDown & KEY_A){
			paused=!paused;
		}
		if (kDown & KEY_Y){
			gol.randomPreset();
		}
		if (kDown & KEY_START){
			break;
		}
		if (kDown & KEY_DUP && maxTimer > 1){
			maxTimer--;
		}
		if (kDown & KEY_DDOWN && maxTimer < 60){
			maxTimer++;
		}
		if (kDown & KEY_DRIGHT && maxTimer < 60){
			frameSkip++;
		}
		if (kDown & KEY_DLEFT && maxTimer < 60){
			frameSkip--;
		}
		if (kDown & KEY_B || kDown & KEY_R){
			curRenderColor++;
			if (curRenderColor >= (int)sizeof(renderColors)/4)
				curRenderColor=0;
		}
		if (kDown & KEY_L){
			curRenderColor--;
			if (curRenderColor < 0)
				curRenderColor=sizeof(renderColors)/4-1;
		}

		if (kHeld & KEY_TOUCH){
			int touchX, touchY;
			touchX=mapX+touch.px;
			touchY=mapY+touch.py;
			touchX=touchX/scale;
			touchY=touchY/scale;
			gol.setPosAlive(touchX, touchY);
			for (int i=0; i<320; i++){
					for (int j=0; j<240; j++){
						if (gol.getPosAlive(i,j) == 1){
							//Add alive cells to texture
							sf2d_set_pixel(tex_a, i, j, (u32)renderColors[curRenderColor]);	
						}else{
							sf2d_set_pixel(tex_a, i, j, RGBA8(255,255,255,255));
						}
					}
				}
		}

		if (abs(cSPos.dy) > 5){
			float oldscale=scale;
			scale=scale+cSPos.dy/50.0;
			if (scale < 1)
				scale = 1;
			if (scale > 10)
				scale = 10;

			mapX = ((mapX+160)/oldscale)*scale-160;
			mapY = ((mapY+120)/oldscale)*scale-120;

			if (mapX < 0)
				mapX = 0;
			if (mapX+320 > 320*scale)
				mapX=scale*320-320;
			if (mapY < 0)
				mapY = 0;
			if (mapY+240 > 240*scale)
				mapY=scale*240-240;
		}

		if (abs(cPos.dx) > deadzone){
			mapX+=cPos.dx/(15/(scale/3));
			if (mapX < 0)
				mapX = 0;
			if (mapX+320 > 320*scale)
				mapX=scale*320-320;
		}
		if (abs(cPos.dy) > deadzone){
			mapY-=cPos.dy/(15/(scale/3));
			if (mapY < 0)
				mapY = 0;
			if (mapY+240 > 240*scale)
				mapY=scale*240-240;
		}

		if (!paused){
			timer++;
			if (timer>=maxTimer){
				if (maxTimer == 1){
					for (int i=0; i<frameSkip; i++){
						gol.update();
					}
				}
				gol.update();
				for (int i=0; i<320; i++){
					for (int j=0; j<240; j++){
						if (gol.getPosAlive(i,j) == 1){
							//Add alive cells to texture
							sf2d_set_pixel(tex_a, i, j, (u32)renderColors[curRenderColor]);	
						}else{
							sf2d_set_pixel(tex_a, i, j, RGBA8(255,255,255,255));
						}
					}
				}
				timer=0;
			}
		}
		string playing_str="Not Playing";
		if (!paused)
			playing_str="Playing";
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			sftd_draw_textf(font, 10, 10, RGBA8(255,255,255,255), 15, "Game Of Life");
			sftd_draw_textf(font, 200, 10, RGBA8(255,255,255,255), 15, "Alive color: %s - 0x%X, %i", renderColorsString[curRenderColor].c_str(), renderColors[curRenderColor], sizeof(renderColors));
			sftd_draw_textf(font, 10, 30, RGBA8(255,255,255,255), 15, "Alive: %i", gol.getAlive());
			sftd_draw_textf(font, 10, 50, RGBA8(255,255,255,255), 15, "Dead : %i", gol.getDead());
			sftd_draw_textf(font, 10, 70, RGBA8(255,255,255,255), 15, "Simulation speed: %.1f, Currently %s", 30.0/maxTimer, playing_str.c_str());
			sftd_draw_textf(font, 10, 90, RGBA8(255,255,255,255), 15, "Frameskip (only at max speed): %i", frameSkip);
			sftd_draw_textf(font, 10, 120, RGBA8(255,255,255,255), 15, "Controls:\nD-pad up/down: speed up/down\nDpad left/right: frameskip less/more\nA: Play/Pause");
			sftd_draw_textf(font, 320, 220, RGBA8(255,255,255,255), 15, "fps: %.2f", sf2d_get_fps());
		sf2d_end_frame();
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			//Render to screen
			sf2d_draw_texture_scale(tex_a, -mapX, -mapY, scale, scale);
		sf2d_end_frame();

		sf2d_swapbuffers();
	}
	sftd_free_font(font);
	sftd_fini();
	sf2d_fini();
	romfsExit();
	return 1;
}