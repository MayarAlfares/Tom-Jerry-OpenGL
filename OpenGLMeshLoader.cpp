#include <math.h>
#include <random>
#include <ctime>
#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>

int WIDTH = 1280;
int HEIGHT = 720;

#define DEG2RAD(a) (a * 0.0174532925)
GLuint tex;
char title[] = "Tom and Jerry";

// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.01;
GLdouble zFar = 5000;

/// Model varibales
Model_3DS model_mouse;
Model_3DS model_brick;
Model_3DS model_cheese;
Model_3DS model_plant;


Model_3DS model_cat;
Model_3DS model_strawberry;
Model_3DS model_trophy;
Model_3DS model_oven;
Model_3DS model_kitchen;
Model_3DS model_window;
Model_3DS model_table;
Model_3DS model_teaSet;
Model_3DS model_cabinet;
Model_3DS model_fridge;
Model_3DS model_carpet;
Model_3DS model_fountain;
Model_3DS model_flower;
class Vector
{

public:
	float x, y, z;

	Vector(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector operator+(Vector &v) {
		return Vector(x + v.x, y + v.y, z + v.z);
	}

	Vector operator-(Vector &v) {
		return Vector(x - v.x, y - v.y, z - v.z);
	}

	Vector operator*(float n) {
		return Vector(x * n, y * n, z * n);
	}

	Vector operator/(float n) {
		return Vector(x / n, y / n, z / n);
	}

	Vector unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector cross(Vector v) {
		return Vector(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}

};

class Camera {
public:
	Vector eye, center, up;

	Camera(float eyeX = 1.0f, float eyeY = 1.0f, float eyeZ = 1.0f, float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
		eye = Vector(eyeX, eyeY, eyeZ);
		center = Vector(centerX, centerY, centerZ);
		up = Vector(upX, upY, upZ);
	}

	void moveX(float d) {
		Vector right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		Vector view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a) {
		Vector view = (center - eye).unit();
		Vector right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector view = (center - eye).unit();
		Vector right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}

	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}
};

Vector Eye(20, 5, 20);
Vector At(0, 0, 0);
Vector Up(0, 1, 0);

int cameraZoom = 0;
Camera camera;

// Model Variables
Model_3DS model_house;
Model_3DS model_tree;

// Textures
GLTexture tex_ground;
GLTexture tex_ground2;
GLTexture tex_sky;

//Jerry (Player) Coordinates
int JerryX = 0;
int JerryY = 0;
int JerryZ = -20;

//Obstacles coordinates
int Obstacle1X = 0;
int Obstacle1Y = 0;
int Obstacle1Z = 0;

int Obstacle2X = 0;
int Obstacle2Y = 0;
int Obstacle2Z = 0;

int Obstacle3X = 0;
int Obstacle3Y = 0;
int Obstacle3Z = 0;

//Pickup coordinates
int Pickup1X = 0;
int Pickup1Y = 0;
int Pickup1Z = 0;

int Pickup2X = 0;
int Pickup2Y = 0;
int Pickup2Z = 0;

int Pickup3X = 0;
int Pickup3Y = 0;
int Pickup3Z = 0;

float move = 0.0;
float TransTimer = 0; //the pickups animation

int numberOfPickups = 0;


//flags 
int levelNumber = 1;
int level2 = 0;
int level21 = 0;

int level22 = 0;
int level23 = 0;


int level221 = 0;

int level222 = 0;
int level223 = 0;

bool Level2 = false;
int m = 0;
bool moving = false;
//winner trophy motion

float trophyY = 25.0;
float congrats = 3200;
int textColor = 0;
//camera flag
bool FirstPerson = false;
bool ThirdPerson = false;
float lightR = 0;
float lightG = 0;
float lightB = 0;
// The next global variable controls the animation's state and speed.
float CurrentAngle = 0.0f;         // Angle in degrees
float AnimateStep = 0.5f;         // Rotation step per update
float Noemit[4] = { 0.0, 0.0, 0.0, 1.0 };
//=======================================================================
// Lighting Configuration Function
//=======================================================================
void InitLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources
	glEnable(GL_LIGHT0);

