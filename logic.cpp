#include "logic.h"
#include <queue>
#include <cmath>
#include <iostream>

static const Bit BEEP{ 400, 500, 15000 };
static const Bit DELAY{ 0, 300, 0 };
static void raysToSound(const std::vector<Ray>& data, std::list<Bit>& si);

Logic::Logic(LidarControl& lc, AudioPlayer& ap,
	RayIllustrator& ri) : is_off{ false }, lidar{ lc }, sound{ ap }, render{ ri } {

}

void Logic::quit() {
	is_off = true;
}

void Logic::run() {
	while (!is_off) {
		auto data = lidar.GetData();
		std::list<Bit> sound_interpret;
		sound.setPosition(0, 1, 0);
		raysToSound(data, sound_interpret);
		sound.Play(sound_interpret);

		int left, amount_was = sound_interpret.size();
		while ((left = sound.Wait(50)) != 0 && !is_off) {
			HandleEvents();

			int playedCount = amount_was - left;		
			setPos(playedCount, data);
			render.RenderAsTriangles(data, playedCount);
		}
	}
}

void Logic::setPos(int played, const std::vector<Ray>& data) {
	if (!data.empty()) {
		double angle = data[played].angle;
		//double dist = data[played].dist / 1000;	//	TODO: shoudl we use this at all???
		sound.setPosition(static_cast<float>(cos(angle)),
			static_cast<float>(sin(angle)), 0);
	}
}


void raysToSound(const std::vector<Ray>& data, std::list<Bit>& si) {
	for (auto ray : data) {
		Bit sample{ 0, 25, 25000 }; //freq,length,amplitude
		long i = ray.dist / 250;
		sample.frequency = 650 - i * 20;
		//	cerr!!!
		
		si.push_back(sample);
	}
	si.push_front(DELAY);
	si.push_front(BEEP);
	si.push_front(DELAY);
}

void Logic::HandleEvents() {
	while (SDL_PollEvent(&event)) {
		switch (event.key.keysym.sym) {
		case SDLK_ESCAPE:
			quit();
			break;
		case SDLK_s:
			sound.clear();
			break;
		default:
			break;
		}
	}
}
