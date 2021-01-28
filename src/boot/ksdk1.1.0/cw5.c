#include <stdlib.h>

#include "fsl_misc_utilities.h"
#include "fsl_device_registers.h"
#include "fsl_i2c_master_driver.h"
#include "fsl_spi_master_driver.h"
#include "fsl_rtc_driver.h"
#include "fsl_clock_manager.h"
#include "fsl_power_manager.h"
#include "fsl_mcglite_hal.h"
#include "fsl_port_hal.h"

#include "gpio_pins.h"
#include "SEGGER_RTT.h"
#include "warp.h"

#include "devMMA8451Q.h"
#include "devSSD1331.h"

extern volatile WarpI2CDeviceState      deviceMMA8451QState;

static uint32_t
SW2()
{
	/* SW2() will return the state of the button marked SW2 on the FRDM board
	 * returns 0 when pressed, and 1 when not
	 */
        static uint32_t press = 1;
	press = GPIO_DRV_ReadPinInput(kWarpPinSW2);
        return press;
}

static uint32_t
SW3()
{
	/* SW3() will return the state of the button marked SW3 on the FRDM board
	 * returns 0 when pressed, and 1 when not
	 */
	static uint32_t press = 1;
	press = GPIO_DRV_ReadPinInput(kWarpPinSW3);
	return press;
}

static void
colour(uint8_t RGB[3])
{
	/* a space saving function that takes an RGB value to write to the OLED board whenever a line or rectangle is drawn
	 * as we write to the OLED board so many times, and the colour commands don't change, we can save possibly 100 s of lines for readability
	 */
	writeCommand(RGB[0]);		//colour intensity (red)
	writeCommand(RGB[1]);		//colour intensity (green)
	writeCommand(RGB[2]);		//colour intensity (blue)
}

static void
loadingBar(uint8_t length, uint8_t barCol[3], uint8_t BGCol[3])
{
	/* loadingBar displays a bar accross a bottom part of the screen
	 * used in conjunction with setHeight() to give a visual of how much time is left to perform a certain action - like inputting your height
	 */
	writeCommand(kSSD1331CommandDRAWRECT);
	writeCommand(0);
	writeCommand(50);
	writeCommand(96);
	writeCommand(60);
	colour(BGCol);
	colour(BGCol);

	writeCommand(kSSD1331CommandDRAWRECT);
	writeCommand(0);
	writeCommand(50);
	writeCommand(length * 4);
	writeCommand(60);
	colour(barCol);
	colour(barCol);
}

static void
centerSquare(uint8_t sqrCol[3])
{
	/* centerSquare() simply sets the colour of the center of the screen - coincidentally this is not actually a square
	 * as the center colour is changed, this function helps to keep the 'main' code function tidy
	 */
	writeCommand(kSSD1331CommandDRAWRECT);
       	writeCommand(36);
	writeCommand(24);
	writeCommand(60);
	writeCommand(40);
	colour(sqrCol);
	colour(sqrCol);
}