	// Define Light source 0 ambient light
	GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	// Define Light source 0 diffuse light
	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	// Define Light source 0 Specular light
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	// Finally, define light source 0 position in World Space
	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}
//setting up light
// Lighting values
float ambientLight[4] = { 0.2, 0.2, 0.2, 1.0 };
float Lt0amb[4] = { 0.3, 0.3, 0.3, 1.0 };
float Lt0diff[4] = { 1.0, 1.0, 1.0, 1.0 };
float Lt0spec[4] = { 1.0, 1.0, 1.0, 1.0 };

float zeroPos[4] = { 0, 0, 0, 1 };         // Origin (homogeneous representation)
float dirI[4] = { 1, 0, 0, 0 };            // Direction of unit vector I (point at infinity)

int LightIsPositional = 0;

int RunMode = 1;


//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:  Angle between the bottom and top of the projectors, in degrees.
	//
	// aspectRatio:
	//Ratio of width to height of the clipping plane.
	//
	// zNear and zFar:
	//Specify the front and back clipping planes distances from camera.
	//
	//*******************************************************************************************//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	//gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	gluLookAt(camera.eye.x, camera.eye.y, camera.eye.z, camera.center.x, camera.center.y, camera.center.z, camera.up.x, camera.up.y, camera.up.z);
	//*******************************************************************************************//
	// EYE (ex, ey, ez): defines the location of the camera.
	//
	// AT (ax, ay, az):
	//denotes the direction where the camera is aiming at.
	//
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.
	//
	//*******************************************************************************************//

	InitLightSource();

	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}



//=======================================================================
// Render Functions
//=======================================================================
void RenderGround()
{
	glDisable(GL_LIGHTING);
	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);
	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);
	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);
	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	// Set quad normal direction.
	glTexCoord2f(0, 0);
	// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -20);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, 20);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, 20);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);
	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);
	// Set material back to white instead of grey used for the ground texture.
}



void RenderGround2()
{
	glDisable(GL_LIGHTING);
	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);
	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);
	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground2.texture[0]);
	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	// Set quad normal direction.
	glTexCoord2f(0, 0);
	// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -20);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, 20);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, 20);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);
	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);
	// Set material back to white instead of grey used for the ground texture.
}


void RenderGroundKitchen() //80x80
{
	glDisable(GL_LIGHTING);
	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);
	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);
	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_sky.texture[0]);
	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	// Set quad normal direction.
	glTexCoord2f(0, 0);
	// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -20);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, 20);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, 20);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);
	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);
	// Set material back to white instead of grey used for the ground texture.
}
//player animation
int flag = 1;
void update(int value) {
	if (flag && moving)
	{
		move += 0.01f;
		if (move > 0.2)
		{
			flag = 0;

		}
	}
	if (!flag && moving)
	{
		move -= 0.01f;
		if (move < 0)
		{
			flag = 1;

		}
		if (move == 0) {
			moving = false;
		}
	}

	glutPostRedisplay(); //Tell GLUT that the display has changed

	//Tell GLUT to call update again in 25 milliseconds
	glutTimerFunc(25, update, 0);
}

//=======================================================================
// Display Function
//=======================================================================

