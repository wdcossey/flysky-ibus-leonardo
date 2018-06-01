# flysky-ibus-leonardo
Decodes FlySky IBus data directly to an Arduino Leonardo USB Game Controller.

The Arduino sketch can be found [here](/flysky-ibus-leonardo/flysky-ibus-leonardo.ino), simply build and upload it to your Leonardo.

Hardware:
* iBus compatible RC receiver.
* Arduino Leonardo/Pro Micro 5v (ATmega32u4 microcontroller).

Software:
* There's **no** need for any special software or virtual joysticks as it's all controlled by the microcontroller.

Notes:
* Connect the IBus signal wire to pin 10 on the Arduino Leonardo.
* Uses X-Axis, Y-Axis, Z-Axis, Throttle (simply [re]calibrate the controller in your sim of choice)

This project is built using Visual Studio 2017 Community and Visual Micro (Arduino extension).

![Game Controllers](/images/win10_game_controllers.png)
