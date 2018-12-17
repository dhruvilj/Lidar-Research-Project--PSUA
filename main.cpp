#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <iostream>
#include "audio_player.h"
#include "lidar_control.h"
#include "ray_illustrator.h"
#include "logic.h"
//#include "server.h"

using namespace std;
int main(int argc, char* argv[]) {
    //Server server{1500};
    //server.waitForClient();
    //server.closeConnection(); //debug only
	RayIllustrator render{ 512, 512, "LIDAR" };
	if (render.Init()) { 
		//LidarControl lidar{ "12345.data" }; // timestamp.data for file, and just { } for lidar
		LidarControl lidar{}; // timestamp.data for file, and just { } for lidar
		if (lidar.Init()) { 
			lidar.set_log_data_off(); 
			lidar.setInfinity(true); 

			AudioPlayer sound{}; 
			if (sound.Init()) {
 
				//-----------------------------
				Logic logic(lidar, sound, render); 
				logic.run(); 
				//-----------------------------

				sound.Terminate();
			}
			lidar.Terminate();
		}
		render.Terminate();
	}
	render.Terminate();
	SDL_Quit();
	return 0;
}