void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, aspectRatio, zNear, zFar);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}
void output(GLfloat x, GLfloat y, char *format, ...)
{
	va_list args;
	char buffer[200], *p;

	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);
	glPushMatrix();
	glColor3f(textColor, textColor, textColor);
	glLineWidth(4);
	glTranslatef(x, y, 0);
	for (p = buffer; *p; p++)
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
	glPopMatrix();
}
void myDisplay(void)
{
	setupCamera();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	GLfloat lightIntensity[] = { lightR, 0, 0, 1.0f };
	GLfloat lightPosition[] = { 0.0f, 100.0f, lightG, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightIntensity);
	if (RunMode == 1) {
		// Calculate animation parameters
		CurrentAngle += AnimateStep;
		if (CurrentAngle > 360.0) {
			CurrentAngle -= 360.0*floor(CurrentAngle / 360.0);   // Don't allow overflow
		}
	}
	// Position the light (before drawing the illuminated objects)
	glPushMatrix();
	glRotatef(CurrentAngle, 0.0, 0.0, 1.0);      // Rotate through animation angle
	glTranslatef(10.0, 5.0, 0.0);               // Translate rotation center to origin


	if (LightIsPositional == 1) {
		glLightfv(GL_LIGHT0, GL_POSITION, zeroPos);   // Position is transformed by ModelView matrix
	}
	else {
		glLightfv(GL_LIGHT0, GL_POSITION, dirI);      // Direction is transformed by ModelView matrix
	}
	glPopMatrix();

	// Draw Ground Garden

	glPushMatrix();
	glTranslated(level2, level2, level2);
	RenderGround();
	glPopMatrix();

	glPushMatrix();

	// Draw Player Model
	glPushMatrix();
	glTranslated(m, move, 0);
	glPushMatrix();
	glRotated(90, 1, 0, 0);
	glTranslated(JerryX, JerryZ, JerryY);
	glScalef(0.5, 0.5, 0.5);
	model_mouse.Draw();
	glPopMatrix();
	glPopMatrix();


	//draw fountain 


	glPushMatrix();
	glTranslated(0, level2, 0);
	model_fountain.Draw();
	glPopMatrix();
	//draw plants 

	glPushMatrix();
	glTranslated(-5, level2, -15);
	glPushMatrix();
	glRotated(90, 1, 0, 0);
	glScaled(20, 20, 20);
	model_flower.Draw();
	glPopMatrix();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-1, level2, -15);
	glPushMatrix();
	glRotated(90, 1, 0, 0);
	glScaled(20, 20, 20);
	model_flower.Draw();
	glPopMatrix();
	glPopMatrix();



	glPushMatrix();
	glTranslated(5, level2, -15);
	glPushMatrix();
	glRotated(90, 1, 0, 0);
	glScaled(20, 20, 20);
	model_flower.Draw();
	glPopMatrix();
	glPopMatrix();


	glPushMatrix();
	glTranslated(9, level2, -15);
	glPushMatrix();
	glRotated(90, 1, 0, 0);
	glScaled(20, 20, 20);
	model_flower.Draw();
	glPopMatrix();
	glPopMatrix();




	glPushMatrix();
	glTranslated(13, level2, -15);
	glPushMatrix();
	glRotated(90, 1, 0, 0);
	glScaled(20, 20, 20);
	model_flower.Draw();
	glPopMatrix();
	glPopMatrix();


	glPushMatrix();
	glTranslated(-8, 0 + level2, -15);
	glRotated(90, 1, 0, 0);
	glScaled(0.07, 0.07, 0.07);
	model_tree.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-17, 0 + level2, -15);
	glPushMatrix();
	glRotated(90, 0, 1, 0);
	glPushMatrix();
	glTranslated(0, 2, 0);
	glRotated(90, 1, 0, 0);
	glScaled(0.06, 0.06, 0.06);
	model_plant.Draw();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();


	glPushMatrix();
	glTranslated(-17, 0 + level2, -5);
	glPushMatrix();
	glRotated(90, 0, 1, 0);
	glPushMatrix();
	glTranslated(0, 2, 0);
	glRotated(90, 1, 0, 0);
	glScaled(0.06, 0.06, 0.06);
	model_plant.Draw();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-17, 0 + level2, 5);
	glPushMatrix();
	glRotated(90, 0, 1, 0);
	glPushMatrix();
	glTranslated(0, 2, 0);
	glRotated(90, 1, 0, 0);
	glScaled(0.06, 0.06, 0.06);
	model_plant.Draw();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();


	glPushMatrix();
	glTranslated(-17, 0 + level2, 15);
	glPushMatrix();
	glRotated(90, 0, 1, 0);
	glPushMatrix();
	glTranslated(0, 2, 0);
	glRotated(90, 1, 0, 0);
	glScaled(0.06, 0.06, 0.06);
	model_plant.Draw();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();

	// Draw obstacles (bricks) Model
	glPushMatrix();
	glTranslated(Obstacle1X + level2, Obstacle1Y + level2, Obstacle1Z + level2);
	glScaled(0.03, 0.03, 0.03);
	model_brick.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslated(Obstacle2X + level2, Obstacle2Y + level2, Obstacle2Z + level2);
	glScaled(0.03, 0.03, 0.03);
	model_brick.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslated(Obstacle3X + level2, Obstacle3Y + level2, Obstacle3Z + level2);
	glScaled(0.03, 0.03, 0.03);
	model_brick.Draw();
	glPopMatrix();


	// Draw pickups Model
	glPushMatrix();
	glTranslated(Pickup1X + level21, Pickup1Y + level21 + TransTimer, Pickup1Z + level21);
	glRotatef(90.f, 1, 0, 0);
	glScaled(15, 15, 15);
	model_strawberry.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslated(Pickup2X + level22, Pickup2Y + level22 + TransTimer, Pickup2Z + level22);
	glRotatef(90.f, 1, 0, 0);
	glScaled(15, 15, 15);
	model_strawberry.Draw();
	glPopMatrix();

	glPushMatrix();
	glTranslated(Pickup3X + level23, Pickup3Y + level23 + TransTimer, Pickup3Z + level23);
	glRotatef(90.f, 1, 0, 0);
	glScaled(15, 15, 15);
	model_strawberry.Draw();
	glPopMatrix();

	//Rendering Level 2
	if (Level2) {
		level2 = 2000;
		textColor = 1;
		RenderGround2();

		glPushMatrix();
		glTranslated(0.0, 20.0, -20.0);
		glRotated(90, 1, 0, 0);
		RenderGround2();
		glPopMatrix();


		glPushMatrix();
		glTranslated(-20.0, 20.0, 0.0);
		glRotated(90, 0, 0, 1);
		RenderGround2();
		glPopMatrix();


		glPushMatrix();
		glRotated(90, 1, 0, 0);
		glTranslated(0, -17, 0);
		glScalef(7, 7, 7);
		model_oven.Draw();
		glPopMatrix();


		glPushMatrix();
		glTranslated(0, 0, -8);
		glRotated(90, 1, 0, 0);
		glScaled(0.05, 0.05, 0.05);
		model_carpet.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 0, 8);
		glRotated(90, 1, 0, 0);
		glScaled(0.05, 0.05, 0.05);
		model_carpet.Draw();
		glPopMatrix();
		glPushMatrix();
		glTranslated(10, 0, 0);
		glPushMatrix();
		glRotated(90, 1, 0, 0);
		glTranslated(0, -17, 0);
		glScalef(6, 6, 6);
		model_cabinet.Draw();
		glPopMatrix();
		glPopMatrix();


		glPushMatrix();
		glRotated(90, 1, 0, 0);
		glTranslated(-7, -15, -8);
		glScalef(7, 7, 7);
		model_kitchen.Draw();

		glPopMatrix();



		glPushMatrix();
		glTranslated(-20, 4, -3);
		glPushMatrix();
		glRotated(90, 0, 1, 0);
		glPushMatrix();
		glRotated(90, 1, 0, 0);
		glScalef(3.5, 3.5, 3.5);
		model_window.Draw();
		glPopMatrix();
		glPopMatrix();
		glPopMatrix();


		glPushMatrix();
		glTranslated(-20, 0, -12);
		glPushMatrix();
		glRotated(90, 0, 1, 0);
		glPushMatrix();
		glRotated(90, 1, 0, 0);
		glScalef(6.5, 6.5, 6.5);
		model_fridge.Draw();
		glPopMatrix();
		glPopMatrix();
		glPopMatrix();




		glPushMatrix();
		glTranslated(-20, 4, 8);
		glPushMatrix();
		glRotated(90, 0, 1, 0);
		glPushMatrix();
		glRotated(90, 1, 0, 0);
		glScalef(3.5, 3.5, 3.5);
		model_window.Draw();
		glPopMatrix();
		glPopMatrix();
		glPopMatrix();

		glPushMatrix();
		glTranslated(-17, 0, 3);
		glPushMatrix();
		glRotated(90, 1, 0, 0);
		glScaled(9, 9, 9);
		model_table.Draw();
		glPopMatrix();
		glPopMatrix();



		glPushMatrix();
		glTranslated(-17, 4, 3);
		glPushMatrix();
		glRotated(90, 1, 0, 0);
		glScaled(9, 9, 9);
		model_teaSet.Draw();
		glPopMatrix();
		glPopMatrix();



		glPushMatrix();

		glTranslated(Obstacle1X, Obstacle1Y, Obstacle1Z);
		glScaled(0.3, 0.3, 0.3);
		glRotated(90, 1, 0, 0);
		glRotated(-35, 0, 0, 1);
		model_cat.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslated(Obstacle2X, Obstacle2Y, Obstacle2Z);
		glScaled(0.3, 0.3, 0.3);
		glRotated(90, 1, 0, 0);
		glRotated(-35, 0, 0, 1);
		model_cat.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslated(Obstacle3X, Obstacle3Y, Obstacle3Z);
		glScaled(0.3, 0.3, 0.3);
		glRotated(90, 1, 0, 0);
		glRotated(-35, 0, 0, 1);
		model_cat.Draw();
		glPopMatrix();


		glPushMatrix();
		glTranslated(Pickup1X + level221, Pickup1Y + level221 + TransTimer, Pickup1Z + level221);
		glRotatef(90.f, 1, 0, 0);
		glRotatef(180.f, 0, 0, 1);
		glScaled(12, 12, 12);
		model_cheese.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslated(Pickup2X + level222, Pickup2Y + level222 + TransTimer, Pickup2Z + level222);

		glRotatef(90.f, 1, 0, 0);
		glRotatef(180.f, 0, 0, 1);
		glScaled(12, 12, 12);
		model_cheese.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslated(Pickup3X + level223, Pickup3Y + level223 + TransTimer, Pickup3Z + level223);

		glRotatef(90.f, 1, 0, 0);
		glRotatef(180.f, 0, 0, 1);
		glScaled(12, 12, 12);
		model_cheese.Draw();
		glPopMatrix();

	}


	//Render Trophy
	glPushMatrix();
	glTranslated(0, trophyY, 3);
	glPushMatrix();
	glRotated(90, 1, 0, 0);
	glRotated(-35, 0, 0, 1);
	glTranslated(0, 7, 0);
	glScalef(10, 10, 10);
	model_trophy.Draw();
	glPopMatrix();
	glPopMatrix();
	//}


	//sky box
	glPushMatrix();

	GLUquadricObj * qobj;
	qobj = gluNewQuadric();
	glTranslated(50, 0, 0);
	glRotated(90, 1, 0, 1);
	glBindTexture(GL_TEXTURE_2D, tex);
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 100, 100, 100);
	gluDeleteQuadric(qobj);


	glPopMatrix();


	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 3000, 0, 3000);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	output(35, 2600, "Level:%d", levelNumber);
	output(35, 2450, "Score:%d", numberOfPickups);
	glPushMatrix();
	glTranslated(0, congrats, 0);
	output(940, 0, "Congratulations !!");
	glPopMatrix();



	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();

	glutSwapBuffers();
}