static void
numbers(uint8_t row, uint8_t column, uint8_t number, uint8_t col[3])
{
	/* numbers() is the function that will actually display any number we want on the screen, as well as a decimal point
	 * this works by drawing out lines in a similar fashion to a 7-segment display, with numbers >1 being constant width
	 * the decimal point and number 1 are of width 1
	 * if the screen had a much smaller pixel size, then this function would not work so well - as the lines would be too small to see, but on the OLED screen used in the project the lines are more than adequate
	 */
	uint8_t H = 15;
	uint8_t W = 8;
	uint8_t Ha = 7;
	switch(number)
	{
		default:
			break;
		case 0:
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column);
			writeCommand(row);
			writeCommand(column);
			writeCommand(row + H);
			colour(col);
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column);
			writeCommand(row + H);
			writeCommand(column + W);
			writeCommand(row + H);
			colour(col);
			writeCommand(kSSD1331CommandDRAWLINE);
                        writeCommand(column + W);
			writeCommand(row + H);
			writeCommand(column + W);
			writeCommand(row);
			colour(col);
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column + W);
			writeCommand(row);
			writeCommand(column);
			writeCommand(row);
			colour(col);
			break;
		case 1:
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column);           //column address of start
			writeCommand(row);              //row address of start
			writeCommand(column);           //collumn address of end
			writeCommand(row + H);         //row address of end
			colour(col);
			break;
		case 2:
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column + W);
			writeCommand(row + H);
			writeCommand(column);
			writeCommand(row + H);
			colour(col);
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column);
			writeCommand(row + H);
			writeCommand(column);
			writeCommand(row + Ha);
			colour(col);
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column);
			writeCommand(row + Ha);
			writeCommand(column + W);
			writeCommand(row + Ha);
			colour(col);
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column + W);
			writeCommand(row + Ha);
			writeCommand(column + W);
			writeCommand(row);
			colour(col);
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column + W);
			writeCommand(row);
			writeCommand(column);
			writeCommand(row);
			colour(col);
			break;
		case 3:
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column);
			writeCommand(row);
			writeCommand(column + W);
			writeCommand(row);
			colour(col);
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column + W);
			writeCommand(row);
			writeCommand(column + W);
			writeCommand(row + H);
			colour(col);
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column + W);
			writeCommand(row + H);
			writeCommand(column);
			writeCommand(row + H);
			colour(col);
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column + W);
			writeCommand(row + Ha);
			writeCommand(column);
			writeCommand(row + Ha);
			colour(col);
			break;
		case 4:
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column);
			writeCommand(row);
			writeCommand(column);
			writeCommand(row + Ha);
			colour(col);
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column);
			writeCommand(row + Ha);
			writeCommand(column + W);
			writeCommand(row + Ha);
			colour(col);
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column + W);
			writeCommand(row);
			writeCommand(column + W);
			writeCommand(row + H);
			colour(col);
			break;
		case 5:
			writeCommand(kSSD1331CommandDRAWLINE);
                        writeCommand(column);
			writeCommand(row + H);
			writeCommand(column + W);
			writeCommand(row + H);
			colour(col);
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column + W);
			writeCommand(row + H);
			writeCommand(column + W);
			writeCommand(row + Ha);
			colour(col);
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column + W);
			writeCommand(row + Ha);
			writeCommand(column);
			writeCommand(row + Ha);
			colour(col);
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column);
			writeCommand(row + Ha);
			writeCommand(column);
			writeCommand(row);
			colour(col);
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column);
			writeCommand(row);
			writeCommand(column + W);
			writeCommand(row);
			colour(col);
			break;
		case 6:
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column + W);
			writeCommand(row);
			writeCommand(column);
			writeCommand(row);
			colour(col);
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column);
			writeCommand(row);
			writeCommand(column);
			writeCommand(row + H);
			colour(col);
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column);
			writeCommand(row + H);
			writeCommand(column + W);
			writeCommand(row + H);
			colour(col);
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column + W);
			writeCommand(row + H);
			writeCommand(column + W);
			writeCommand(row + Ha);
			colour(col);
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column + W);
			writeCommand(row + Ha);
			writeCommand(column);
			writeCommand(row + Ha);
			colour(col);
			break;
		case 7:
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column);
			writeCommand(row);
			writeCommand(column + W);
			writeCommand(row);
			colour(col);
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column + W);
			writeCommand(row);
			writeCommand(column + W);
			writeCommand(row + H);
			colour(col);
			break;
		case 8:
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column);
			writeCommand(row);
			writeCommand(column);
			writeCommand(row + H);
			colour(col);
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column);
			writeCommand(row + H);
			writeCommand(column + W);
			writeCommand(row + H);
			colour(col);
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column + W);
			writeCommand(row + H);
			writeCommand(column + W);
			writeCommand(row);
			colour(col);
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column + W);
			writeCommand(row);
			writeCommand(column);
			writeCommand(row);
			colour(col);
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column);
			writeCommand(row + Ha);
			writeCommand(column + W);
			writeCommand(row + Ha);
			colour(col);
			break;
		case 9:
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column);
			writeCommand(row);
			writeCommand(column + W);
			writeCommand(row);
			colour(col);
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column + W);
			writeCommand(row);
			writeCommand(column + W);
			writeCommand(row + H);
			colour(col);
		    	writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column);
			writeCommand(row);
			writeCommand(column);
			writeCommand(row + Ha);
			colour(col);
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column);
			writeCommand(row + Ha);
			writeCommand(column + W);
			writeCommand(row + Ha);
			colour(col);	
			break;
		case 10: // dot
			writeCommand(kSSD1331CommandDRAWLINE);
			writeCommand(column);
			writeCommand(row + H);
			writeCommand(column);
			writeCommand(row + H);
			colour(col);
			break;
	}
}

