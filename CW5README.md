/* MODULE 4B25 - COURSEWORK 5 */

#name: Elliot Krishek
#college: St John's
#CRSid: EJK52

#		Pedometer and Pace Tracker
A README.md summarizing the project and describing the layout of the
repository/archive. 

/* Summary */

The 'Pedometer and Pace Tracker' project is one way of realising a pedometer and additional features through a microcontroller with only a 3-axis accelerometer, and display. The 3-axis accelerometer commmunicates through an I2C interface, whilst the display communicates through an SPI interface.

Overall, when used in the intended fashion, the 'Pedometer and Pace Tracker' will be able to dynamically(1) display the total number of steps you have taken, the average steps per minute, an estimate for speed (km/h), as well as the user inputted height (cm) which is used to calculate the speed estimate.

The 'Pedometer and Pace Tracker' works best when in a square orientation (i.e. one axis points directly up), and with an activity such as running, or jogging on the spot(2), or walking.

/* File structure */

The code used in this project was initially forked from the Warp firmware. Therefore, I will only highlight the files that have been either modified or created during the whole process.

# modified files
/* slashes have been added before file names to help them stand out from the comments */

/build.sh					(adding the new created files to the build path, and commented out other driver files {i.e. devBMX055.h} from the build path)
/CMakeLists.txt				(adding the new created files to the build path)
/devSSD1331.c				(additional code at the end to turn the screen green from 
coursework item 2)
/gpio_pins.c				(remapped two GPIO pins to allow buttons SW2 and SW3 input)
/gpio_pins.h				(renamed the two GPIO pins to make more sense in their new purpose)
/warp-kl03-ksdk1.1-boot.c	(added the main cw5.c function for the code to run, and removed the warp menu loop. Additionally commnted out any includes of additional driver files {i.e. devBMX055.h})

/devMMA8451Q.c & /warp.h 	(modified in earlier courseworks)

# created files
/* all created files in directory Warp-firmware/src/boot/ksdk1.1.0 */

/cw5.c						(the code I wrote for this project)
/cw5.h						(header file for the functions used in cw5.c)

# user guide

Upon startup, the screen will turn on green, then peach. After this the number 175 will appear in the top right of the screen (indicating user height in cm), along with a green bar accross the bottom of the screen that indicates how long you have left to input your height (~8.5 seconds). The height value can be changed using the buttons marked SW2 and SW3, which are next to each other on the FRDM board. Upon changing the height value, the time left to input the height will be reset. Furthermore, when holding down the a button to increase or decrease the height value, the delay between each increment or decrement will decrease - again this delay will reset to the longest delay when the button is no longer pressed.

After height has been selected, the center portion of the screen will go red - indicating that you can start the main pedometer loop. This can be initiated by simply pressing either of the 2 buttons used previously (but not both at the same time). Upon starting the pedometer loop, the center portion of the screen will turn from red to green.

After some time has passed (~18 seconds) the top left portion of the screen will display the total number of steps taken so far, and the bottom right portion will display the estimated speed (based on the users height and steps per 2 seconds).

To stop the pedometer, simply press both buttons together (SW2 and SW3 at the same time) for 2 seconds. This action will be successful if the center portion of the screen turns from green to cyan/light blue. Upon this action, the total number of steps will be updated (top left of the screen), the speed will also update to show the average speed for the entire session (time that the pedometer was in use) (bottom right of the screen). Furthermore, the bottom left portion of the screen will display the average steps per minute from the entire session.

To restart/reuse the pedometer, simply press the reset button - marked RST/SW1.