void HelperPickups() {
	if (JerryX == Pickup1X && JerryZ == Pickup1Z && level21 < 500) {
		numberOfPickups++;
		level21 = 1000;
		PlaySound("sounds/pickup.wav", NULL, SND_ASYNC | SND_FILENAME);
	}

	if (JerryX == Pickup2X && JerryZ == Pickup2Z && level22 < 500) {
		numberOfPickups++;
		level22 = 1000;
		PlaySound("sounds/pickup.wav", NULL, SND_ASYNC | SND_FILENAME);

	}

	if (JerryX == Pickup3X && JerryZ == Pickup3Z && level23 < 500) {
		numberOfPickups++;
		level23 = 1000;
		PlaySound("sounds/pickup.wav", NULL, SND_ASYNC | SND_FILENAME);

	}

	if (JerryX == Pickup1X && JerryZ == Pickup1Z && Level2 == true && level21 > 500 && level221 < 500) {
		numberOfPickups++;
		level221 = 1000;
		PlaySound("sounds/pickup.wav", NULL, SND_ASYNC | SND_FILENAME);

	}

	if (JerryX == Pickup2X && JerryZ == Pickup2Z && Level2 == true && level22 > 500 && level222 < 500) {
		numberOfPickups++;
		level222 = 1000;
		PlaySound("sounds/pickup.wav", NULL, SND_ASYNC | SND_FILENAME);

	}

	if (JerryX == Pickup3X && JerryZ == Pickup3Z && Level2 == true && level23 > 500 && level223 < 500) {
		numberOfPickups++;
		level223 = 1000;
		PlaySound("sounds/pickup.wav", NULL, SND_ASYNC | SND_FILENAME);

	}

	if (numberOfPickups == 3) {
		Level2 = true;
		level2 = 2000;
		levelNumber = 2;
		int JerryX = 0;
		int JerryY = 0;
		int JerryZ = -20;
		m = 3;
	}
	if (numberOfPickups == 6) {
		PlaySound("sounds/win.wav", NULL, SND_ASYNC | SND_FILENAME);
	}
}

