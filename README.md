# ofxModuleCanon

Module for [ofxModule](https://github.com/reddoLabs/ofxModule) that allows easy control over libgphoto [supported cameras](http://www.gphoto.org/proj/libgphoto2/support.php) using [ofxGphoto](https://github.com/brinoausrino/ofxGphoto).

Compatible with openFrameworks 0.11.0+ and Linux.

Generate all project files using the openFrameworks Project Generator.
Manually add linker flags `-lgphoto2` and `-lgphoto2_port`.

## Required Addons

* [ofxModule](https://github.com/reddoLabs/ofxModule)
* [ofxGphoto](https://github.com/brinoausrino/ofxGphoto)

## Getting started

Have a look at the sample project to get started directly and have a small explanation here.

## Settings

The module can be configured in the settings.json.

```json
{
    "Gphoto":{
		"deviceName": "Fujifilm XT-2",
        "mirrorX":true,
        "mirrorY":true,
        "streamingMethod": "both",
        "rotate":90
    }
}
```

| property      | type          | description  |
| ------------- |---------------| -----|
| deviceId      | int        | sets device ID (search order of gphoto, can change on restart) |
| deviceName    | string       | name of the device as shoen in device list|
| mirrorX       | boolean       | mirrors Image on x-axis |
| mirrorY       | boolean       | mirrors Image on y-axis |
| rotate        | int           | rotate the image either 0, 90, 180 or 270|
| streamingMethod | string      | transmit type of images to other modules either "texture","pixels" or "both"|

## Communication

### Control the module

Activate/ deactivate live-view:

```cpp
ofJson j;
j["value"] = false; // activate the module, for deactivate use true
notifyEvent("setIdle", j);
```

Take photo:

```cpp
notifyEvent("capture");
```

### Get messages from the module

Depending of the selected streaming method you receive the cam image as `ofTexture` or `ofPixels`. A photo arrives at the address `photo`, other messages represent the current live view.

```cpp
void Presenter::proceedModuleEvent(ModuleEvent& e) {
		
		// update the cam image 
		if (e.type == ModuleEvent::MODULE_EVENT_TEXTURE) {
			if (e.address == "photo") {
				highResTexture = e.texture;
			}else{
				lowResTexture = e.texture;
			}
			else  camTexture = e.texture;
		}else if (e.type == ModuleEvent::MODULE_EVENT_PIXELS) {
			if (e.address == "photo") {
				highResImage.setFromPixels(*e.pixels);
			} else  {
				lowResImage.setFromPixels(*e.pixels);
			}
		}
    }
```