#pragma once

#include "ofMain.h"
#include "ModuleRunnable.h"
#include "ofxGphoto.h"

namespace ofxModule {

	/// \brief control for a Cameras supported by libgphoto
	/// \ outputes the current Picture as ofTexture via ModuleEvent 
	/// \ outputes can be controlled using ModuleEvents
	/// \ 
	/// \ Messages
	/// \ capture : true  -> camera takes photo
	/// \ setIdle : true/false -> enables disables Live-View
class ofxModuleGphoto : public ModuleRunnable{
	
  public:
	
	ofxModuleGphoto(string moduleName = "Gphoto", string settingsPath = "settings.json");
    void update();
    void stopModule();
    
protected:
    void proceedModuleEvent(ModuleEvent& e);
    
private:
	ofxGphoto::GPhoto device;
	ofPixels photoPixels;
	ofPixels liveViewPixels;

	shared_ptr<ofTexture> texPtrLiveView;
	shared_ptr <ofTexture> texPtrPhoto;

	shared_ptr<ofPixels> pixPtrLiveView;
	shared_ptr<ofPixels> pixPtrPhoto;

	bool isPhotoNew;
	bool takePhoto;

	//int width;
	//int height;
	//ofRectangle dest;

	int deviceId;
	string deviceName;

	ofPixels blackLiveView;

	string streamingMethod;
	bool isMirrorX;
	bool isMirrorY;
	
	ofFbo mirrorFbo;
	ofFbo rotateFbo;

	
};
}
//#endif