//=======================================================================
// Keyboard Function
//=======================================================================
void myKeyboard(unsigned char button, int x, int y)
{
	float d = 0.1;

	switch (button)
	{

	case'd':
	{
		if (JerryX > -15) { //within boundaries
			if (!((JerryX - 5 == Obstacle1X && JerryZ == Obstacle1Z) // no collision with obstacles
				|| (JerryX - 5 == Obstacle2X && JerryZ == Obstacle2Z)
				|| (JerryX - 5 == Obstacle3X && JerryZ == Obstacle3Z))) {
				JerryX -= 5;
				HelperPickups();
				moving = true;
			}
			else if (((JerryX - 5 == Obstacle1X && JerryZ == Obstacle1Z) // collision with obstacles
				|| (JerryX - 5 == Obstacle2X && JerryZ == Obstacle2Z)
				|| (JerryX - 5 == Obstacle3X && JerryZ == Obstacle3Z))) {
				PlaySound("sounds/collision.wav", NULL, SND_ASYNC | SND_FILENAME);
				moving = false;
			}
		}
	}
	break;
	case'a':
		if (JerryX < 15) {
			if (!((JerryX + 5 == Obstacle1X && JerryZ == Obstacle1Z)
				|| (JerryX + 5 == Obstacle2X && JerryZ == Obstacle2Z)
				|| (JerryX + 5 == Obstacle3X && JerryZ == Obstacle3Z))) {
				JerryX += 5;

				HelperPickups();
				moving = true;
			}
			else if (((JerryX + 5 == Obstacle1X && JerryZ == Obstacle1Z)
				|| (JerryX + 5 == Obstacle2X && JerryZ == Obstacle2Z)
				|| (JerryX + 5 == Obstacle3X && JerryZ == Obstacle3Z))) {
				PlaySound("sounds/collision.wav", NULL, SND_ASYNC | SND_FILENAME);
				moving = false;
			}
		}
		break;
	case's':
		if (JerryZ > -15)
		{
			if (!((JerryZ - 5 == Obstacle1Z && JerryX == Obstacle1X)
				|| (JerryZ - 5 == Obstacle2Z && JerryX == Obstacle2X)
				|| (JerryZ - 5 == Obstacle3Z && JerryX == Obstacle3X))) {
				JerryZ -= 5;

				HelperPickups();
				moving = true;
			}
			else if (((JerryZ - 5 == Obstacle1Z && JerryX == Obstacle1X)
				|| (JerryZ - 5 == Obstacle2Z && JerryX == Obstacle2X)
				|| (JerryZ - 5 == Obstacle3Z && JerryX == Obstacle3X))) {
				PlaySound("sounds/collision.wav", NULL, SND_ASYNC | SND_FILENAME);
				moving = false;
			}
		}
		break;
	case 'w':
		if (JerryZ < 15) {
			if (!((JerryZ + 5 == Obstacle1Z && JerryX == Obstacle1X)
				|| (JerryZ + 5 == Obstacle2Z && JerryX == Obstacle2X)
				|| (JerryZ + 5 == Obstacle3Z && JerryX == Obstacle3X))) {
				JerryZ += 5;
				HelperPickups();
				moving = true;
			}
			else if (((JerryZ + 5 == Obstacle1Z && JerryX == Obstacle1X)
				|| (JerryZ + 5 == Obstacle2Z && JerryX == Obstacle2X)
				|| (JerryZ + 5 == Obstacle3Z && JerryX == Obstacle3X))) {
				PlaySound("sounds/collision.wav", NULL, SND_ASYNC | SND_FILENAME);
				moving = false;
			}
		}
		break;

		//case 'w':
		//  camera.moveY(d);
		//  break;
		//  case 's':
		//  camera.moveY(-d);
		//  break;
		//  case 'a':
		//  camera.moveX(d);
		//  break;
		//  case 'd':
		//  camera.moveX(-d);
		//  break;
		//  case 'q':
		//  camera.moveZ(d);
		//  break;
		//  case 'e':
		//  camera.moveZ(-d);
		//  break;

	case 'z': {
		FirstPerson = true;
		ThirdPerson = false;
	}
			  break;
	case 'x': {
		FirstPerson = false;
		ThirdPerson = true;
	}
			  break;
	case'v': {

		camera.eye.x = 20.572256;
		camera.eye.y = 20.572256;
		camera.eye.z = 20.572256;
		camera.center.x = 19.958872;
		camera.center.y = 19.995344;
		camera.center.z = 20.032875;
		camera.up.x = 0.000000;
		camera.up.y = 1.000000;
		camera.up.z = 0.000000;
		break;
	}
	case 27:
		exit(0);
		break;
	default: {
		moving = false;
		break;
	}
	}

	//if (button == 'p') {
	//  printf("camera.eye.x = %f;\n", camera.eye.x);
	//  printf("camera.eye.y = %f;\n", camera.eye.y);
	//  printf("camera.eye.z = %f;\n", camera.eye.z);


	//  printf("camera.center.x = %f;\n", camera.center.x);
	//  printf("camera.center.y = %f;\n", camera.center.y);
	//  printf("camera.center.z = %f;\n", camera.center.z);


	//  printf("camera.up.x = %f;\n", camera.up.x);
	//  printf("camera.up.y = %f;\n", camera.up.y);
	//  printf("camera.up.z = %f;\n", camera.up.z);
	//}

	if (ThirdPerson) {
		camera.eye.x = JerryX - 0.284;
		camera.eye.y = 24.811;
		camera.eye.z = 2 + JerryZ;
		camera.center.x = JerryX - 0.2966;
		camera.center.y = 23.828;
		camera.center.z = 2.2 + JerryZ;
		camera.up.x = 0.024;
		camera.up.y = 0.185;
		camera.up.z = 0.982;
	}
	if (FirstPerson) {
		camera.eye.x = JerryX - 0.2;
		camera.eye.y = 4;
		camera.eye.z = 1 + JerryZ;
		camera.center.x = JerryX;
		camera.center.y = 4;
		camera.center.z = 2 + JerryZ;
		camera.up.x = 0.000000;
		camera.up.y = 1.000000;
		camera.up.z = 0.16;
	}

	glutPostRedisplay();
}

