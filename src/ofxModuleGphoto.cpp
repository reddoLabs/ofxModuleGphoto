#include "ofxModuleGphoto.h"

namespace ofxModule {
	//------------------------------------------------------------------
	ofxModuleGphoto::ofxModuleGphoto(string moduleName,string settingsPath) :ModuleRunnable("Gphoto", moduleName, settingsPath) {

		//setup cam
		isMirrorX = settings["mirrorX"] == nullptr ? false : settings["mirrorX"].get<bool>();
		isMirrorY = settings["mirrorY"] == nullptr ? false : settings["mirrorY"].get<bool>();
		streamingMethod = settings["streamingMethod"] == nullptr || settings["streamingMethod"] == "" ? "texture" : settings["streamingMethod"];
		int deviceId = settings["deviceId"] != nullptr ? settings["deviceId"].get<int>()  : 0;
		string deviceName = settings["deviceName"] != nullptr ? settings["deviceName"] : "";

		//width = settings["width"] == nullptr ? 0 : settings["width"];
		//height = settings["height"] == nullptr ? 0 : settings["height"];

		//dest = ofRectangle(0, 0, width, height);


		auto li = ofxGphoto::GPhoto::listDevices();
				if (deviceName != "") {
					for (auto& device : li) {
						if (device.name == deviceName) deviceId = device.id;
					}
				}
		device.setup(deviceId);

		isPhotoNew = false;
		takePhoto = false;

		texPtrLiveView = shared_ptr<ofTexture>(new ofTexture());
		texPtrPhoto = shared_ptr<ofTexture>(new ofTexture());

		pixPtrLiveView = shared_ptr<ofPixels>(&liveViewPixels);
		pixPtrPhoto = shared_ptr<ofPixels>(&photoPixels);

		//lastActivation = ofGetElapsedTimeMillis();

	}

	//------------------------------------------------------------------
	void ofxModuleGphoto::update() {

		if (!isIdle && device.isConnected()) {

			device.update();

			if (takePhoto && device.isPhotoNew())
			{
				photoPixels = device.getPhotoPixels();
					if (isMirrorX || isMirrorY) {
						photoPixels.mirror(isMirrorY, isMirrorX);
					}
					if (settings["rotate"] == 90) {
						photoPixels.rotate90(1);
					} else if (settings["rotate"] == 180) {
						photoPixels.rotate90(2);
					} else if (settings["rotate"] == 270) {
						photoPixels.rotate90(3);
					}
					/*if (width != 0 && height != 0) {
						ofRectangle t = dest;
						ofRectangle src = ofRectangle(0, 0, liveViewPixels.getWidth(), liveViewPixels.getHeight());
						t.scaleTo(src, OF_SCALEMODE_FIT);
						photoPixels.crop(t.x, t.y, t.width, t.height);
						photoPixels.resize(width, height);
					}*/
					if (streamingMethod == "texture" || streamingMethod == "both") {
						this->texPtrPhoto->loadData(photoPixels);
					}


				takePhoto = false;
				isPhotoNew = true;
			}

			//update live view
			if (!isPhotoNew)
			{
				if (device.isLiveView()) {
					liveViewPixels = device.getLivePixels();
					if(liveViewPixels.size() > 0){
						if (isMirrorX || isMirrorY) {
							liveViewPixels.mirror(isMirrorY, isMirrorX);
						}
						if (settings["rotate"] == 90) {
							liveViewPixels.rotate90(1);
						} else if (settings["rotate"] == 180) {
							liveViewPixels.rotate90(2);
						} else if (settings["rotate"] == 270) {
							liveViewPixels.rotate90(3);
						}
						//if (width != 0 && height != 0) {
						//	ofRectangle t = dest;
						//	ofRectangle src = ofRectangle(0, 0, liveViewPixels.getWidth(), liveViewPixels.getHeight());
						//	t.scaleTo(src, OF_SCALEMODE_FIT);
						//	liveViewPixels.crop(t.x, t.y, t.width, t.height);
						//	liveViewPixels.resize(width, height);
						//}
						this->texPtrLiveView->loadData(liveViewPixels);
						//use a black preview to prevent a not current image
						if (!blackLiveView.isAllocated()) {
							ofFbo fbo;
							fbo.allocate(liveViewPixels.getWidth(), liveViewPixels.getHeight());
							fbo.begin();
							ofClear(0);
							fbo.end();
							fbo.readToPixels(blackLiveView);
						}
					}

				}
				/*else if (ofGetElapsedTimeMillis() - lastActivation < 100 && blackLiveView.isAllocated()) {
					liveViewPixels = blackLiveView;
					texPtrLiveView->loadData(blackLiveView);

				}*/
			}


			if (isPhotoNew) {
				if (streamingMethod == "texture" || streamingMethod == "both") {
					notifyEvent(texPtrPhoto, "photo");
				}
				if (streamingMethod == "pixels" || streamingMethod == "both") {
					notifyEvent(pixPtrPhoto, "photo");
				}
				//logNotice("draw", "high resolution picture sent");
				isPhotoNew = false;
			}
			else if(texPtrLiveView->isAllocated()){
				if (streamingMethod == "texture" || streamingMethod == "both") {
					notifyEvent(texPtrLiveView);
				}
				if (streamingMethod == "pixels" || streamingMethod == "both") {
					notifyEvent(pixPtrLiveView);
				}

			}
		}
	}

	//------------------------------------------------------------------
	void ofxModuleGphoto::stopModule() {
		device.close();
	}

	// received module events will be processed here
	void ofxModuleGphoto::proceedModuleEvent(ModuleEvent & e)
	{
		if (e.address == "capture") {
			device.takePhoto();
			takePhoto = true;
		}
		else if (e.address == "setIdle") {
			if (e.message["value"].get<bool>() == true && device.isLiveView()) {
				device.setLiveView(false);
				ofLogNotice("proceedModuleEvent", "closing live view");
			}
			else if (e.message["value"].get<bool>() == false && !device.isLiveView())
			{
				device.setLiveView(true);
				ofLogNotice("proceedModuleEvent", "opening live view");
			}
		}
	}
}