static void
display_numbers(int number, char location, uint8_t digiCol[3], uint8_t BGC[3])
{
	/* display_numbers() is the only function or variable not named with CamelCase
	 * this is the function that will take an integer number, and the location for that number (predefined, and a choice of the 4 preset locations)
	 * and splits the integer up into its digits for displaying.
	 * note that only 5 digit numbers will possibly be displayed. I have made this choice because I am not expecting to reach more than a 5 digit step count or any other number.
	 * it would be simply a matter of changing the 5 in digits to be a larger number to handle numbers 100,000 or greater
	 */
	uint8_t 	digit;
	uint8_t		digits[5] = {0};
	uint8_t		i = 0;
	uint8_t		screenLength = 96;
	uint8_t		screenHeight = 64;
	uint8_t		startCol = 0;
	uint8_t		startRow = 0;
	bool		backwards = false;
	uint8_t		width = 8;
	uint8_t		height = 15;
	uint8_t		margin = 5;
	bool		zeroFlag = false;
	while(number > 0)
	{
		digit = number % 10;
		digits[i] = digit;
		i += 1;
		number /= 10;
	}
	switch(location)
	{
		default:
			break;
		case 'h':
			writeCommand(kSSD1331CommandDRAWRECT);
			writeCommand(50);	//column address of start
			writeCommand(screenHeight - margin + 1);	//row address of start
			writeCommand(screenLength - margin + 1);	//collumn address of end
			writeCommand(margin + height + 1);	//row address of end
			colour(BGC);
			colour(BGC);

			startCol = screenLength - margin;
			startRow = margin;
			backwards = false;
			break;
		case 's':
			writeCommand(kSSD1331CommandDRAWRECT);
			writeCommand(margin - 1);       //column address of start
			writeCommand(margin - 1);        //row address of start
			writeCommand(margin + 45);       //collumn address of end
			writeCommand(margin + height + 1);       //row address of end
			colour(BGC);
			colour(BGC);

			startCol = margin;
			startRow = margin;
			backwards = true;
			break;
		case 'a':
			writeCommand(kSSD1331CommandDRAWRECT);
			writeCommand(margin - 1);       //column address of start
			writeCommand(screenHeight - margin - height - 1);        //row address of start
			writeCommand(margin + 45);       //collumn address of end
			writeCommand(screenHeight - margin + 1);       //row address of end
			colour(BGC);
			colour(BGC);

			startCol = margin;
			startRow = screenHeight - margin - height;
			backwards = true;
			break;
		case 'v':
			writeCommand(kSSD1331CommandDRAWRECT);
			writeCommand(50);       //column address of start
			writeCommand(screenHeight - margin - height - 1);        //row address of start
			writeCommand(screenLength - margin + 1);       //collumn address of end
			writeCommand(screenHeight - margin + 1);       //row address of end
			colour(BGC);
			colour(BGC);

			startCol = screenLength - margin;
			startRow = screenHeight - margin - height;
			backwards = false;
			break;
	}
	if(location == 'v')
	{
		digits[4] = digits[3];
		digits[3] = digits[2];
		digits[2] = 10;
	}
	
	if(digits[4] == 0){zeroFlag = true;}
	for(uint8_t j = 0; j < 5; j++)
	{
		if(!backwards)
		{
			if(j > 2 && digits[j] == 0){break;}
			if(digits[j] == 1 || digits[j] == 10){startCol -= 1;}
			else{startCol -= width;}
			numbers(startRow, startCol, digits[j], digiCol);
			startCol -= 2;
//			OSA_TimeDelay(10);
		}
		else
		{
			if(zeroFlag && digits[4 - j] == 0){}
			else
			{
				zeroFlag = false;
				numbers(startRow, startCol, digits[4 - j], digiCol);
				if(digits[4 - j] == 1 || digits[4 - j] == 10){startCol += 2;}
				else{startCol += width + 2;}
//				OSA_TimeDelay(10);
			}
		}
	}
}

