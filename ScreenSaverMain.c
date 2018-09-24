
/************************************************************************************

File: 			ScreenSaverMain.c

Description:	A personal, retro-beach themes screensaver.
				freeglut.h (OpenGL) is required for this project.

Author:			Adam Al-Jumaily

*************************************************************************************/


/* include the library header files */
#include <freeglut.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

/* Definitions */
#define PI 3.14159265
#define DEG_TO_RAD PI/180.0

/* Global variable declarations     */

// Flag to control the morph button toggle, so that it is not toggled by the timer function.
bool morphButtonToggle = false;

// Three flags toggled by buttons which control the features.
bool sparkFlag = false;
bool sparkleFlag = false;
bool morphFlag = false;

// Boolean value controls pausing during morphing.
bool timerRunning = false;

// Controls the speed at which the shape morphs.
float morphSpeed = 0.004;

// Controls the size of the spark and the rate at which it grows.
float sparkSize = 0;
float sparkSizeGrowth = 0.3;

// Values which control the spark as it moves along the shape.
float beta = 0.0;
float sparkPosX;
float sparkPosY;
int sparkCurrentIndex = 0;
int sparkTargetIndex = 1;

// Controls rotation of the spark.
float theta = 0.0;

// Controls linear interpolation of polygon vertices when morphing.
float alpha = 0.0;

// Number of clouds to be rendered.
int numberOfClouds = 6;

// Cloud X initial offsets.
float cloudOffset[6] = {
	55, 250, 300, 0, 25, 420
};
// Cloud lengths.
float cloudLength[6] = {
	225, 150, 300, 95, 200, 75
};
// Cloud X offset increments.
float cloudDistanceMoved[6] = {
	0, 0, 0, 0, 0, 0
};
// Cloud speeds.
float cloudSpeeds[6] = {
	0.2, 0.1, 0.05, 0.04, 0.045, 0.15
};

// Number of birds.
int numberOfBirds = 4;
// Bird width.
float birdWidth = 15;
// Bird wing height offset.
float wingOffset[4] = { 10, 10, 10, 10 };
// Bird positions.
float birdPositions[4][2] = {
	{50, 200}, {100, 300}, {300, 400}, {250, 100}
};
// Bird speeds.
float birdSpeeds[4] = {
	0.4, 0.3, 0.5, 0.8
};
// Bird flapping control array
bool birdFlapping[4] = {
	false, false, false, false
};

// Matrix of vertices that make up the letter.
float const LETTER[31][2] = {
		// Top rectangle vertices 0-3
		/* A1, A2, A3, A4*/
	    {125, 450}, {425, 450}, 
		{425, 400}, {125, 400},

		// Middle rectangle vertices 4-16
		/* B1 - B13 */
			// Polygon 1: B1, B2, B3, B4, B5, B6, B7
			{275, 400}, {325, 400}, {325, 275}, 
			{325, 200}, {300, 150}, {275, 200},
			{275, 275},
			// Triangle 1 (bottom-right): B8, B9, B10
			{325, 200}, {325, 150}, {300, 150},
			// Triangle 2 (bottom-left): B11, B12, B13
			{300, 150}, {275, 150}, {275, 200},

		// Points for bottom-right triangle 17-19
		/* C1, C2, C3*/
		{275, 150}, {325, 150},
		{275, 100},
		// Points for bottom rectangle 20-23
		/* D1, D2, D3, D4*/
		{275, 150}, {275, 100},
		{150, 100}, {150, 150},
		// Points for bottom-left triangle 24-26
		/* E1, E2, E3 */
		{100, 150}, {150, 150},
		{150, 100},
		// Points for left rectangle 27-30
		/* F1, F2, F3, F4*/
		{ 100, 225 }, { 150, 225 }, 
		{ 150, 150 }, { 100, 150 }
		
};

// Matrix of vertices that make up the intermediate morphing shape.
float const INTERMEDIARY_SHAPE_1[31][2] = {
	// Top rectangle vertices 0-3
	/* A1, A2, A3, A4*/
	{ 125, 450 },{ 325, 450 },
	{ 325, 400 },{ 125, 400 },

	// Middle rectangle vertices 4-16
	/* B1 - B13 */
	// Polygon 1: B1, B2, B3, B4, B5, B6, B7
	{ 275, 400 },{ 325, 400 },{ 325, 275 },
	{ 325, 200 },{ 300, 150 },{ 275, 200 },
	{ 275, 275 },
	// Triangle 1 (bottom-right): B8, B9, B10
	{ 325, 200 },{ 325, 150 },{ 300, 150 },
	// Triangle 2 (bottom-left): B11, B12, B13
	{ 300, 150 },{ 275, 150 },{ 275, 200 },

	// Points for bottom-right triangle 17-19
	/* C1, C2, C3*/
	{ 275, 150 },{ 325, 150 },
	{ 275, 100 },
	// Points for bottom rectangle 20-23
	/* D1, D2, D3, D4*/
	{ 275, 150 },{ 275, 100 },
	{ 150, 100 },{ 150, 150 },
	// Points for bottom-left triangle 24-26
	/* E1, E2, E3 */
	{ 150, 150 },{ 150, 150 },
	{ 150, 100 },
	// Points for left rectangle 27-30
	/* F1, F2, F3, F4*/
	{ 150, 150 },{ 150, 150 },
	{ 150, 150 },{ 150, 150 }

};

// Matrix of vertices that make up the second intermediate morphing shape.
float const INTERMEDIARY_SHAPE_2[31][2] = {
	// Top rectangle vertices 0-3
	/* A1, A2, A3, A4*/
	{ 275, 450 },{ 325, 450 },
	{ 325, 400 },{ 275, 400 },

	// Middle rectangle vertices 4-16
	/* B1 - B13 */
		// Polygon 1: B1, B2, B3, B4, B5, B6, B7
		{ 275, 400 },{ 325, 400 },{ 325, 275 },
		{ 325, 200 },{ 300, 150 },{ 275, 200 },
		{ 275, 275 },
		// Triangle 1 (bottom-right): B8, B9, B10
		{ 325, 200 },{ 325, 150 },{ 300, 150 },
		// Triangle 2 (bottom-left): B11, B12, B13
		{ 300, 150 },{ 275, 150 },{ 275, 200 },

	// Points for bottom-right triangle 17-19
	/* C1, C2, C3*/
	{ 275, 150 },{ 325, 150 },
	{ 275, 100 },
	// Points for bottom rectangle 20-23
	/* D1, D2, D3, D4*/
	{ 275, 150 },{ 275, 100 },
	{ 275, 100 },{ 275, 150 },
	// Points for bottom-left triangle 24-26
	/* E1, E2, E3 */
	{ 275, 150 },{ 275, 150 },
	{ 275, 150 },
	// Points for left rectangle 27-30
	/* F1, F2, F3, F4*/
	{ 275, 150 },{ 275, 150 },
	{ 275, 150 },{ 275, 150 }

};

