/**
 * Author: Jason Hoopes
 */

#include <vector>
#include <string>
#include <libraries/AudioFile/AudioFile.h>
#include <Bela.h>

#include <cmath>

#pragma once



class Granular 
{

public:

	Granular();
	~Granular();
	
	/**
	 * Setup all parameters necessary for granulation
	 *
	 * @param numGrains - number of grains to use during playback
	 * @param sampleRate - sample rate of audio device
	 * @param filename - String containing path to wav file (relative to render.cpp)

	 */
	bool setup(int numGrains, float sampleRate, const std::string& filename);
	
	
	/**
	 * Process the next audio frame from the grain at the given index
	 * 
	 * @param grainIdx - index of grain to process
	 * 
	 * @return an amplitude value between 0.0 and 1.0
	 */
	float processGrain(int grainIdx);

	/**
	 * Process the next audio frame, summed over all active grains
	 */
	float process();

	/**
	 * Generates a gain value between 0.0 and 1.0, based on a window of the given grain size 
	 * 
	 * @param pointer - the position of the current pointer
	 * @param grainSize - the size of the grain that is being windowed
	 * 
	 * @return a gain value normalized between 0.0 and 1.0
	 */
	float applyWindow(int pointer, float grainSize);
	
	/**
	 * Initializes every parameter to its default value
	 */
	bool initializeAllParams(float sampleRate);
	
	// Cubic interpolation 
	/**
	 * Interpolates the value of a sample with position `pos` between frames f1 and f2. 
	 * f0 and f3 are the neighboring samples on each respective side
	 * 
	 * @param pos - the position between frames f1 and f2
	 * 
	 * @returns the interpolated amplitude value
	 */ 
	float interpolateCubic(float f0, float f1, float f2, float f3, float pos);


	// Set grain size in milliseconds 
	void setGrainSizeMS(float grainSize);
	// Set file position in milliseconds
	void setFilePositionMS(float filePosition);
	// Set speed of read pointers
	// Playback speed is the amount of samples that the read pointer will be incremented by at each audio frame
	void setPlaybackSpeed(float playbackSpeed);
	
	// Set the probability that a grain will be played 
	// This must be a value between 0.0 and 1.0
	void setProbability(float probability);
	// Set the amount of randomness (0.0-1.0) that will be applied to the grain size
	void setGrainSizeRandomness(float randomness);
	// Set the amount of randomness (0.0-1.0) that will be applied to the file position;
	void setFilePositionRandomness(float randomness);
	
	// Propogates the global, stored parameters to a single grain
	void updateGrain(float grainIdx);
	// Propogates the global, stored parameters to all grains
	void updateAllGrains();
	
private:

	// Number of Grains
	int _numGrains;
	
	// Sample Rate for calculations
	float _sampleRate;
	
	// Buffer of sample values that will be read by grains
	std::vector<float> _sampleBuffer;

	// The current read pointer for each respective grain in frames
	std::vector<float> _pointers;
	
	// The current starting position of each grain in frames
	std::vector<float> _filePositions;
	
	// The current size of each grain in frames
	std::vector<float> _grainSizes;
	
	// Whether each grain should be muted for its next cycle. 
	// This resets at the end of every grain's cycle
	std::vector<bool> _muted;


	float _filePosition;
	float _grainSize;
	float _playbackSpeed;
	float _probability;
	float _filePositionRandomness;
	float _grainSizeRandomness;
	
	
};