static void
setup()
{
	/* setup() is a function to be called at the start of the 'main' coursework 5 code
	 * this will do as the name suggests and perform the necessary on power on configurations, for both the OLED screen and accelerometer
	 * it should be noted that the accelerometer is being used in the +-2g range, as my testing with running (which has the largest accelerations) has shown that it is not necessary to increase this range
	 */
	uint16_t        menuI2cPullupValue = 32768;
	
	devSSD1331init();

//      SEGGER_RTT_WriteString(0, "\r\n\tEnabling I2C pins...\n");
	enableI2Cpins(menuI2cPullupValue);

	configureSensorMMA8451Q(0x00,/* Payload: Disable FIFO */
				0x01,/* Normal read 8bit, 800Hz, normal, active mode */
				menuI2cPullupValue
				);

	writeSensorRegisterMMA8451Q(0x2A, 0, menuI2cPullupValue);       //enter standby mode
	writeSensorRegisterMMA8451Q(0x0E, 0, menuI2cPullupValue);       //bit 4 sets high pass filtering (1 for yes), bit 1 and bit 0 set scale, so 00 is +-2g, 01 is 4, 11 is 8
	writeSensorRegisterMMA8451Q(0x2A, 1, menuI2cPullupValue);       //exit standby mode, enter active mode
}

static double
maxAxSel(int16_t Xs[40], int16_t Ys[40], int16_t Zs[40])
{
	/* maxAxSel() determines which of the three axis has the most activity (by obtaining the range of accleration values)
	 * the value we return from this is then the threshold value we will use later to determine whether or not a step has been performed
	 * this threshold value is also 'encoded' or 'modulated' by a decimal value that we will associate with an axis - 0.1 for the x, 0.2 for y, and 0.3 for z
	 * this 'encoding' is only necessary as we cannot return more than one value in c. Therefore, if this was in a different language - such as Python, this would not be necessary.
	 */
	int16_t		xL;
	int16_t		xSmall;
	int16_t		xRange;
	int16_t         yL;
        int16_t         ySmall;
	int16_t         yRange;
	int16_t         zL;
        int16_t         zSmall;
	int16_t         zRange;
	xL = Xs[0];
	xSmall = xL;
	yL = Ys[0];
	ySmall = yL;
	zL = Zs[0];
	zSmall = zL;

	for(int n = 0; n < 40; n++)
	{
		if(Xs[n] > xL) {xL = Xs[n];}
		if(Xs[n] < xSmall) {xSmall = Xs[n];}
		if(Ys[n] > yL) {yL = Ys[n];}
		if(Ys[n] < ySmall) {ySmall = Ys[n];}
		if(Zs[n] > zL) {zL = Zs[n];}
		if(Zs[n] < zSmall) {zSmall = Zs[n];}
	}
	xRange = xL - xSmall;
	yRange = yL - ySmall;
	zRange = zL - zSmall;
	if((xRange < 2000 ) && (yRange < 2000) && (zRange < 2000)){return 1.0;}
	if( (xRange >= yRange) && (xRange >= zRange) ) {return (((xL + xSmall)/2) + 0.1);}
	if( (yRange >= xRange) && (yRange >= zRange) ) {return (((yL + ySmall)/2) + 0.2);}
	if( (zRange >= yRange) && (zRange >= xRange) ) {return (((zL + zSmall)/2) + 0.3);}
	
	return 0.0;
}

static uint8_t
crossing(int16_t data[40], int threshold)
{
	/* crossing() is the function where we actually determine whether or not we have crossed over the threshold value, and therefore have made a step
	 * it should be noted that this currently only counts a step on the falling edge. This is an arbitrary choice, and we could look for rising edge crosses instead.
	 */
	uint8_t 	crosses = 0;
	
	for(int i = 1; i<40; i++)
	{
		if((data[i] < data[i-1]) && (data[i] < threshold) && (data[i-1] > threshold)) { crosses += 1; }
	}
	
	return crosses;
}

