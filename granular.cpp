/**
 * Author: Jason Hoopes
 */ 


#include "granular.h"


Granular::Granular() {}
Granular::~Granular() {}

bool Granular::initializeAllParams(float sampleRate) 
{

	for (int i = 0; i < _numGrains; i++) {
		
		this->_pointers[i] = 0.0;
		this->_filePositions[i] = 0;
		this->_grainSizes[i] = 0.0 * sampleRate;
		this->_muted[i] = false;
		
	}
	
	return true;
}

bool Granular::setup(int numGrains, float sampleRate, const std::string & filename) 
{
	this->_numGrains = numGrains;
	this->_sampleRate = sampleRate;
	this->_pointers.resize(this->_numGrains);
	this->_filePositions.resize(this->_numGrains);
	this->_grainSizes.resize(this->_numGrains);
	this->_muted.resize(this->_numGrains);
	
	// Set the starting values for each parameter
	if (!this->initializeAllParams(sampleRate)) {
		return false;
	}
	
	_sampleBuffer = AudioFileUtilities::loadMono(filename);
	if (_sampleBuffer.size() == 0) {
		rt_printf("Failed loading file with path \"%s\"", filename.c_str());
		return false;
	}
	
	rt_printf("Loaded sample with %d frames \n", _sampleBuffer.size());

	return true;
}

float Granular::interpolateCubic(float f0, float f1, float f2, float f3, float pos) {
	
	float a0 = f3 - f2 - f0 + f1;
	float a1 = f0 - f1 - a0;
	float a2 = f2 - f0;
	float a3 = f1;

	return a0*(std::pow(pos,3)) + a1*(std::pow(pos,2)) + a2*pos + a3;
}


float Granular::processGrain(int grainIdx)
{
	
	float out = 0.0;
	
	if (_muted[grainIdx]) {
		return out;
	}
	
	float pos = _pointers[grainIdx] + _filePositions[grainIdx];
	int prevFrame = floorf(pos);
	
	if (prevFrame >= this->_sampleBuffer.size()) {
		this->_pointers[grainIdx] = 0;
		return 0.0;
	}
	
	float fracBelow = pos - prevFrame;
	out = interpolateCubic(this->_sampleBuffer[prevFrame-1], this->_sampleBuffer[prevFrame], this->_sampleBuffer[prevFrame+1], this->_sampleBuffer[prevFrame+2], fracBelow);
	
	// Calculate and apply amplitude windowing
	float windowVal =  applyWindow(this->_pointers[grainIdx], this->_grainSizes[grainIdx]);
	out = out * .75 * windowVal;
	
	this->_pointers[grainIdx] += this->_playbackSpeed;
	//	end of grain 
	if (this->_pointers[grainIdx] > _grainSizes[grainIdx]) {
		this->_pointers[grainIdx] = 0;
		this->updateGrain(grainIdx);
	}
	
    return out;
}

float Granular::process() 
{
	float out = 0;
	
	for (int i = 0; i < this->_numGrains; i++) {
		out += this->processGrain(i);
	}
	
	return out;
}

float Granular::applyWindow(int pointer, float grainSize) {
	
	int windowType = 0; 
	if (grainSize < 1) {
		return 0;
	}

	if (windowType == 1) {
		return map((float) pointer, 0.0, grainSize, 1.0, 0.0);
	}
	
	else if (windowType == 2) {
		return map((float) pointer, 0.0, grainSize, 0.0, 1.0);
	}
	
	else {
		return sinf((M_PI*pointer)/grainSize);
	}
	
}


void Granular::setGrainSizeMS(float grainSize)
{
	_grainSize = grainSize/1000 * _sampleRate;
}

void Granular::setFilePositionMS(float filePosition)
{
	_filePosition = filePosition/1000 * _sampleRate;
}

void Granular::setPlaybackSpeed(float playbackSpeed)
{
	_playbackSpeed = playbackSpeed;
}

void Granular::setProbability(float probability) 
{
	_probability = probability*100;
}

void Granular::setFilePositionRandomness(float randomness) {
	_filePositionRandomness = randomness;
}

void Granular::setGrainSizeRandomness(float randomness) {
	_grainSizeRandomness = randomness;
}

void Granular::updateGrain(float grainIdx) {

	_grainSizes[grainIdx] = _grainSize;
	_filePositions[grainIdx] = _filePosition;
	
	// _muted[grainIdx] = rand()%100 > _probability;
	
	float random = rand()%100;
	_grainSizes[grainIdx] -= (random/100.0*_grainSizeRandomness*_grainSize);
	_filePositions[grainIdx] += (random/100.0*_filePositionRandomness*_filePosition);
}

void Granular::updateAllGrains() {
	for (int i = 0; i > _numGrains; i++) {
		updateGrain(i);
	}
}











