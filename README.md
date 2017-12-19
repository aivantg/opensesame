# Open Sesame

I was tired of opening my dorm room door with a plain old key, so I decided to build a system to make it more interesting! In short: I upgraded my dorm room so that you can now get in through two new ways: scanning a key card on a reader outside the door or knocking a secret pattern on the door itself.

![OpenSesame Gif](https://i.imgur.com/PT4G9zO.gif)

Full Video: https://youtu.be/Dy8ZlKT2p58

## How it works

All in all, this project uses two Arduino microcontrollers to control all the parts that make it work. One is on the inside of the door controlling the motor and listening for knocks while the other is on the outside of the door, ready to scan a key card scans.If you don't know what an "Arduino" is, it's a small credit-card sized circuit board that lets you program a small processor to interact with various electrical components. It's fantastic for hobby projects like this. Learn more on [their website](http://www.arduino.cc)! Keep reading for some slightly more technical details about the project.

The circuit on the outside of the door is the simplest of all. The Arduino is mounted to the wall, powered by a battery pack, and connected to a breadboard containing two main components: an RFID scanner and a 433hz radio transmitter. The code waits for a card to be scanned, reads the UID of the card compares it to the stored and accepted IDs, and then broadcasts the result using the transmitter. The code is all in `Open_Sesame_Auth_Controller.ino`. You will also see some code relating to a button that controls the demo mode. This was so that we could send the motor controller a signal to turn on demo mode for our friends without revealing our secret knock.

Behind the door, there is another arduino system plugged into the wall that has three main components: a servo motor, a 433hz radio reciever and a piezo vibration sensor. The servo motor is mounted to the wall with a command strip and attached to the door handle through a string. When it turns a full 180°, it pulls the handle down enough that someone can simply push the door open. When the arduino recieves the right signal through the reciever, it will pull down the door. This part of the code is fairly simple; it gets significantly more complex when trying to read knocks through the piezo sensor taped onto the door itself. In short, there is a button on the breadboard that starts "recording mode" and when recording, it is listening for spikes in its readings and records the time in milliseconds between each one. When not in recording mode, it is constantly waiting to hear that same pattern of knocks within an average of 100 millisecond accuracy. The code can be found in `Open_Sesame_Motor_Controller`.

## Parts

I'm going to list out the parts I used for the project and where to find them, but I'm not necessarily listing the best place to find them. The links are just to show you what models of each product I used. Adafruit is reliable but has a shipping fee and isn't always cheap. Amazon will almost definitely be quicker to ship however it might be more expensive for certain parts. And ebay will definitely be the cheapest despite taking almost a month or more for shipping. Many will be sold in bulk for much cheaper, so make sure to look around depending on your needs!

Main Components: 
* [2x Arduino Unos](https://www.adafruit.com/product/50)
* [2x Half-Sized Breadboards](https://www.adafruit.com/product/64)
* [1x Battery Pack](https://www.adafruit.com/product/248)
* [1x RFID Reader/Card Kit](https://www.amazon.com/Gowoops-RFID-Kit-Arduino-Raspberry/dp/B01KFM0XNG/ref=sr_1_2?ie=UTF8&qid=1513653749&sr=8-2&keywords=RFID+Scanner)
* [1x Radio 433hz Transmitter/Reciever Pair](https://www.amazon.com/SMAKN%C2%AE-433Mhz-Transmitter-Receiver-Arduino/dp/B00M2CUALS/ref=sr_1_fkmr1_3?ie=UTF8&qid=1513654007&sr=8-3-fkmr1&keywords=433hz+transmitter+arduino)
* [1x Piezo Vibration Sensor](https://www.adafruit.com/product/1740)
* [1x High-Torque Servo Motor](https://www.amazon.com/Digital-Torque-Waterproof-DS3218MG-Control/dp/B076CNKQX4/ref=sr_1_15?ie=UTF8&qid=1513654229&sr=8-15&keywords=high+torque+servo+motor) (Note, this is what worked best for my door, but something else might work better for yours. Additionally, I chose this model specifically because it was able to run on a 5V power source)

Other Components: 
* Male-to-Male and Male-to-Female wires
* A few LEDs and buttons to help make the setup interactive
* Some creativity on attaching things to walls. I recommend command strips!


## How to make it yourself

If you are practiced with making Arduino projects, take a look at my how-it-works description above as well as the code and it should be relatively clear on how to start. If you're brand new with Arduino projects, consider buying a kit on amazon to get familiar and then come back and check out the project to see if you can get it working! I'm also always glad to help or answer any questions :)

## Final Notes
* On my particular door, there was a lot of noise in the vibrations that were detected on the door, so you might need to adjust some of the thresholds set in the motor controller according to your own situation
* My door was best opened using a servo motor and a string, but your own situation might not be so straightforward so be creative! That was definitely the most difficult part of this project, so don't lose faith!
* If you've never programmed an Arduino before, note that there are two integral functions: The `setup()` function will run once when the arduino is turned on and then the `loop()` function will run on repeat afterwards, which means a lot of variables have to be saved and kept track of outside of the functions and in a unique way. 

## Let me know if you have any questions!
