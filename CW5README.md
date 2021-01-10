# MODULE 4B25 - COURSEWORK 5

# name:		Elliot Krishek
# college:	St John's
# CRSid:	EJK52

# Pedometer and Pace Tracker

The 'Pedometer and Pace Tracker' project is one way of realising a pedometer and additional features through a microcontroller with only a 3-axis accelerometer, and display. The 3-axis accelerometer commmunicates through an I2C interface, whilst the display communicates through an SPI interface.

Overall, when used in the intended fashion, the 'Pedometer and Pace Tracker' will be able to dynamically display the total number of steps you have taken, the average steps per minute, an estimate for speed (km/h), as well as the user inputted height (cm) which is used to calculate the speed estimate.

The 'Pedometer and Pace Tracker' works best when in a square orientation (i.e. one axis points directly up), and with an activity such as running, or jogging on the spot, or walking.

# File structure

The code used in this project was initially forked from the Warp firmware. Therefore, I will only highlight the files that have been either modified or created during the whole process.

# Modified files

build.sh					(adding the new created files to the build path, and commented out other driver files {i.e. devBMX055.h} from the build path)

CMakeLists.txt				(adding the new created files to the build path)

devSSD1331.c				(additional code at the end to turn the screen green from coursework item 2)

gpio_pins.c				(remapped two GPIO pins to allow buttons SW2 and SW3 input)

gpio_pins.h				(renamed the two GPIO pins to make more sense in their new purpose)

warp-kl03-ksdk1.1-boot.c	(added the main cw5.c function for the code to run, and removed the warp menu loop. Additionally commnted out any includes of additional driver files {i.e. devBMX055.h})

devMMA8451Q.c & warp.h 	(modified in earlier courseworks)

# created files
all created files in directory Warp-firmware/src/boot/ksdk1.1.0

cw5.c						(the code I wrote for this project)

cw5.h						(header file for the functions used in cw5.c)

# Software explanations

In this project, 14 functions have been created and used. Within the main function in warp-kl03-ksdk1-boot.c the only function we need to call is 'cw5Code'; the 13 other functions are all static (i.e. can only be used within cw5.c) and are called from other functions within cw5.c .

The first function to note is 'setup', which runs all initialisations required for the display, and configures the accelerometer. 'SW2', and 'SW3' are simply in place to check whether the 2 buttons that are used as inputs to the system have been pressed. It should be noted that checking the status of the buttons can be done in a single line without these functions, but they make the code more readable.

'maxAxSel' is the function that takes the accelerometer data and determines which of the 3 axis has the most activity, and returns the threshold value for that axis, as well as modulating/encoding the number to determine the axis that threshold belongs to later.

'crossing' is the function where we use that threshold along with the data from the selected axis to determine how many times the data crossed that threshold - and thus determine the number of steps taken in the sampled time period.

'getAccelData' is the function that is called from 'cw5Code' actually reads the accelerometer data, and makes use of the previous 2 functions in order to return the step count for the 2 second period that has just been sampled.

'speed' is a function that take a number of steps, the amount of sampling periods those steps were performed in, and the users height, to give a conversion into a speed in km/h. This is only an estimated speed value.

Throughout the code, there are many instances of drawing lines and rectangles onto the display. Therefore, 'colour' has been used in order to save space, as it converts 3 lines of setting an RGB value into a 1 line command.

'numbers' is a function that can draw out a number 0-9, or a decimal point, and display it relative to the top left corner of the screen.

'display_numbers' is the function that takes the general location for a number (i.e. top left or bottom right), and process the number into its digits, before displaying them in their designated areas. This is one of the functions called directly from 'cw5Code'

'loadingBar' is a function that displays a bar across the bottom portion of the screen. This is only used when the user selects their height, and indicates how long is left by decreasing the length of the bar over time.

'setHeight' is a function created to tidy up 'cw5Code'. It takes input from the physical SW2&3 buttons through 'SW2' and 'SW3', and displays the current height selected and time left (throught the loading bar).

'centerSquare' is also a function which tidys up 'cw5Code', which simply sets a center portion of the display to a chosen colour. This colour will help to indicate what actions should/can be performed.

Finally, we have 'cw5Code', which combines all of the previous functions to give the Pedometer and Pace Tracker features and usability as described in the user guide below. It should be noted that this code does not loop automatically. If looping was desired, it would be a simple addition to make.

# Hardware guide

To make the Pedometer and Pace Tracker, an FRDM KL03 board, a display using the ssd1331 driver chip (such as the adafruit OLED breakout board [product ID: 684] used in this project), and 7 wires.

wiring diagram:

FRDM J2 pin 1 -> R (reset pin)

FRDM J3 pin 5 -> + (supply voltage)

FRDM J3 pin 7 -> G (ground)

FRDM J4 pin 1 -> CK (clock input)

FRDM J4 pin 2 -> SI (?)

FRDM J4 pin 3 -> DC (?)

FRDM J4 pin 5 -> OC (?)

# User guide

The Pedometer and Pace Tracker should first be places on the waist/hip, with a major axis pointing directly up/down. This can be achieved by having the display be upright while wearing.

Upon startup, the screen will turn on green, then peach. After this the number 175 will appear in the top right of the screen (indicating user height in cm), along with a green bar accross the bottom of the screen that indicates how long you have left to input your height (~8.5 seconds). The height value can be changed using the buttons marked SW2 and SW3, which are next to each other on the FRDM board. Upon changing the height value, the time left to input the height will be reset. Furthermore, when holding down the a button to increase or decrease the height value, the delay between each increment or decrement will decrease - again this delay will reset to the longest delay when the button is no longer pressed.

After height has been selected, the center portion of the screen will go red - indicating that you can start the main pedometer loop and your chosen activity, such as running, jogging on the spot, or walking. This can be initiated by simply pressing either of the 2 buttons used previously (but not both at the same time). Upon starting the pedometer loop, the center portion of the screen will turn from red to green.

After some time has passed (~20 seconds) the top left portion of the screen will display the total number of steps taken so far, and the bottom right portion will display the estimated speed (based on the users height and steps per 2 seconds).

To stop the pedometer, simply press both buttons together (SW2 and SW3 at the same time) for 2 seconds. This action will be successful if the center portion of the screen turns from green to cyan/light blue. Upon this action, the total number of steps will be updated (top left of the screen), the speed will also update to show the average speed for the entire session (time that the pedometer was in use) (bottom right of the screen). Furthermore, the bottom left portion of the screen will display the average steps per minute from the entire session.

To restart/reuse the pedometer, simply press the reset button - marked RST/SW1.