static uint8_t
getAccelData(bool print)
{
	/* getAccelData() is arguably the most important function, as this is where the accleration values are actually obtained and all data handling happens here
	 * 40 values are obtained at 50 millisecond intervals, to get 2 seconds worth of data. The last data points (i.e. the 25th value) do not have any delay after them so that the next set of data can be obained at the beginning of the next 2 second period
	 */
	uint16_t        LSB;
	uint16_t        MSB;
	int16_t         combinedValue;
	WarpStatus      i2cReadStatus;
	int		DLength = 40; // max value seems to be 50 or just above
	int16_t		xData[DLength];
	int16_t         yData[DLength];
	int16_t         zData[DLength];
	int		delay = 50;
	uint32_t	startTime;
	double		maxAx;
	uint8_t		crossings = 0;

	for(int i=0; i<DLength; i++)
	{
	startTime = OSA_TimeGetMsec();
//	SEGGER_RTT_printf(0, "time = %d \n", startTime);
	
	i2cReadStatus = readSensorRegisterMMA8451Q(kWarpSensorOutputRegisterMMA8451QOUT_X_MSB,2);

	if(i2cReadStatus != kWarpStatusOK)
	{
		SEGGER_RTT_WriteString(0, "\n error reading X register \n");
		xData[i] = 0;
	}
	else
	{
		MSB = deviceMMA8451QState.i2cBuffer[0];
		LSB = deviceMMA8451QState.i2cBuffer[1];
		combinedValue = ((MSB & 0XFF) << 6) | (LSB >> 2);
		combinedValue = (combinedValue ^ (1 << 13)) - (1 << 13);
//		SEGGER_RTT_printf(0, "\nReading from sensor X: %d", combinedValue);
		xData[i] = combinedValue;
	}

	i2cReadStatus = readSensorRegisterMMA8451Q(kWarpSensorOutputRegisterMMA8451QOUT_Y_MSB,2);
	if(i2cReadStatus != kWarpStatusOK)
	{
		SEGGER_RTT_WriteString(0, "\n error reading Y register \n");
		yData[i] = 0;
	}
	else
	{
		MSB = deviceMMA8451QState.i2cBuffer[0];
		LSB = deviceMMA8451QState.i2cBuffer[1];
		combinedValue = ((MSB & 0XFF) << 6) | (LSB >> 2);
		combinedValue = (combinedValue ^ (1 << 13)) - (1 << 13);
//		SEGGER_RTT_printf(0, "\nReading from sensor Y: %d", combinedValue);
		yData[i] = combinedValue;
	}

	i2cReadStatus = readSensorRegisterMMA8451Q(kWarpSensorOutputRegisterMMA8451QOUT_Z_MSB,2);
	if(i2cReadStatus != kWarpStatusOK)
	{
		SEGGER_RTT_WriteString(0, "\n error reading Z register \n");
		zData[i] = 0;
	}
	else
	{
		MSB = deviceMMA8451QState.i2cBuffer[0];
		LSB = deviceMMA8451QState.i2cBuffer[1];
		combinedValue = ((MSB & 0XFF) << 6) | (LSB >> 2);
		combinedValue = (combinedValue ^ (1 << 13)) - (1 << 13);
//		SEGGER_RTT_printf(0, "\nReading from sensor Z: %d", combinedValue);
        	zData[i] = combinedValue;
	}

	while((OSA_TimeGetMsec() - startTime) < delay && i < DLength - 1){}
	
	}
	
	if(print){
	for(int j=0; j<DLength; j++)
	{
		SEGGER_RTT_printf(0, "\n X, %d,", xData[j]);
		SEGGER_RTT_printf(0, "Y, %d,", yData[j]);
		SEGGER_RTT_printf(0, "Z, %d", zData[j]);
	}
	}

	maxAx = maxAxSel(xData, yData, zData);
	if(maxAx == 0){ SEGGER_RTT_WriteString(0, "\n MAXIMAL AXIS SELECTION HAS FAILED, SEND HELP \n"); }
	if(maxAx == 1.0){crossings = 0;}
	if((maxAx - 0.1) - (int)(maxAx - 0.1) == 0){
		maxAx = maxAx - 0.1;
		crossings = crossing(xData, maxAx);	
	}
	if((maxAx - 0.2) - (int)(maxAx - 0.2) == 0){
		maxAx = maxAx - 0.2;
		crossings = crossing(yData, maxAx);
	}
	if((maxAx - 0.3) - (int)(maxAx - 0.3) == 0){
		maxAx = maxAx - 0.3;
		crossings = crossing(zData, maxAx);
	}
	SEGGER_RTT_printf(0, "\n step count = %d \n", crossings);
	return crossings;
}