// Matrix of vertices that make up the third intermediate morphing shape.
float const INTERMEDIARY_SHAPE_3[31][2] = {
	// Top rectangle vertices 0-3
	/* A1, A2, A3, A4*/
	{ 275, 450 },{ 325, 450 },
	{ 325, 400 },{ 275, 400 },

	// Middle rectangle vertices 4-16
	/* B1 - B13 */
		// Polygon 1: B1, B2, B3, B4, B5, B6, B7
		{ 275, 400 },{ 325, 400 },{ 325, 275 },
		{ 325, 200 },{ 300, 150 },{ 275, 200 },
		{ 275, 275 },
		// Triangle 1 (bottom-right): B8, B9, B10
		{ 325, 200 },{ 325, 150 },{ 300, 150 },
		// Triangle 2 (bottom-left): B11, B12, B13
		{ 300, 150 },{ 275, 150 },{ 275, 200 },

	// Points for bottom-right triangle 17-19
	/* C1, C2, C3*/
	{ 300, 150 },{ 300, 150 },
	{ 300, 150 },
	// Points for bottom rectangle 20-23
	/* D1, D2, D3, D4*/
	{ 300, 150 },{ 300, 150 },
	{ 300, 150 },{ 300, 150 },
	// Points for bottom-left triangle 24-26
	/* E1, E2, E3 */
	{ 300, 150 },{ 300, 150 },
	{ 300, 150 },
	// Points for left rectangle 27-30
	/* F1, F2, F3, F4*/
	{ 300, 150 },{ 300, 150 },
	{ 300, 150 },{ 300, 150 }

};

// Matrix of vertices that make up the star.
float const STAR[31][2] = {
	// Top rectangle vertices 0-3
	/* A1, A2, A3, A4*/
	{ 250, 450 },{ 250, 450 },
	{ 285, 315 },{ 215, 315 },

	// Middle rectangle vertices 4-16
	/* B1 - B13 */
	// Polygon 1: B1, B2, B3, B4, B5, B6, B7
	{ 215, 315 },{ 285, 315 },{ 400, 300 },
	{ 310, 240 },{ 250, 200 },{ 190, 240 },
	{ 100, 300 },
	// Triangle 1 (bottom-right): B8, B9, B10
	{ 310, 240 },{ 325, 125 },{ 250, 200 },
	// Triangle 2 (bottom-left): B11, B12, B13
	{ 250, 200 },{ 175, 125 },{ 190, 240 },

	// Points for bottom-right triangle 17-19
	/* C1, C2, C3*/
	{ 250, 200 },{ 250, 200 },
	{ 250, 200 },
	// Points for bottom rectangle 20-23
	/* D1, D2, D3, D4*/
	{ 250, 200 },{ 250, 200 },
	{ 250, 200 },{ 250, 200 },
	// Points for bottom-left triangle 24-26
	/* E1, E2, E3 */
	{ 250, 200 },{ 250, 200 },
	{ 250, 200 },
	// Points for left rectangle 27-30
	/* F1, F2, F3, F4*/
	{ 250, 200 },{ 250, 200 },
	{ 250, 200 },{ 250, 200 }

};

// Holds sparkle endpoint locations.
float sparklePositions[10][2];

// Holds sparkle centerpoint locations.
float sparkleCenters[10][2] = {
	{ 250, 450 },{ 250, 450 },{ 250, 450 },
	{ 250, 450 },{ 250, 450 },{ 250, 450 },
	{ 250, 450 },{ 250, 450 },{ 250, 450 },
	{ 250, 450 }
};

// Holds spark speeds.
float sparkleSpeeds[10];

// Holds spark alpha values.
float sparkleAlphas[10] = {
	0.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 0.0
};

// Pointer to the current state of the shape.
float(*current)[2] = LETTER;

// Pointer to the target morphed state of the shape. 
float (*target)[2] = INTERMEDIARY_SHAPE_1;

/* Control variable for state/shape changes.
	0  :  The current state is Letter, going to intermediary 1.
	1  :  The current state is intermediary 1, going to intermediary 2.
	2  :  The current state is a intermediary 2, going to intermediary 3. 
	3  :  The current state is intermediary 3, going to star.  
	4  :  The current state is star, going to intermediary 3.
	5  :  The current state is intermediary 3, going to intermediary 2.
	6  :  The current state is intermediary 2, going to intermediary 1.
	7  :  The current state is intermediary 1, going to Letter.      */
int currentState = 0;


/************************************************************************

Function:		initializeSparkles

Description:	Initializes the random starting values for the sparkles to use.

*************************************************************************/

void initializeSparkles() {
	for (int i = 0; i < 10; i++) {
		sparkleSpeeds[i] = ((float)rand() / ((float)RAND_MAX * 50)) + 0.01;
		sparklePositions[i][0] = rand() % 500;
		sparklePositions[i][1] = rand() % 500;
	}
}

/************************************************************************

Function:		morphTimerControl

Description:	Function that sets the morphFlag back to true if it
				has been set to false by a timer callback. Also updates
				the target shape and the current state of the morph.

*************************************************************************/

void morphTimerControl(int x) {
	if (timerRunning) {
		morphFlag = true;
		switch (x) {
		case 0:
			target = INTERMEDIARY_SHAPE_3;
			currentState = 4;
			break;
		case 1:
			target = INTERMEDIARY_SHAPE_1;
			currentState = 0;
			break;
		}
	}
}

/************************************************************************

Function:		birdTimerControl

Description:	Flaps bird wings at intervals determined by speed.

*************************************************************************/

void birdTimerControl(int x) {
	switch (x) {
	case 2:
		birdFlapping[x - 2] = false;
		break;
	case 3:
		birdFlapping[x - 2] = false;
		break;
	case 4:
		birdFlapping[x - 2] = false;
		break;
	case 5:
		birdFlapping[x - 2] = false;
		break;
	}
}

/************************************************************************

Function:		mouseClick

Description:	Event handler for mouseclicks.

*************************************************************************/
void mouseClick(int button, int state, int x, int y) {
	
	// If the left mouse button is pressed down
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {

		// If the mouseclick occurs within the spark button dimensions...
		if (
			(x >= (60) && x <= (242))
			&& ((y >= 666) && (y <= 722)))
		{
			// Toggle the spark flag and redisplay.
			sparkFlag = !sparkFlag;
			if (sparkleFlag) {
				sparkleFlag = false;
			}
			glutPostRedisplay();
		}
		// If the mouseclick occurs within the sparkle button dimensions...
		else if (
			(x >= (288) && x <= (467))
			&& ((y >= 666) && (y <= 722)))
		{
			// Toggle the sparkle flag and redisplay if spark flag is on.
			if (sparkFlag) {
				initializeSparkles();
				sparkleFlag = !sparkleFlag;
				glutPostRedisplay();
			}
			initializeSparkles();
		}
		// If the mouseclick occurs within the morph button dimensions...
		else if (
			(x >= (513) && x <= (690))
			&& ((y >= 666) && (y <= 722)))
		{
			// Toggles the morph button outline.
			morphButtonToggle = !morphButtonToggle;
			// Toggles either the timer or morph flags to stop morphing.
			if (timerRunning) {
				timerRunning = false;
				morphFlag = false;
			}
			else {
				morphFlag = !morphFlag;
			}
			glutPostRedisplay();
		}
	}
}