//=======================================================================
// Motion Function
//=======================================================================
void myMotion(int x, int y)
{
	y = HEIGHT - y;

	if (cameraZoom - y > 0)
	{
		Eye.x += -0.1;
		Eye.z += -0.1;
	}
	else
	{
		Eye.x += 0.1;
		Eye.z += 0.1;
	}

	cameraZoom = y;

	glLoadIdentity();
	//Clear Model_View Matrix

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	//Setup Camera with modified paramters

	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glutPostRedisplay();
	//Re-draw scene 
}

//=======================================================================
// Mouse Function
//=======================================================================
void myMouse(int button, int state, int x, int y)
{
	FirstPerson = false;
	ThirdPerson = false;
	camera.eye.x = 20.572256;
	camera.eye.y = 20.572256;
	camera.eye.z = 20.572256;
	camera.center.x = 19.958872;
	camera.center.y = 19.995344;
	camera.center.z = 20.032875;
	camera.up.x = 0.000000;
	camera.up.y = 1.000000;
	camera.up.z = 0.000000;
}

void Special(int key, int x, int y) {
	float a = 1.0;

	switch (key) {
	case GLUT_KEY_UP:
		camera.rotateX(a);
		break;
	case GLUT_KEY_DOWN:
		camera.rotateX(-a);
		break;
	case GLUT_KEY_LEFT:
		camera.rotateY(a);
		break;
	case GLUT_KEY_RIGHT:
		camera.rotateY(-a);
		break;
	}

	glutPostRedisplay();
}