static uint8_t
setHeight(uint8_t numCol[3], uint8_t barCol[3], uint8_t bgCol[3])
{
	/* setHeight() displays a height in cm on the screen that can be adjusted using the SW2 and SW3 buttons (to increase or decrease height respectively)
	 * when holding down either button, the delay between height changes decreases, and goes back to the defualt value after letting go of both buttons
	 * this function will last for at least 8 seconds (with the heightSetup cap being 24). if this feels too long, it could be reduced to ~4 seconds by capping at 12, and doubling the multiplier in loadingbar()
	 */
	uint8_t         heightSetup = 0;
	uint8_t         hDelay = 50;
	uint8_t		height = 175;
	uint32_t        inc;
	uint32_t        dec;
	uint8_t		prev = 0;

	while(heightSetup < 24)
	{
		if(prev == 0){display_numbers(height, 'h', numCol, bgCol);}
		inc = SW2();
		dec = SW3();
		if(inc == 0){height += 1;}
		if(dec == 0){height -= 1;}
		if(inc == 1 && dec == 1){heightSetup += 1; hDelay = 50; loadingBar(24 - heightSetup, barCol, bgCol);}
		if((inc == 0 || dec == 0) && prev == 0 && hDelay > 1){hDelay -= 7; loadingBar(24, barCol, bgCol);}
		if(inc == 0 || dec == 0){prev = 0; heightSetup = 0;}
		else{prev = 1;}
		OSA_TimeDelay(hDelay);
	}
	loadingBar(24, bgCol, bgCol);
	SEGGER_RTT_printf(0, "\nyour height is %dcm\n", height);OSA_TimeDelay(500);
	return height;
}

static void
speed(int steps, uint8_t reads, uint8_t height, uint8_t numCol[3], uint8_t bgCol[3])
{
	/* speed() takes the number of steps done along with the amount of reads that the steps were done in, along with the user height, and displays the estimated speed of the user
	 * please note that the speed is only an estimate, and not calculated from accelerometer values directly.
	 * the conversion is taken from https://www.analog.com/en/analog-dialogue/articles/pedometer-design-3-axis-digital-acceler.html
	 * the assumption is essentially: more steps in 2 second period -> faster speed. Therefore, this is only a reference value and should not be taken as the absolute correct value of your speed
	 */
	uint8_t		step2sec = 0;
	uint16_t		stride = 0;
	uint16_t	newHeight = 0;
	step2sec = steps/reads;
	newHeight = height * step2sec;
	switch(step2sec)
	{
		default:
			stride = 1.2 * newHeight;
			break;
		case 0:
			stride = 0;
			break;
		case 1:
			stride = newHeight / 5;
			break;
		case 2:
			stride = newHeight / 4;
			break;
		case 3:
			stride = newHeight / 3;
			break;
		case 4:
			stride = newHeight / 2;
			break;
		case 5:
			stride = newHeight / 1.2;
			break;
		case 6:
			stride = newHeight;
			break;
		case 7:
			stride = newHeight;
			break;
	}
	stride = stride * 3.6 / 2;
	display_numbers(stride, 'v', numCol, bgCol);
}