/************************************************************************

Function:		idle

Description:	Idle function for animation.

*************************************************************************/
void idle() {
	
	// Moves and repositions birds, flaps wings.
	for (int i = 0; i < numberOfBirds; i++) {

		// Increments bird X position.
		birdPositions[i][0] += birdSpeeds[i];

		// If the bird goes off-screen, reset it at a randomly generated Y value with random speed.
		if (birdPositions[i][0] >= 500) {
			birdPositions[i][0] = -birdWidth;
			birdPositions[i][1] = rand() % 400 + 100;
			birdSpeeds[i] = 0.3 + ((float)rand() / ((float)RAND_MAX));
		}
		
		// If the bird is not currently flapping, switch the wingOffset and flag, then use a callback function.
		if (birdFlapping[i] == false) {
			wingOffset[i] *= -1;
			birdFlapping[i] = true;
			glutTimerFunc((birdSpeeds[i]*1200), birdTimerControl, (i+2));
		}
	}

	// Moves clouds and repositions them if necessary
	for (int i = 0; i < numberOfClouds; i++) {
		cloudDistanceMoved[i] += cloudSpeeds[i];
		if ((cloudOffset[i] + cloudDistanceMoved[i]) >= 500) {
			cloudOffset[i] = -cloudLength[i];
			cloudDistanceMoved[i] = 0;
		}
	}

	// If the spark flag is enabled.
	if (sparkFlag) {

		// Interpolates the spark centerpoint using beta, between two interpolating vertices of the morphing shape. The location of these vertices is determined by alpha.
		sparkPosX = ((1 - beta) * (((1 - alpha)*current[sparkCurrentIndex][0]) + (alpha * target[sparkCurrentIndex][0]))) + (beta * (((1 - alpha)*current[sparkTargetIndex][0]) + (alpha * target[sparkTargetIndex][0])));
		sparkPosY = ((1 - beta) * (((1 - alpha)*current[sparkCurrentIndex][1]) + (alpha * target[sparkCurrentIndex][1]))) + (beta * (((1 - alpha)*current[sparkTargetIndex][1]) + (alpha * target[sparkTargetIndex][1])));

		// Increase beta by 0.005 + a value determined using the remaining distance between the morphing points being interpolated between. This is to maintain a similar speed of the spark.
		beta += 0.005 + (0.000001 * sqrt(pow((beta * (((1 - alpha)*current[sparkTargetIndex][0]) + (alpha * target[sparkTargetIndex][0])) - (((1 - alpha)*current[sparkCurrentIndex][0]) + (alpha * target[sparkCurrentIndex][0]))), 2)
				+ pow((beta * (((1 - alpha)*current[sparkTargetIndex][1]) + (alpha * target[sparkTargetIndex][1])) - (((1 - alpha)*current[sparkCurrentIndex][1]) + (alpha * target[sparkCurrentIndex][1]))), 2)));

		// If the current and target points are the same in both our current shape and our target shape, then we skip interpolation between these points as they are equal and will not change.
		if ((current[sparkCurrentIndex][0] == current[sparkTargetIndex][0]) && (current[sparkCurrentIndex][1] == current[sparkTargetIndex][1]) &&
			(target[sparkCurrentIndex][0] == target[sparkTargetIndex][0]) && (target[sparkCurrentIndex][1] == target[sparkTargetIndex][1])) {
			beta = 1;
		}

		// Switch statement which determines the next point to interpolate to.
		if (beta >= 1) {
			sparkCurrentIndex = sparkTargetIndex;
			switch (sparkCurrentIndex) {
			case 2:
				sparkTargetIndex = 5;
				break;
			case 7:
				sparkTargetIndex = 12;
				break;
			case 12:
				sparkTargetIndex = 19;
				break;
			case 19:
				sparkTargetIndex = 22;
				break;
			case 22:
				sparkTargetIndex = 24;
				break;
			case 24:
				sparkTargetIndex = 27;
				break;
			case 29:
				sparkTargetIndex = 15;
				break;
			case 16:
				sparkTargetIndex = 9;
				break;
			case 10:
				sparkTargetIndex = 4;
				break;
			case 4:
				sparkTargetIndex = 3;
				break;
			case 3:
				sparkTargetIndex = 0;
				break;
			default:
				sparkTargetIndex++;
			}
			beta = 0.0;
		}

		// Rotates, grows, and shrinks the spark.
		theta += 0.5;
		sparkSize += sparkSizeGrowth;
		if (theta >= 360) {
			theta = 0;
		}
		if ((sparkSize > 10.0) || (sparkSize < -10.0)) {
			sparkSizeGrowth *= -1;
		}
		glutPostRedisplay();
	}

	// If the morph flag is toggled.
	if (morphFlag) {
		if (alpha >= 1.0) {
			switch (currentState) {
			case 0:
				// Letter has become first intermediary. Need to go to second.
				alpha = 0.0;
				current = INTERMEDIARY_SHAPE_1;
				target = INTERMEDIARY_SHAPE_2;
				currentState = 1;
				break;
			case 1:
				// Intermediary1 has become second intermediary. Need to go to third.
				alpha = 0.0;
				current = INTERMEDIARY_SHAPE_2;
				target = INTERMEDIARY_SHAPE_3;
				currentState = 2;
				break;
			case 2:
				// Intermediary2 has become third intermediary. Need to go to star.
				alpha = 0.0;
				morphSpeed = 0.003;
				current = INTERMEDIARY_SHAPE_3;
				target = STAR;
				currentState = 3;
				break;
			case 3:
				// Intermediary has become Star. Need to go back to intermediary 3.
				alpha = 0.0;
				morphSpeed = 0.004;
				current = STAR;
				//currentState = 4;
				// Sets the timer flag to true to deal with morph clicks during this time.
				morphFlag = false;
				timerRunning = true;
				// Callback which pauses the morph for 2.5 seconds.
				glutTimerFunc(2500, morphTimerControl, 0);
				break;
			case 4:
				// Star has become third intermediary. Need to go to second intermediary.
				alpha = 0.0;
				current = INTERMEDIARY_SHAPE_3;
				target = INTERMEDIARY_SHAPE_2;
				currentState = 5;
				break;
			case 5:
				// Third intermediary has become second intermediary. Need to go to first intermediary.
				alpha = 0.0;
				current = INTERMEDIARY_SHAPE_2;
				target = INTERMEDIARY_SHAPE_1;
				currentState = 6;
				break;
			case 6:
				// Second intermediary has become first intermediary. Need to go to Letter.
				alpha = 0.0;
				morphSpeed = 0.003;
				current = INTERMEDIARY_SHAPE_1;
				target = LETTER;
				currentState = 7;
				break;
			case 7:
				// First intermediary has become Letter. Need to go back to first intermediary.
				alpha = 0.0;
				morphSpeed = 0.004;
				current = LETTER;
				// Sets the timer flag to true to deal with morph clicks during this time.
				morphFlag = false;
				timerRunning = true;
				// Callback which pauses the morph for 2.5 seconds.
				glutTimerFunc(2500, morphTimerControl, 1);
				//currentState = 0;
				break;
			}
		}
		else {
			alpha += morphSpeed;
		}
		glutPostRedisplay();
	}

	// If the sparkle flag is toggled.
	if (sparkleFlag) {
		/* For each sparkle, if interpolation is completed, generate a new sparkle using the current centerpoint of the spark 
		   and a randomly generated vertice and speed.  */
		for (int i = 0; i < 10; i++) {
			if (sparkleAlphas[i] >= 1) {
				sparkleCenters[i][0] = sparkPosX;
				sparkleCenters[i][1] = sparkPosY;
				sparklePositions[i][0] = rand() % 500;
				sparklePositions[i][1] = rand() % 500;
				sparkleSpeeds[i] = ((float)rand() / ((float)RAND_MAX*50)) + 0.01;
				sparkleAlphas[i] = 0.0;
			}
			sparkleAlphas[i] += sparkleSpeeds[i];
		}
	}
	glutPostRedisplay();
}