//=======================================================================
// Reshape Function
//=======================================================================
void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}



//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	// Loading Model files
	model_mouse.Load("Models/Jerry/JERRY.3ds");
	model_brick.Load("Models/brick/Brick Block.3ds");
	model_cheese.Load("Models/cheese/cheese.3ds");
	model_cat.Load("Models/TOM/TOM.3ds");
	model_strawberry.Load("Models/berry/Strawberry.3ds");
	model_trophy.Load("Models/trophy/ocup.3ds");
	model_oven.Load("Models/oven/kitchen_oven.3ds");
	model_kitchen.Load("Models/kitchen1/pantry_shelves.3ds");
	model_window.Load("Models/window/windowInterior_townHall.3ds");
	model_table.Load("Models/table/otukue2.3ds");
	model_teaSet.Load("Models/teaSet/otukue.3ds");
	model_cabinet.Load("Models/cabinet/cleaning_cabinet.3ds");
	model_fridge.Load("Models/fridge/kitchen_icebox.3ds");
	model_carpet.Load("Models/carpet/Carpet_C.3ds");
	model_plant.Load("Models/plant/skPlantsWindowBoxMesh.3ds");
	model_tree.Load("Models/tree/N64_Tree.3ds");
	model_fountain.Load("Models/fountain/fontmm.3ds");
	model_flower.Load("Models/flower/INJ-GAU_iOS_WEAPON_Harley_Quinn's_Rose_Injustice.3ds");
	// Loading texture files
	tex_ground.Load("Textures/ground.bmp");
	tex_ground2.Load("Textures/ground2.bmp");

	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
}


