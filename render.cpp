/*
 ____  _____ _        _    
| __ )| ____| |      / \   
|  _ \|  _| | |     / _ \  
| |_) | |___| |___ / ___ \ 
|____/|_____|_____/_/   \_\

http://bela.io

Author: Jason Hoopes

*/



#include <Bela.h>
#include <vector>
#include "granular.h"
#include <libraries/GuiController/GuiController.h>
#include <libraries/Gui/Gui.h>

// #define USE_GUI

// MASTER LEVEL
float maxAmplitude = .8;

Granular _granularInstance;

#ifdef USE_GUI
	Gui gui;
	GuiController controller;
	int grainSizeSlider;
	int filePositionSlider;
	int playbackSpeedSlider;
	int gsRandSlider;
	int fpRandSlider;
#endif


int grainSize;
int filePosition;
float playbackSpeed;
float grainSizeRandomness;
float filePositionRandomness;

 
bool setup(BelaContext *context, void *userData)
{	
	
	// Check if analog channels are enabled
	if(context->analogFrames == 0 || context->analogFrames > context->audioFrames) {
		rt_printf("Error: this example needs analog enabled, with 4 or 8 channels\n");
		return false;
	}

	
	if (!_granularInstance.setup(8, context->audioSampleRate, "./piano3.wav")) {
		rt_printf("failed to setup granular");
		return false;
	}
	
	grainSize = 715;
	filePosition = 3770;
	playbackSpeed = 1.0;
	grainSizeRandomness = .85;
	filePositionRandomness = .45;

	#ifdef USE_GUI
		gui.setup(context->projectName);
		controller.setup(&gui, "Granular Controls");
		grainSizeSlider = controller.addSlider("Grain Size (MS)", grainSize, 0.0, 1000.0, 1.0);
		filePositionSlider = controller.addSlider("File Position (MS)", filePosition, 0.0, 5000.0, 1.0);
		playbackSpeedSlider = controller.addSlider("Playback Speed (MS)", playbackSpeed, -2.0, 2.0, .001);
		gsRandSlider = controller.addSlider("Grain Randomness", grainSizeRandomness, 0.0, 1.0, .001);
		fpRandSlider = controller.addSlider("File Position Randomness", filePositionRandomness, 0.0, 1.0, .001);
	#endif

	return true;
	
}



void render(BelaContext *context, void *userData)
{

	#ifdef USE_GUI
		grainSize = controller.getSliderValue(grainSizeSlider);
		filePosition = controller.getSliderValue(filePositionSlider);
		playbackSpeed = controller.getSliderValue(playbackSpeedSlider);
		grainSizeRandomness = controller.getSliderValue(gsRandSlider);
		filePositionRandomness = controller.getSliderValue(fpRandSlider);
	#endif
	
    for(unsigned int n = 0; n < context->audioFrames; n++) {
    	
    	
    	_granularInstance.setGrainSizeMS(grainSize);
    	_granularInstance.setFilePositionMS(filePosition);
    	_granularInstance.setPlaybackSpeed(playbackSpeed);
    	_granularInstance.setGrainSizeRandomness(grainSizeRandomness);
    	_granularInstance.setFilePositionRandomness(filePositionRandomness);
    	_granularInstance.setProbability(0.5);

		

		float out = _granularInstance.process();

		for(unsigned int channel = 0; channel < context->audioOutChannels; channel++) {
			audioWrite(context, n, channel, out);
		}
  }
}





void cleanup(BelaContext *Process, void *userData)
{

}