/************************************************************************

Function:		display

Description:	Displays a stylized capital 'J' with a button panel.

*************************************************************************/
void display(void)
{
	// clear the screen 
	glClear(GL_COLOR_BUFFER_BIT);

	// draw the background
	glBegin(GL_POLYGON);
	glColor3f(0.807, 0.235, 0.757);
	glVertex2f(0, 500);
	glVertex2f(500, 500);
	glColor3f(0.984, 0.768, 0.482);
	glVertex2f(500, 0);
	glVertex2f(0, 0);
	glEnd();

	// draw the clouds
		// Cloud 1 is drawn at the front, so at the end of the display function.
		// Cloud 2
	glBegin(GL_POLYGON);
	glColor4f(1, 1, 1, 0.6);
	glVertex2f(cloudOffset[1] + cloudDistanceMoved[1], 250);
	glVertex2f(cloudOffset[1] + cloudDistanceMoved[1], 315);
	glVertex2f((cloudOffset[1] + cloudDistanceMoved[1]) + cloudLength[1], 315);
	glVertex2f((cloudOffset[1] + cloudDistanceMoved[1]) + cloudLength[1], 250);
	glEnd();
		// Cloud 3
	glBegin(GL_POLYGON);
	glColor4f(1, 1, 1, 0.4);
	glVertex2f(cloudOffset[2] + cloudDistanceMoved[2], 125);
	glVertex2f(cloudOffset[2] + cloudDistanceMoved[2], 130);
	glVertex2f((cloudOffset[2] + cloudDistanceMoved[2]) + cloudLength[2], 130);
	glVertex2f((cloudOffset[2] + cloudDistanceMoved[2]) + cloudLength[2], 125);
	glEnd();
		// Cloud 4
	glBegin(GL_POLYGON);
	glColor4f(1, 1, 1, 0.2);
	glVertex2f(cloudOffset[3] + cloudDistanceMoved[3], 80);
	glVertex2f(cloudOffset[3] + cloudDistanceMoved[3], 90);
	glVertex2f((cloudOffset[3] + cloudDistanceMoved[3]) + cloudLength[3], 90);
	glVertex2f((cloudOffset[3] + cloudDistanceMoved[3]) + cloudLength[3], 80);
	glEnd();
		// Cloud 5
	glBegin(GL_POLYGON);
	glColor4f(1, 1, 1, 0.5);
	glVertex2f(cloudOffset[4] + cloudDistanceMoved[4], 185);
	glVertex2f(cloudOffset[4] + cloudDistanceMoved[4], 235);
	glVertex2f((cloudOffset[4] + cloudDistanceMoved[4]) + cloudLength[4], 235);
	glVertex2f((cloudOffset[4] + cloudDistanceMoved[4]) + cloudLength[4], 185);
	glEnd();
		// Cloud 6
	glBegin(GL_POLYGON);
	glColor4f(1, 1, 1, 0.7);
	glVertex2f(cloudOffset[5] + cloudDistanceMoved[5], 350);
	glVertex2f(cloudOffset[5] + cloudDistanceMoved[5], 415);
	glVertex2f((cloudOffset[5] + cloudDistanceMoved[5]) + cloudLength[5], 415);
	glVertex2f((cloudOffset[5] + cloudDistanceMoved[5]) + cloudLength[5], 350);
	glEnd();
	
	// Draw the birds behind the letter.
	// Bird 1
	glBegin(GL_LINES);
	glColor4f(0, 0, 0, 0.9);
	glVertex2f(birdPositions[0][0], birdPositions[0][1]);
	glVertex2f(birdPositions[0][0] + birdWidth, birdPositions[0][1]);
	glEnd();
	glBegin(GL_TRIANGLES);
	glVertex2f(birdPositions[0][0] + 4, birdPositions[0][1]);
	glVertex2f(birdPositions[0][0] + 2, birdPositions[0][1] + wingOffset[0]);
	glVertex2f(birdPositions[0][0] + 10, birdPositions[0][1]);
	glEnd();
	glBegin(GL_TRIANGLES);
	glVertex2f(birdPositions[0][0] + 7, birdPositions[0][1]);
	glVertex2f(birdPositions[0][0] + 8, birdPositions[0][1] + wingOffset[0]);
	glVertex2f(birdPositions[0][0] + 12, birdPositions[0][1]);
	glEnd();
	// Bird 2
	glBegin(GL_LINES);
	glColor4f(0, 0, 0, 0.9);
	glVertex2f(birdPositions[1][0], birdPositions[1][1]);
	glVertex2f(birdPositions[1][0] + birdWidth, birdPositions[1][1]);
	glEnd();
	glBegin(GL_TRIANGLES);
	glVertex2f(birdPositions[1][0] + 4, birdPositions[1][1]);
	glVertex2f(birdPositions[1][0] + 2, birdPositions[1][1] + wingOffset[1]);
	glVertex2f(birdPositions[1][0] + 10, birdPositions[1][1]);
	glEnd();
	glBegin(GL_TRIANGLES);
	glVertex2f(birdPositions[1][0] + 7, birdPositions[1][1]);
	glVertex2f(birdPositions[1][0] + 8, birdPositions[1][1] + wingOffset[1]);
	glVertex2f(birdPositions[1][0] + 12, birdPositions[1][1]);
	glEnd();

	// START DRAWING THE PRIMARY SHAPE.
	// draw the top rectangle
	glBegin(GL_POLYGON);
	glColor3f(0.733, 0.490, 0.927);
	glVertex2f(((1 - alpha)*current[0][0]) + (alpha * target[0][0]), ((1 - alpha)*current[0][1]) + (alpha * target[0][1]));
	glVertex2f(((1 - alpha)*current[1][0]) + (alpha * target[1][0]), ((1 - alpha)*current[1][1]) + (alpha * target[1][1]));
	glVertex2f(((1 - alpha)*current[2][0]) + (alpha * target[2][0]), ((1 - alpha)*current[2][1]) + (alpha * target[2][1]));
	glVertex2f(((1 - alpha)*current[3][0]) + (alpha * target[3][0]), ((1 - alpha)*current[3][1]) + (alpha * target[3][1]));
	glEnd();
	// draw the center-right rectangle
	glBegin(GL_POLYGON);
		// Center polygon: B1-B7
	glColor3f(0.733, 0.490, 0.927);
		// B1
	glVertex2f(((1 - alpha)*current[4][0]) + (alpha * target[4][0]), ((1 - alpha)*current[4][1]) + (alpha * target[4][1]));
		// B2
	glVertex2f(((1 - alpha)*current[5][0]) + (alpha * target[5][0]), ((1 - alpha)*current[5][1]) + (alpha * target[5][1]));
		// B3
	glVertex2f(((1 - alpha)*current[6][0]) + (alpha * target[6][0]), ((1 - alpha)*current[6][1]) + (alpha * target[6][1]));
	glColor3f(0.666, 0.906, 0.910);
		// B4
	glVertex2f(((1 - alpha)*current[7][0]) + (alpha * target[7][0]), ((1 - alpha)*current[7][1]) + (alpha * target[7][1]));
		// B5
	glVertex2f(((1 - alpha)*current[8][0]) + (alpha * target[8][0]), ((1 - alpha)*current[8][1]) + (alpha * target[8][1]));
		// B6
	glVertex2f(((1 - alpha)*current[9][0]) + (alpha * target[9][0]), ((1 - alpha)*current[9][1]) + (alpha * target[9][1]));
		// B7
	glVertex2f(((1 - alpha)*current[10][0]) + (alpha * target[10][0]), ((1 - alpha)*current[10][1]) + (alpha * target[10][1]));
	glEnd();

	// Draw the bottom-right triangle of shape B
	glBegin(GL_TRIANGLES);
		// B8
	glVertex2f(((1 - alpha)*current[11][0]) + (alpha * target[11][0]), ((1 - alpha)*current[11][1]) + (alpha * target[11][1]));
		// B9
	glVertex2f(((1 - alpha)*current[12][0]) + (alpha * target[12][0]), ((1 - alpha)*current[12][1]) + (alpha * target[12][1]));
		// B10
	glVertex2f(((1 - alpha)*current[13][0]) + (alpha * target[13][0]), ((1 - alpha)*current[13][1]) + (alpha * target[13][1]));
	glEnd();

	// Draw the bottom-left triangle of shape B
	glBegin(GL_TRIANGLES);
		// B11
	glVertex2f(((1 - alpha)*current[14][0]) + (alpha * target[14][0]), ((1 - alpha)*current[14][1]) + (alpha * target[14][1]));
		// B12
	glVertex2f(((1 - alpha)*current[15][0]) + (alpha * target[15][0]), ((1 - alpha)*current[15][1]) + (alpha * target[15][1]));
		// B13
	glVertex2f(((1 - alpha)*current[16][0]) + (alpha * target[16][0]), ((1 - alpha)*current[16][1]) + (alpha * target[16][1]));
	glEnd();

	
	// draw the bottom-right triangle
	glBegin(GL_TRIANGLES);
	glVertex2f(((1 - alpha)*current[17][0]) + (alpha * target[17][0]), ((1 - alpha)*current[17][1]) + (alpha * target[17][1]));
	glVertex2f(((1 - alpha)*current[18][0]) + (alpha * target[18][0]), ((1 - alpha)*current[18][1]) + (alpha * target[18][1]));
	glVertex2f(((1 - alpha)*current[19][0]) + (alpha * target[19][0]), ((1 - alpha)*current[19][1]) + (alpha * target[19][1]));
	glEnd();

	// draw the bottom rectangle
	glBegin(GL_POLYGON);
	glVertex2f(((1 - alpha)*current[20][0]) + (alpha * target[20][0]), ((1 - alpha)*current[20][1]) + (alpha * target[20][1]));
	glVertex2f(((1 - alpha)*current[21][0]) + (alpha * target[21][0]), ((1 - alpha)*current[21][1]) + (alpha * target[21][1]));
	glColor3f(0.749, 1.0, 0.898);
	glVertex2f(((1 - alpha)*current[22][0]) + (alpha * target[22][0]), ((1 - alpha)*current[22][1]) + (alpha * target[22][1]));
	glVertex2f(((1 - alpha)*current[23][0]) + (alpha * target[23][0]), ((1 - alpha)*current[23][1]) + (alpha * target[23][1]));
	glEnd();

	// draw the bottom-left triangle
	glBegin(GL_TRIANGLES);
	glVertex2f(((1 - alpha)*current[24][0]) + (alpha * target[24][0]), ((1 - alpha)*current[24][1]) + (alpha * target[24][1]));
	glVertex2f(((1 - alpha)*current[25][0]) + (alpha * target[25][0]), ((1 - alpha)*current[25][1]) + (alpha * target[25][1]));
	glVertex2f(((1 - alpha)*current[26][0]) + (alpha * target[26][0]), ((1 - alpha)*current[26][1]) + (alpha * target[26][1]));
	glEnd();

	// draw the left rectangle
	glBegin(GL_POLYGON);
	glColor3f(0.941, 0.898, 0.921);
	glVertex2f(((1 - alpha)*current[27][0]) + (alpha * target[27][0]), ((1 - alpha)*current[27][1]) + (alpha * target[27][1]));
	glVertex2f(((1 - alpha)*current[28][0]) + (alpha * target[28][0]), ((1 - alpha)*current[28][1]) + (alpha * target[28][1]));
	glColor3f(0.749, 1.0, 0.898);
	glVertex2f(((1 - alpha)*current[29][0]) + (alpha * target[29][0]), ((1 - alpha)*current[29][1]) + (alpha * target[29][1]));
	glVertex2f(((1 - alpha)*current[30][0]) + (alpha * target[30][0]), ((1 - alpha)*current[30][1]) + (alpha * target[30][1]));
	glEnd();

	// STOP DRAWING THE PRIMARY SHAPE.


	// draw the bottom box for buttons
	glBegin(GL_POLYGON);
	glColor3f(0.533, 0.490, 0.827);
	glVertex2i(0, 0);
	glVertex2i(500, 0);
	glColor4f(0.941, 0.898, 0.921, 0.5);
	glVertex2i(500, 75);
	glVertex2i(0, 75);
	glEnd();

	// draw the spark button and its outline
	if (sparkFlag) {
		// User toggle display
		glBegin(GL_POLYGON);
		glColor4f(0, 0, 0, 0.5);
		glVertex2i(38, 18);
		glVertex2i(38, 57);
		glVertex2i(162, 57);
		glVertex2i(162, 18);
		glEnd();
	}
	glBegin(GL_POLYGON);
	glColor4f(0.941, 0.898, 0.921, 0.0);
	glVertex2i(40, 20);
	glVertex2i(40, 55);
	glColor4f(0.941, 0.898, 0.921, 1.0);
	glVertex2i(160, 55);
	glVertex2i(160, 20);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.990, 0.345, 0.357);
	glVertex2i(45, 25);
	glVertex2i(45, 50);
	glVertex2i(155, 50);
	glVertex2i(155, 25);
	glEnd();

	// draw the sparkle button and its outline
	if (sparkleFlag) {
		// User toggle display
		glBegin(GL_POLYGON);
		glColor4f(0, 0, 0, 0.5);
		glVertex2i(188, 18);
		glVertex2i(188, 57);
		glVertex2i(312, 57);
		glVertex2i(312, 18);
		glEnd();
	}
	glBegin(GL_POLYGON);
	glColor4f(0.941, 0.898, 0.921, 0.0);
	glVertex2i(190, 20);
	glVertex2i(190, 55);
	glColor4f(0.941, 0.898, 0.921, 1.0);
	glVertex2i(310, 55);
	glVertex2i(310, 20);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.990, 0.345, 0.357);
	glVertex2i(195, 25);
	glVertex2i(195, 50);
	glVertex2i(305, 50);
	glVertex2i(305, 25);
	glEnd();

	// draw the morph button and its outlines
	if (morphButtonToggle) {
		// User toggle display
		glBegin(GL_POLYGON);
		glColor4f(0, 0, 0, 0.5);
		glVertex2i(338, 18);
		glVertex2i(338, 57);
		glVertex2i(462, 57);
		glVertex2i(462, 18);
		glEnd();
	}

	glBegin(GL_POLYGON);
	glColor4f(0.941, 0.898, 0.921, 0.0);
	glVertex2i(340, 20);
	glVertex2i(340, 55);
	glColor4f(0.941, 0.898, 0.921, 1.0);
	glVertex2i(460, 55);
	glVertex2i(460, 20);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.990, 0.345, 0.357);
	glVertex2i(345, 25);
	glVertex2i(345, 50);
	glVertex2i(455, 50);
	glVertex2i(455, 25);
	glEnd();

	// draw text on the spark button
	glColor3f(0.0, 0.0, 0.0);
	char spark[] = "SPARK";
	int s1 = sizeof(spark) / sizeof(spark[0]);
	glRasterPos2i(75, 32);
	for (int i = 0; i < s1; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, spark[i]);
	}

	// draw text on the sparkle button
	char sparkle[] = "SPARKLE";
	int s2 = sizeof(sparkle) / sizeof(sparkle[0]);
	glRasterPos2i(215, 32);
	for (int i = 0; i < s2; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, sparkle[i]);
	}

	// draw text on the morph button
	char morph[] = "MORPH";
	int s3 = sizeof(morph) / sizeof(morph[0]);
	glRasterPos2i(370, 32);
	for (int i = 0; i < s3; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, morph[i]);
	}

	if (sparkFlag) {

		/* Draws the spark. 
			Draws 8 lines to make up the spark which rotate around the vertex determined
			by sparkPosX and sparkPosY.*/
		glLineWidth(4.5);
		glBegin(GL_LINES);

		glColor4f(1, 1, 0, 1.0);
		glVertex2f(sparkPosX, sparkPosY);
		glColor4f(1, 1, 0, 0.0);
		glVertex2f(((50.0 + sparkSize) * cos(DEG_TO_RAD * theta) + sparkPosX), ((50.0 + sparkSize) * sin(DEG_TO_RAD * theta) + sparkPosY));

		glColor4f(1, 1, 0, 1.0);
		glVertex2f(sparkPosX, sparkPosY);
		glColor4f(1, 1, 0, 0.0);
		glVertex2f(((30.0 + sparkSize) * cos(DEG_TO_RAD * (theta + 45)) + sparkPosX), ((30.0 + sparkSize) * sin(DEG_TO_RAD * (theta + 45)) + sparkPosY));

		glColor4f(1, 1, 0, 1.0);
		glVertex2f(sparkPosX, sparkPosY);
		glColor4f(1, 1, 0, 0.0);
		glVertex2f(((50.0 + sparkSize) * cos(DEG_TO_RAD * (theta + 90)) + sparkPosX), ((50.0 + sparkSize) * sin(DEG_TO_RAD * (theta + 90)) + sparkPosY));

		glColor4f(1, 1, 0, 1.0);
		glVertex2f(sparkPosX, sparkPosY);
		glColor4f(1, 1, 0, 0.0);
		glVertex2f(((30.0 + sparkSize) * cos(DEG_TO_RAD * (theta + 135)) + sparkPosX), ((30.0 + sparkSize) * sin(DEG_TO_RAD * (theta + 135)) + sparkPosY));

		glColor4f(1, 1, 0, 1.0);
		glVertex2f(sparkPosX, sparkPosY);
		glColor4f(1, 1, 0, 0.0);
		glVertex2f((((50.0 + sparkSize) * cos(DEG_TO_RAD * (theta + 180))) + sparkPosX), (((50.0 + sparkSize) * sin(DEG_TO_RAD * (theta + 180))) + sparkPosY));

		glColor4f(1, 1, 0, 1.0);
		glVertex2f(sparkPosX, sparkPosY);
		glColor4f(1, 1, 0, 0.0);
		glVertex2f(((30.0 + sparkSize) * cos(DEG_TO_RAD * (theta + 225)) + sparkPosX), ((30.0 + sparkSize) * sin(DEG_TO_RAD * (theta + 225)) + sparkPosY));

		glColor4f(1, 1, 0, 1.0);
		glVertex2f(sparkPosX, sparkPosY);
		glColor4f(1, 1, 0, 0.0);
		glVertex2f(((50.0 + sparkSize) * cos(DEG_TO_RAD * (theta + 270)) + sparkPosX), ((50.0 + sparkSize) * sin(DEG_TO_RAD * (theta + 270)) + sparkPosY));

		glColor4f(1, 1, 0, 1.0);
		glVertex2f(sparkPosX, sparkPosY);
		glColor4f(1, 1, 0, 0.0);
		glVertex2f(((30.0 + sparkSize) * cos(DEG_TO_RAD * (theta + 315)) + sparkPosX), ((30.0 + sparkSize) * sin(DEG_TO_RAD * (theta + 315)) + sparkPosY));
		glEnd();
	}

	if (sparkleFlag) {
		/* Shoots sparkles out of the spark. Using interpolation of a line segment between
			a randomly generated point and the point at which the sparkle was initialized. 
			There are 10 sparkles occuring at any one time. */
		glColor3f(0.0, 0.0, 0.0);
		glBegin(GL_LINES);
		// Spark 1
		glColor4f(0 + sparkleAlphas[0], 0 + sparkleAlphas[1], 0 + sparkleAlphas[2], 0);
		glVertex2f((1 - sparkleAlphas[0])*sparkleCenters[0][0] + sparkleAlphas[0] * sparklePositions[0][0], (1 - sparkleAlphas[0])*sparkleCenters[0][1] + sparkleAlphas[0] * sparklePositions[0][1]);
		glColor4f(0 + sparkleAlphas[0], 0 + sparkleAlphas[1], 0 + sparkleAlphas[2], 1);
		glVertex2f((1 - (sparkleAlphas[0] + 0.2))*sparkleCenters[0][0] + (sparkleAlphas[0] + 0.2) * sparklePositions[0][0], (1 - (sparkleAlphas[0] + 0.2))*sparkleCenters[0][1] + (sparkleAlphas[0] + 0.2) * sparklePositions[0][1]);
		// Spark 2 
		glColor4f(0 + sparkleAlphas[1], 0 + sparkleAlphas[2], 0 + sparkleAlphas[3], 0);
		glVertex2f((1 - sparkleAlphas[1])*sparkleCenters[1][0] + sparkleAlphas[1] * sparklePositions[1][0], (1 - sparkleAlphas[1])*sparkleCenters[1][1] + sparkleAlphas[1] * sparklePositions[1][1]);
		glColor4f(0 + sparkleAlphas[1], 0 + sparkleAlphas[2], 0 + sparkleAlphas[3], 1);
		glVertex2f((1 - (sparkleAlphas[1] + 0.2))*sparkleCenters[1][0] + (sparkleAlphas[1] + 0.2) * sparklePositions[1][0], (1 - (sparkleAlphas[1] + 0.2))*sparkleCenters[1][1] + (sparkleAlphas[1] + 0.2) * sparklePositions[1][1]);
		// Spark 3
		glColor4f(0 + sparkleAlphas[2], 0 + sparkleAlphas[3], 0 + sparkleAlphas[4], 0);
		glVertex2f((1 - sparkleAlphas[2])*sparkleCenters[2][0] + sparkleAlphas[2] * sparklePositions[2][0], (1 - sparkleAlphas[2])*sparkleCenters[2][1] + sparkleAlphas[2] * sparklePositions[2][1]);
		glColor4f(0 + sparkleAlphas[2], 0 + sparkleAlphas[3], 0 + sparkleAlphas[4], 1);
		glVertex2f((1 - (sparkleAlphas[2] + 0.2))*sparkleCenters[2][0] + (sparkleAlphas[2] + 0.2) * sparklePositions[2][0], (1 - (sparkleAlphas[2] + 0.2))*sparkleCenters[2][1] + (sparkleAlphas[2] + 0.2) * sparklePositions[2][1]);
		// Spark 4
		glColor4f(0 + sparkleAlphas[3], 0 + sparkleAlphas[4], 0 + sparkleAlphas[5], 0);
		glVertex2f((1 - sparkleAlphas[3])*sparkleCenters[3][0] + sparkleAlphas[3] * sparklePositions[3][0], (1 - sparkleAlphas[3])*sparkleCenters[3][1] + sparkleAlphas[3] * sparklePositions[3][1]);
		glColor4f(0 + sparkleAlphas[3], 0 + sparkleAlphas[4], 0 + sparkleAlphas[5], 1);
		glVertex2f((1 - (sparkleAlphas[3] + 0.2))*sparkleCenters[3][0] + (sparkleAlphas[3] + 0.2) * sparklePositions[3][0], (1 - (sparkleAlphas[3] + 0.2))*sparkleCenters[3][1] + (sparkleAlphas[3] + 0.2) * sparklePositions[3][1]);
		// Spark 5
		glColor4f(0 + sparkleAlphas[4], 0 + sparkleAlphas[5], 0 + sparkleAlphas[6], 0);
		glVertex2f((1 - sparkleAlphas[4])*sparkleCenters[4][0] + sparkleAlphas[4] * sparklePositions[4][0], (1 - sparkleAlphas[4])*sparkleCenters[4][1] + sparkleAlphas[4] * sparklePositions[4][1]);
		glColor4f(0 + sparkleAlphas[4], 0 + sparkleAlphas[5], 0 + sparkleAlphas[6], 1);
		glVertex2f((1 - (sparkleAlphas[4] + 0.2))*sparkleCenters[4][0] + (sparkleAlphas[4] + 0.2) * sparklePositions[4][0], (1 - (sparkleAlphas[4] + 0.2))*sparkleCenters[4][1] + (sparkleAlphas[4] + 0.2) * sparklePositions[4][1]);
		// Spark 6
		glColor4f(0 + sparkleAlphas[5], 0 + sparkleAlphas[6], 0 + sparkleAlphas[7], 0);
		glVertex2f((1 - sparkleAlphas[5])*sparkleCenters[5][0] + sparkleAlphas[5] * sparklePositions[5][0], (1 - sparkleAlphas[5])*sparkleCenters[5][1] + sparkleAlphas[5] * sparklePositions[5][1]);
		glColor4f(0 + sparkleAlphas[5], 0 + sparkleAlphas[6], 0 + sparkleAlphas[7], 0);
		glVertex2f((1 - (sparkleAlphas[5] + 0.2))*sparkleCenters[5][0] + (sparkleAlphas[5] + 0.2) * sparklePositions[5][0], (1 - (sparkleAlphas[5] + 0.2))*sparkleCenters[5][1] + (sparkleAlphas[5] + 0.2) * sparklePositions[5][1]);
		// Spark 7
		glColor4f(0 + sparkleAlphas[6], 0 + sparkleAlphas[7], 0 + sparkleAlphas[8], 0);
		glVertex2f((1 - sparkleAlphas[6])*sparkleCenters[6][0] + sparkleAlphas[6] * sparklePositions[6][0], (1 - sparkleAlphas[6])*sparkleCenters[6][1] + sparkleAlphas[6] * sparklePositions[6][1]);
		glColor4f(0 + sparkleAlphas[6], 0 + sparkleAlphas[7], 0 + sparkleAlphas[8], 1);
		glVertex2f((1 - (sparkleAlphas[6] + 0.2))*sparkleCenters[6][0] + (sparkleAlphas[6] + 0.2) * sparklePositions[6][0], (1 - (sparkleAlphas[6] + 0.2))*sparkleCenters[6][1] + (sparkleAlphas[6] + 0.2) * sparklePositions[6][1]);
		// Spark 8
		glColor4f(0 + sparkleAlphas[7], 0 + sparkleAlphas[8], 0 + sparkleAlphas[9], 0);
		glVertex2f((1 - sparkleAlphas[7])*sparkleCenters[7][0] + sparkleAlphas[7] * sparklePositions[7][0], (1 - sparkleAlphas[7])*sparkleCenters[7][1] + sparkleAlphas[7] * sparklePositions[7][1]);
		glColor4f(0 + sparkleAlphas[7], 0 + sparkleAlphas[8], 0 + sparkleAlphas[9], 1);
		glVertex2f((1 - (sparkleAlphas[7] + 0.2))*sparkleCenters[7][0] + (sparkleAlphas[7] + 0.2) * sparklePositions[7][0], (1 - (sparkleAlphas[7] + 0.2))*sparkleCenters[7][1] + (sparkleAlphas[7] + 0.2) * sparklePositions[7][1]);
		// Spark 9
		glColor4f(0 + sparkleAlphas[8], 0 + sparkleAlphas[9], 0 + sparkleAlphas[0], 0);
		glVertex2f((1 - sparkleAlphas[8])*sparkleCenters[8][0] + sparkleAlphas[8] * sparklePositions[8][0], (1 - sparkleAlphas[8])*sparkleCenters[8][1] + sparkleAlphas[8] * sparklePositions[8][1]);
		glColor4f(0 + sparkleAlphas[8], 0 + sparkleAlphas[9], 0 + sparkleAlphas[0], 1);
		glVertex2f((1 - (sparkleAlphas[8] + 0.2))*sparkleCenters[8][0] + (sparkleAlphas[8] + 0.2) * sparklePositions[8][0], (1 - (sparkleAlphas[8] + 0.2))*sparkleCenters[8][1] + (sparkleAlphas[8] + 0.2) * sparklePositions[8][1]);
		// Spark 10
		glColor4f(0 + sparkleAlphas[9], 0 + sparkleAlphas[0], 0 + sparkleAlphas[1], 0);
		glVertex2f((1 - sparkleAlphas[9])*sparkleCenters[9][0] + sparkleAlphas[9] * sparklePositions[9][0], (1 - sparkleAlphas[9])*sparkleCenters[9][1] + sparkleAlphas[9] * sparklePositions[9][1]);
		glColor4f(0 + sparkleAlphas[9], 0 + sparkleAlphas[0], 0 + sparkleAlphas[1], 1);
		glVertex2f((1 - (sparkleAlphas[9] + 0.2))*sparkleCenters[9][0] + (sparkleAlphas[9] + 0.2) * sparklePositions[9][0], (1 - (sparkleAlphas[9] + 0.2))*sparkleCenters[9][1] + (sparkleAlphas[9] + 0.2) * sparklePositions[9][1]);

		glEnd();
	}

	// Draw the birds in front of the letter.
	// Bird 3
	glBegin(GL_LINES);
	glColor3f(0, 0, 0);
	glVertex2f(birdPositions[2][0], birdPositions[2][1]);
	glVertex2f(birdPositions[2][0] + birdWidth, birdPositions[2][1]);
	glEnd();
	glBegin(GL_TRIANGLES);
	glVertex2f(birdPositions[2][0] + 4, birdPositions[2][1]);
	glVertex2f(birdPositions[2][0] + 2, birdPositions[2][1] + wingOffset[2]);
	glVertex2f(birdPositions[2][0] + 10, birdPositions[2][1]);
	glEnd();
	glBegin(GL_TRIANGLES);
	glVertex2f(birdPositions[2][0] + 7, birdPositions[2][1]);
	glVertex2f(birdPositions[2][0] + 8, birdPositions[2][1] + wingOffset[2]);
	glVertex2f(birdPositions[2][0] + 12, birdPositions[2][1]);
	glEnd();
	// Bird 3
	glBegin(GL_LINES);
	glColor3f(0, 0, 0);
	glVertex2f(birdPositions[3][0], birdPositions[3][1]);
	glVertex2f(birdPositions[3][0] + birdWidth, birdPositions[3][1]);
	glEnd();
	glBegin(GL_TRIANGLES);
	glVertex2f(birdPositions[3][0] + 4, birdPositions[3][1]);
	glVertex2f(birdPositions[3][0] + 2, birdPositions[3][1] + wingOffset[3]);
	glVertex2f(birdPositions[3][0] + 10, birdPositions[3][1]);
	glEnd();
	glBegin(GL_TRIANGLES);
	glVertex2f(birdPositions[3][0] + 7, birdPositions[3][1]);
	glVertex2f(birdPositions[3][0] + 8, birdPositions[3][1] + wingOffset[3]);
	glVertex2f(birdPositions[3][0] + 12, birdPositions[3][1]);
	glEnd();

	// Cloud 1
	glBegin(GL_POLYGON);
	glColor4f(1, 1, 1, 0.8);
	glVertex2f(cloudOffset[0] + cloudDistanceMoved[0], 475);
	glVertex2f(cloudOffset[0] + cloudDistanceMoved[0], 500);
	glVertex2f((cloudOffset[0] + cloudDistanceMoved[0]) + cloudLength[0], 500);
	glVertex2f((cloudOffset[0] + cloudDistanceMoved[0]) + cloudLength[0], 475);
	glEnd();

	// send all output to display 
	glutSwapBuffers();
}


/************************************************************************

Function:		main

Description:	Sets up the openGL rendering context and the windowing
system, then begins the display loop.

*************************************************************************/
void main(int argc, char** argv)
{
	// initialize the toolkit
	glutInit(&argc, argv);
	// set display mode
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	// set window size
	glutInitWindowSize(750, 750);
	// set window position on screen
	glutInitWindowPosition(100, 150);
	// Seed for random generation.
	srand(time(NULL));
	// open the screen window
	glutCreateWindow("Personal ScreenSaver - Adam Al Jumaily - A00362836");
	// enable blending
	glEnable(GL_BLEND);
	// enable smooth line drawing.
	glEnable(GL_LINE_SMOOTH);
	// set blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// register redraw function
	glutDisplayFunc(display);
	// set mouse click handler
	glutMouseFunc(mouseClick);
	// set idle function
	glutIdleFunc(idle);
	// set window mode to 2D orthographica and set the window size 
	gluOrtho2D(0.0, 500.0, 0.0, 500.0);
	// set lineWidth
	glLineWidth(3);
	// go into a perpetual loop
	glutMainLoop();
}
