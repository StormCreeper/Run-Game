#include "SoundManager.h"
#include <vector>
#include <iostream>

std::map<const char*, Sound> SoundManager::sounds;

bool SoundManager::initOpenAL() {
	ALCdevice* device = alcOpenDevice(NULL);
	if (!device) return false;

	ALCcontext* context = alcCreateContext(device, NULL);
	if (!context) return false;

	if (!alcMakeContextCurrent(context)) return false;

	return true;
}

void SoundManager::shutdownOpenAL() {

	for (std::pair<const char*, Sound> element : sounds) {
		alDeleteBuffers(1, &element.second.buffer);
		alSourcei(element.second.source, AL_BUFFER, 0);
		alDeleteSources(1, &element.second.source);

	}
	ALCcontext* context = alcGetCurrentContext();
	ALCdevice* device = alcGetContextsDevice(context);

	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);
}

Sound SoundManager::loadSound(const char* filename, const char* name) {
	SF_INFO fileInfo;
	SNDFILE* file = sf_open(filename, SFM_READ, &fileInfo);
	if (!file) {
		std::cout << "Unable to open file : " << filename << std::endl;
		return { 0, 0 };
	}
	ALsizei nbSamples = static_cast<ALsizei>(fileInfo.channels * fileInfo.frames);
	ALsizei sampleRate = static_cast<ALsizei>(fileInfo.samplerate);

	std::vector<ALshort> samples(nbSamples);
	if (!sf_read_short(file, &samples[0], nbSamples)) {
		std::cout << "Error while reading the file" << std::endl;
		return { 0, 0 };
	}
	sf_close(file);

	ALenum format;
	switch (fileInfo.channels) {
	case 1:  format = AL_FORMAT_MONO16;   break;
	case 2:  format = AL_FORMAT_STEREO16; break;
	default: std::cout << "Invalid format" << std::endl; return { 0, 0 };
	}

	ALuint buffer;
	alGenBuffers(1, &buffer);
	alBufferData(buffer, format, &samples[0], nbSamples * sizeof(ALushort), sampleRate);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		std::cout << "Error : ";
		switch (error) {
		case AL_INVALID_NAME:
			std::cout << "Invalid Name";
			break;
		case AL_INVALID_ENUM:
			std::cout << "Invalid Enum";
			break;
		case AL_INVALID_VALUE:
			std::cout << "Invalid Value";
			break;
		case AL_INVALID_OPERATION:
			std::cout << "Invalid Operation";
			break;
		case AL_OUT_OF_MEMORY:
			std::cout << "Out of Memory";
			break;
		}
		std::cout << " - " << filename << std::endl;
		return { 0, 0 };
	}
	ALuint source;
	alGenSources(1, &source);
	alSourcei(source, AL_BUFFER, buffer);

	sounds[name] = { buffer, source};

	return { buffer, source };
}

void SoundManager::playSound(const char* name) {
	playSound(getSound(name));
}
void SoundManager::playSound(Sound sound) {
	alSourcePlay(sound.source);
}

void SoundManager::stopSound(const char* name) {
	stopSound(getSound(name));
}
void SoundManager::stopSound(Sound sound) {
	alSourceStop(sound.source);
}

Sound SoundManager::getSound(const char* name) {
	return sounds[name];
}
bool SoundManager::isPlaying(const char* name) {
	return isPlaying(getSound(name));
}
void SoundManager::changeVolume(const char* name, float value) {
	changeVolume(getSound(name), value);
}
bool SoundManager::isPlaying(Sound sound) {
	ALint status;
	alGetSourcei(sound.source, AL_SOURCE_STATE, &status);
	return status == AL_PLAYING;
}
void SoundManager::changeVolume(Sound sound, float value) {
	alSourcef(sound.source, AL_GAIN, value);
}

void SoundManager::setLooping(const char* name, bool loop) {
	setLooping(getSound(name), loop);
}
void SoundManager::setLooping(Sound sound, bool loop) {
	alSourcei(sound.source, AL_LOOPING, loop);
}