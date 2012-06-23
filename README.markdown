# Feel Good Compliance Orb #

## Environment/Hardware Setup

* Install the latest arduino IDE from [here](http://arduino.cc/hu/Main/Software) as of writing it's 1.0.1
* Extract [this](https://github.com/jmosk/WiFi-DiamondBack-Extended-4-2012) into your libraries folder
* Go into your projects folder and clone this repo there
* Attach your [DiamondBack Arduino](http://linksprite.com/product/showproduct.php?id=74&lang=en) via USB
* Use a powered USB hub if your computer doesn't supply enough power (an MBP had this issue)
* Attach your [MegaBrite](http://docs.macetech.com/doku.php/megabrite) module to the [ShiftBrite Shield](http://docs.macetech.com/doku.php/shiftbrite_shield)
* Cut the green wire and solder/plug it into Pin 7 (current experimental strategy to avoid chatter with the built-in WiFi module)
* Upload the code and click the Serial Monitor button in the upper right of the IDE
