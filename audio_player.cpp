#define _USE_MATH_DEFINES

#include "audio_player.h"
#include <SFML/System/Clock.hpp>
#include <SFML/Audio/SoundStream.hpp>
#include <chrono>
#include <thread>
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;
using namespace sf;

void AudioPlayer::clear()
{
	mtx.lock();
	bits.clear();
	mtx.unlock();
}

bool AudioPlayer::Init()
{
	initialize(1, 44100);

	return true;
}

void AudioPlayer::Terminate()
{
	stop();
}

size_t AudioPlayer::calculateLength() {
	size_t length = 0;
	for (Bit b : bits) {
		length += b.samples_left;
	}
	return length;
}

bool AudioPlayer::onGetData(Chunk& data)
{
	size_t cl = calculateLength();
	size_t length = (cl > 2048) ? 2048 : cl;
	auto stream = new sf::Int16[length];

	mtx.lock();

	unsigned i = 0;
	while (i < length) {
		if (bits.empty()) {
			return false;
		}
		Bit& step = bits.front();
		unsigned samplesToDo = (i + step.samples_left > length) ? length : i + step.samples_left;
		step.samples_left -= samplesToDo - i;
		int samples_per_sec = getSampleRate();
		while (i < samplesToDo) {
			stream[i] = static_cast<sf::Int16>(
				step.amplitude * std::sin(v * 2 * M_PI / samples_per_sec));
			i++;
			v += step.frequency;
		}

		if (step.samples_left == 0) {
			bits.pop_front();
		}
	}

	mtx.unlock();

	data.samples = stream;
	data.sampleCount = length;

	return true;
}

void AudioPlayer::Play(list<Bit> _bits)
{
	int samples_per_sec = getSampleRate();
	for (Bit &b : _bits)
		b.samples_left = b.length * samples_per_sec / 1000;

	mtx.lock();
	bits.splice(bits.end(), _bits);
	mtx.unlock();
	play();
}

int AudioPlayer::Wait(int time)
{
	int time_passed = 0;
	int size;
	sf::Clock clock;
	
	do {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		mtx.lock();
		size = bits.size();
		mtx.unlock();
		time_passed += clock.getElapsedTime().asMilliseconds();
	} while (size > 0 && time_passed < time);
	return size;
}

void AudioPlayer::onSeek(sf::Time timeOffset)
{
	size_t skipCount = static_cast<std::size_t>(
		timeOffset.asSeconds() * getSampleRate() * getChannelCount());
	mtx.lock();
	while (!bits.empty() && skipCount != 0) {
		Bit b = bits.front();
		if (b.samples_left < skipCount) {
			skipCount -= b.samples_left;
			bits.pop_front();
		}
		else {
			b.samples_left -= skipCount;
			skipCount = 0;
		}
	}
	mtx.unlock();
}
