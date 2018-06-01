# flysky-ibus-leonardo
Decodes FlySky IBus data directly to an Arduino Leonardo USB Game Controller.

The Arduino sketch can be found [here](/flysky-ibus-leonardo.ino), simply build and upload it to your Leonardo.

Hardware:
* iBus compatible RC receiver.
* Arduino Leonardo/Pro Micro 5v (ATmega32u4 microcontroller).

Software:
* There's **no** need for any special software or virtual joysticks as it's all controlled by the microcontroller.

Channel Mapping:

Source | Target
------------ | -------------
Channel 1 | X Axis
Channel 2 | Y Axis
Channel 3 | X Rotation
Channel 4 | Y Rotation
Channel 5 | Z Axis
Channel 6 | Z Rotation

Notes:
* To build and Upload this sketch use the [Joystick](https://github.com/MHeironimus/ArduinoJoystickLibrary) library by [Matthew Heironimus](https://github.com/MHeironimus).
* Connect the IBus signal wire to pin 10 on the Arduino Leonardo.
* Uses X-Axis, Y-Axis, Z-Axis, Rx-Axis, Ry-Axis, Rz-Axis (simply [re]calibrate the controller in your sim of choice).

![Game Controllers](/images/win10_game_controllers.png)

![Sample](/images/win10_sample.gif)

Screen recording by [ScreenToGif](https://github.com/NickeManarin/ScreenToGif)