void
cw5Code()
{	
	/* cw5Code() is where all of the above functions are put together to perform the pedometer functionality.
	 * it should be noted that this could easily be made into a loop - either starting before the height selection, or just after. However, I made a decision that this would not be too useful, as previous data is not stored, and so is only a very small quality-of-life improvement. Furthermore, the reset button on the FRDM board essentially acts as the button to reset the code back to before the height selection.
	 */
	uint8_t         peach[3] = {0xFF, 0x14, 0x93}; // it should be noted that a more apt name for this variable would be 'backgroundColour', or perhaps a new variable called background colour could be set to this peach value. However, it would not add much, as the peach value can simply be changed to change the background colour - with the name being kept as peach for fun legacy reasons.
	uint8_t		red[3] = {255, 0, 0};
	uint8_t         green[3] = {0, 255, 0};
	uint8_t         blue[3] = {0, 0, 255};
	uint8_t		cyan[3] = {212, 175, 55};
	uint8_t		height = 0;
	uint8_t		newSteps = 0;
	int		steps = 0;
	int		totalSteps = 0;
	uint32_t	startTime;
	uint8_t		reads = 0;
	int		totalReads = 0;
	uint8_t		displayInt = 9;
	bool		DISPFLAG = false;

	SEGGER_RTT_WriteString(0, "\nPedometer time ;) \n");

	setup();
	
	OSA_TimeDelay(250);

        writeCommand(kSSD1331CommandDRAWRECT);
	writeCommand(0x00);     //column address of start
	writeCommand(0x00);     //row address of start
	writeCommand(0x5F);     //collumn address of end
	writeCommand(0x3F);     //row address of end
	colour(peach);
	colour(peach);

	SEGGER_RTT_WriteString(0, "\n please input your height (cm) \n"); // "inputting user height"
	startTime = OSA_TimeGetMsec();
	height = setHeight(blue, green, peach);
	SEGGER_RTT_printf(0, "\n wait time is: %d milliseconds \n", OSA_TimeGetMsec() - startTime);

	centerSquare(red);
	SEGGER_RTT_WriteString(0, "\nPress either button to start taking acceleration data");OSA_TimeDelay(100);
	SEGGER_RTT_WriteString(0, "\nPress BOTH buttons for TWO SECONDS to stop taking readings, feel free to start at any time :)");
	
	while(SW2() == 1 && SW3() == 1){}
	centerSquare(green);
	while(SW2() == 1 || SW3() == 1)
	{
		startTime = OSA_TimeGetMsec();
		SEGGER_RTT_printf(0, "\nnew read inbound: %d\n", startTime);
		if(reads < displayInt)
		{
			newSteps = getAccelData(false);	//set to true to print out acceleration data (this will make the time between 2 second sampling periods larger than 50 ms, so other metrics will suffer
//			if(newSteps > 0){steps += newSteps; reads += 1;}
			steps += newSteps; reads += 1;
		}
		if(DISPFLAG)
		{
			SEGGER_RTT_printf(0, "\n displaying numbers @ %d\n", OSA_TimeGetMsec() - startTime);
			steps = (displayInt + 1)* steps / displayInt;
			totalSteps += steps;
			totalReads += reads + 1;
			display_numbers(totalSteps, 's', blue, peach);
			speed(steps, reads, height, blue, peach);
			reads = 0;
			steps = 0;
			DISPFLAG = false;
			SEGGER_RTT_printf(0, "\n finished displaying numbers @ %d\n", OSA_TimeGetMsec() - startTime);
		}
		if(reads == 9){DISPFLAG = true;}
		while(OSA_TimeGetMsec() - startTime < 2000){}
	}	
	centerSquare(cyan);
	totalReads += reads; totalSteps += steps;
	OSA_TimeDelay(100);
	display_numbers(totalSteps, 's', blue, peach); OSA_TimeDelay(100);
	OSA_TimeDelay(100);
	display_numbers(30 * totalSteps / totalReads, 'a', blue, peach); OSA_TimeDelay(100);
	OSA_TimeDelay(100);
	speed(totalSteps, totalReads, height, blue, peach);
	
	SEGGER_RTT_printf(0,"\n total steps: %d \n total reads: %d \n", totalSteps, totalReads);

//	"disable I2C pins before exiting this program"
	SEGGER_RTT_WriteString(0, "\r\n\n\tDisabling I2C pins...\n");
        disableI2Cpins();
}


