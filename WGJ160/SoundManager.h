#pragma once

#include <cstdlib>
#include <AL/al.h>
#include <AL/alc.h>
#include <sndfile.h>
#include <map>

struct Sound {
	ALuint buffer;
	ALuint source;
};

class SoundManager {
	static std::map<const char*, Sound> sounds;
public:
	static bool initOpenAL();
	static void shutdownOpenAL();

	static Sound loadSound(const char* filename, const char* name);

	static void playSound(const char* name);
	static void playSound(Sound sound);
	static void stopSound(const char* name);
	static void stopSound(Sound sound);

	static Sound getSound(const char* name);

	static bool isPlaying(const char* name);
	static void changeVolume(const char* name, float value);
	static bool isPlaying(Sound sound);
	static void changeVolume(Sound sound, float value);

	static void setLooping(const char* name, bool loop);
	static void setLooping(Sound sound, bool loop);
};