//=======================================================================
//animations
//=======================================================================

void Anim() {
	float timerRate = 0.001;
	TransTimer += timerRate;

	if (TransTimer > 2) {
		TransTimer = 1.9;
		timerRate *= -1;
	}

	if (TransTimer < 0) {
		TransTimer = 0.1;
		timerRate *= -1;
	}

	if (AnimateStep < 5.0) {         // Don't let speed get very big.
		AnimateStep *= sqrt(2.0);      // Increase the angle increment
	}

	if (AnimateStep > 1.0e-3) {      // Avoid underflow problems.
		AnimateStep /= sqrt(2.0);   // Decrease the angle increment
	}

	if (numberOfPickups == 6 && trophyY >= 0) { //Trophy falls down
		trophyY -= 0.009;
		congrats--;
	}

	if (numberOfPickups == 6 && congrats >= 2000) { //Trophy falls down
		congrats--;
	}


	lightR += 0.001;
	lightG += 0.001;
	lightB += 0.001;

	glutPostRedisplay();
}



//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(0, 0);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);

	glutKeyboardFunc(myKeyboard);

	glutSpecialFunc(Special);

	glutMotionFunc(myMotion);

	glutMouseFunc(myMouse);

	glutReshapeFunc(myReshape);

	glutIdleFunc(Anim);

	myInit();

	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);


	Obstacle1X = 0;
	Obstacle1Z = 10;

	Obstacle2X = -5;
	Obstacle2Z = -5;
	Obstacle3X = -10;
	Obstacle3Z = 0;

	Pickup1X = -15;
	Pickup1Z = 10;
	Pickup2X = 5;
	Pickup2Z = -5;
	Pickup3X = 10;
	Pickup3Z = 10;

	glutTimerFunc(25, update, 0);
	glutMainLoop();
}
