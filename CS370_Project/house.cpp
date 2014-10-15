// CS370 - Fall 2014
// Final Project
/***************************
*       Paul Holtz         *
***************************/
/*Controls:
*use the mouse to look around*
w -> move forward
s -> move backward
a -> strafe left
d -> strafe right
f -> toggle fan on/off

Enjoy!

Work List:
-Better tables and chairs
-First floor railings
-Stair railings
-Texture ceiling
-Texture second floor
-Texture stairs
-Better bar & bar backdrop (window and shelf)
-Decrease texture map size where possible

Known Issues:
-Y position mouselook is unbounded
-Movement speed is altered based on where the camera is pointing
-Mouse pointer is not trapped inside window

Git Repo:
https://github.com/pholtz/HOBL.git
git commit -a -m "message"
git push
*/

#ifdef OSX
	#include <GLUT/glut.h>
#else
	#include <GL/glew.h>
	#include <GL/glut.h>
#endif
#include <SOIL/SOIL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lighting.h"
#include "materials.h"
#include "vectorops.h"

//include Paul's house header file
#include "house.h"

// Shader file utility functions
#include "shaderutils.h"

// Shader files
//lighting
GLchar* defaultVertexFile = "defaultvert.vs";
GLchar* defaultFragmentFile = "defaultfrag.fs";
// Shader objects
GLuint defaultShaderProg;

//New Shader Files & Objects
GLchar* vertexFile = "lightvert.vs";
GLchar* fragmentFile = "lightfrag.fs";
GLuint shaderProg;
GLuint num_lights_param;

//TEXTURE STUFF
/////////////////////////////
//texture files & objects
GLchar* texVertexFile = "texturevert.vs";
GLchar* texFragmentFile = "texturefrag.fs";
GLuint textureProg;
GLint texSampler;
// Texture constants
#define SPACE 0
#define EARTH 1
#define MOON 2
#define DR_PEPPER 3
#define LID 4
#define TILE 5
#define WOOD_MID_SIDES 6
#define WOOD_MID_ENTRANCE 7
#define WALL_ART 8
#define WALLPAPER 9
#define WALLPAPER_NARROW 10
#define WALLPAPER_ROT 11
#define FLOOR_TILE 12
#define NO_TEXTURES 13
// Texture indices
GLuint tex_ids[NO_TEXTURES];
// Texture files
char texture_files[13][20] = { "space.jpg", "et.bmp", "mn.bmp", "drp.jpg", "dlid.jpg", "tile.png", "wdflrms.jpg", "wdflrme.jpg", "jwart.jpg", "wallpaper.jpg", "wallpaper2.jpg", "wallpaperrot.jpg", "fltile.jpg" };
/////////////////////////////

#define RAD2DEG (180.0f/3.14159f)
#define DEG2RAD (3.14159f/180.0f)

#define SURFACE 0
#define VERTEX 1

// Component indices
#define X 0
#define Y 1
#define Z 2

//Display List ID's
#define TABLE 1
#define CEILING 2
#define FLOOR 3
#define WALLS 4
#define DOOR 5
#define WINDOW 6
#define ART 7
#define MIRROR 8
#define ROOF 9
#define ENTRANCE 10
#define STAIRS 11
#define MID_FIRST_FLOOR 12
#define BALCONY 13
#define CHAIR 14
#define BAR 15
#define FIRST_FLOOR_REAR 16

// View modes
#define ORTHOGRAPHIC 0
#define PERSPECTIVE 1

// Cube vertices
GLfloat cube[][3] = { { -0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, 0.5f },
{ -0.5f, -0.5f, 0.5f }, { -0.5f, 0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f },
{ 0.5f, 0.5f, 0.5f }, { -0.5f, 0.5f, 0.5f } };

// Normal vertices
GLfloat vnorm[][3] = { { -0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, 0.5f },
{ -0.5f, -0.5f, 0.5f }, { -0.5f, 0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f },
{ 0.5f, 0.5f, 0.5f }, { -0.5f, 0.5f, 0.5f } };

// Global subdivision parameters
int div_level = 0;
int normal_mode = SURFACE;

//camera global variables
GLfloat eye[3] = { 0.0f, 5.0f, 5.0f };
GLfloat at[3] = { 500.0f, 5.0f, 0.0f };
GLfloat up[3] = { 0.0f, 1.0f, 0.0f };

//first-person view global variables
GLfloat radius = 500.0f;
GLfloat start_x = 0;
GLfloat start_y = 0;
GLfloat dx = 0;
GLfloat dy = 0;
char x_pos[32];
char y_pos[32];
char z_pos[32];
char fps_str[32];

//lighting global variables
GLfloat light0_pos[4] = { 50.0f, 30.0f, 45.0f, 1.0f };
GLfloat light1_pos[4] = { -50.0f, 30.0f, 45.0f, 1.0f };
GLfloat light2_pos[4] = { -50.0f, 30.0f, -45.0f, 1.0f };
GLfloat light3_pos[4] = { 50.0f, 30.0f, -45.0f, 1.0f };
GLfloat light0_attenuation[3] = {3.0f, 0.0f, 0.0f};
GLfloat light1_attenuation[3] = {3.0f, 0.0f, 0.0f};
GLfloat light2_attenuation[3] = {3.0f, 0.0f, 0.0f };
GLfloat light3_attenuation[3] = {3.0f, 0.0f, 0.0f };
GLint num_lights = 4;

//animation variables
GLfloat fan_theta = 0;
GLfloat time = 0;
GLfloat lasttime = 0;
GLfloat fps = 0;
GLboolean fan_anim_flag = 0;
GLboolean light_toggle_flag = 0;

//declare quadrics
GLUquadricObj *fan_light = gluNewQuadric();
GLUquadricObj *lightbulb = gluNewQuadric();
GLUquadricObj *bowl = gluNewQuadric();
GLUquadricObj *soda_can;
GLUquadricObj* moon;
GLUquadricObj* earth;

//FUNCTION PROTOTYPES
void display();
void render_Scene();
void keyfunc(unsigned char key, int x, int y);
void passive(int x, int y);
void idlefunc();
void reshape(int w, int h);
void output(GLfloat x, GLfloat y, GLfloat z, float r, float g, float b, void* font, char *string);
void draw_fan_blade();
void colorcube();
void rquad(GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat v4[], GLfloat n1[], GLfloat n2[], GLfloat n3[], GLfloat n4[]);
void div_quad(GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat v4[], GLfloat n1[], GLfloat n2[], GLfloat n3[], GLfloat n4[], int n);
void render_display_lists();
bool load_textures();
void texture_Scene();
void texcube();
void tex_quad(GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat v4[], GLfloat n1[], GLfloat n2[], GLfloat n3[], GLfloat n4[]);
void hybridcube(int top, int bottom, int left, int right, int front, int back);

//display list prototypes
void table_list();
void ceiling_list();
void floor_list();
void wall_list();
void door_list();
void window_list();
void art_list();
void mirror_list();
void roof_list();
void entrance_list();
void stair_list();
void mid_first_floor_list();
void balcony_list();
void chair_list();
void bar_list();
void first_floor_rear_list();

int main(int argc, char *argv[])
{
	// Initialize GLUT
	glutInit(&argc,argv);

	// Initialize the window with double buffering and RGB colors
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// Set the window size to image size
	//glutInitWindowSize(512,512);
	//glutInitWindowSize(1920, 1080);
	glutInitWindowSize(1280, 1020);

	// Create window
	glutCreateWindow("House");

#ifndef OSX
	// Initialize GLEW - MUST BE DONE AFTER CREATING GLUT WINDOW
	glewInit();
#endif

	// Define callbacks
	glutDisplayFunc(display);
	glutKeyboardFunc(keyfunc);
	glutPassiveMotionFunc(passive);
	glutIdleFunc(idlefunc);
	glutReshapeFunc(reshape);

	// Set background color to white
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	//get rid of cursor
	glutSetCursor(GLUT_CURSOR_NONE);

	glShadeModel(GL_SMOOTH);

	//ENABLE THE DEPTH TEST!!!
	glEnable(GL_DEPTH_TEST);

	//enable lighting
	glEnable(GL_LIGHTING);


	//TEXTURE STUFF
	/////////////////////////////////////////////////
	// TODO: Create moon quadric with texture coords
	moon = gluNewQuadric();
	gluQuadricDrawStyle(moon, GLU_FILL);
	gluQuadricTexture(moon, GL_TRUE);

	earth = gluNewQuadric();
	gluQuadricDrawStyle(earth, GLU_FILL);
	gluQuadricTexture(earth, GL_TRUE);

	soda_can = gluNewQuadric();
	gluQuadricDrawStyle(soda_can, GLU_FILL);
	gluQuadricTexture(soda_can, GL_TRUE);

	// Load textures
	if (!load_textures())
	{
		exit(0);
	}

	// Load shader programs
	textureProg = load_shaders(texVertexFile, texFragmentFile);

	// Activate shader program
	glUseProgram(textureProg);

	// TODO: Associate and assign sampler parameter
	texSampler = glGetUniformLocation(textureProg, "texMap");
	glUniform1i(texSampler, 0);
	//////////////////////////////////////////////////

	//LIGHTING STUFF
	///////////////////////////////////////////////////
	// Load shader programs
	shaderProg = load_shaders(vertexFile, fragmentFile);
	glUseProgram(shaderProg);

	// Associate num_lights parameter
	num_lights_param = glGetUniformLocation(shaderProg, "num_lights");
	//////////////////////////////////////////////////

	//Pre-render the display lists
	render_display_lists();

	// Begin event loop
	glutMainLoop();
	return 0;
}

// Display callback
void display()
{
	// Set number of lights
	glUniform1i(num_lights_param, num_lights);

	// Reset background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//perspective projection
	glFrustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 200.0f);

	// Set modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//set the camera position
	gluLookAt(eye[X], eye[Y], eye[Z], at[X], at[Y], at[Z], up[X], up[Y], up[Z]);

	// Render scene
	glUseProgram(shaderProg);
	render_Scene();

	//Texture Scene
	glUseProgram(textureProg);
	texture_Scene();

	// Flush buffer
	glFlush();

	// Swap buffers
	glutSwapBuffers();
}

// Routine to load textures using SOIL
bool load_textures()
{
	for (int i = 0; i < NO_TEXTURES; i++)
	{
		//Load images
		if (i == DR_PEPPER)
		{
			tex_ids[i] = SOIL_load_OGL_texture(texture_files[i], SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
		}
		else
		{
			tex_ids[i] = SOIL_load_OGL_texture(texture_files[i], SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		}
		
		// Set texture properties if successfully loaded
		if (tex_ids[i] != 0)
		{
			//Set scaling filters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

			//Set wrapping modes
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
		// Otherwise texture failed to load
		else
		{
			return false;
		}
	}
	return true;
}

// Keyboard callback
void keyfunc(unsigned char key, int x, int y)
{
	// <esc> quits
	if (key == 27)
	{
		exit(0);
	}

	// n changes normal mode
	if (key == 'n')
	{
		if (normal_mode == SURFACE)
		{
			normal_mode = VERTEX;
			render_display_lists();
		}
		else if (normal_mode == VERTEX)
		{
			normal_mode = SURFACE;
			render_display_lists();
		}
	}

	//left strafe
	if (key == 'a')
	{
		eye[X] += at[Z] / 300;
		eye[Z] -= at[X] / 300;
	}
	//right strafe
	else if (key == 'd')
	{
		eye[X] -= at[Z] / 300;
		eye[Z] += at[X] / 300;
	}


	if (key == 's')
	{
		//the camera should move away from the 
		//direction that the camera is pointing
		eye[X] -= at[X] / 300;
#ifdef FLY
		eye[Y] -= at[Y] / 300;
#endif
		eye[Z] -= at[Z] / 300;
	}
	else if (key == 'w')
	{
		//the camera should move in the direction
		//that the camera is pointing
		eye[X] += at[X] / 300;
#ifdef FLY
		eye[Y] += at[Y] / 300;
#endif
		eye[Z] += at[Z] / 300;
	}

	if(key == ' ')
	{
#ifdef FLY
		eye[Y] += 0.001f * radius;
		at[Y] += 0.001f * radius;
#endif
	}
	else if(key == 'c')
	{
#ifdef FLY
		eye[Y] -= 0.001f * radius;
		at[Y] -= 0.001f * radius;
#endif
	}

#ifndef FLY
	//CLIP MOVEMENT AT WALLS
	if (eye[X] < -9)
	{
		eye[X] = -9;
	}
	else if (eye[X] > 9)
	{
		eye[X] = 9;
	}

	if (eye[Z] < -9)
	{
		eye[Z] = -9;
	}
	else if (eye[Z] > 9)
	{
		eye[Z] = 9;
	}
#endif

	if (key == 'f')
	{
		fan_anim_flag = !fan_anim_flag;
	}
	else if (key == 'l')
	{
		light_toggle_flag = !light_toggle_flag;
	}

	// Redraw screen
	glutPostRedisplay();
}

//This callback is triggered whenever the mouse is
//moved without any buttons being pressed down.
//Mouselook is implemented in this callback by 
//moving the point the camera is looking at along 
//spherical coordinates.
void passive(int x, int y)
{
	//update the change in mouse x-position
	dx = (abs(start_x) - abs(x)) / 10;
	//update the change in mouse y-position
	dy = (abs(start_y) - abs(y)) / 10;

	//use the change in mouse coordinates to set
	//where the camera is looking (i.e. mouselook)
	at[X] = radius * cos((start_x + dx) * DEG2RAD) * cos((start_y + dy) * DEG2RAD);
	at[Y] = radius * sin((start_y + dy) * DEG2RAD);
	at[Z] = radius * sin((start_x + dx) * DEG2RAD) * cos((start_y + dy) * DEG2RAD);

	//set the new mouse position for later
	start_x = x;
	start_y = y;

	//redraw scene
	glutPostRedisplay();
}

// Idle callback
void idlefunc()
{
	//get the current time since init()
	time = glutGet(GLUT_ELAPSED_TIME);
	//calculate rames per second
	fps = 1000 / (time - lasttime);
	//update lasttime for next go round
	lasttime = time;

	//update fan spin angle
	if (fan_anim_flag)
	{
		fan_theta += (360 / fps);
		if (fan_theta > 360)
		{
			fan_theta -= 360;
		}
	}

	// Render scene
	glutPostRedisplay();
}

// Reshape callback
void reshape(int w, int h)
{
	GLfloat ratio;

	// Set new screen extents
	glViewport(0, 0, w, h);

	// Set projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Taller than wide so scale height
	if (w <= h)
	{
		ratio = (GLfloat)h / (GLfloat)w;
		glOrtho(-2.0f, 2.0f, -2.0f*ratio, 2.0f*ratio, -2.0f, 2.0f);
	}
	// Wider than tall so scale width
	else
	{
		ratio = (GLfloat)w / (GLfloat)h;
		glOrtho(-2.0f*ratio, 2.0*ratio, -2.0f, 2.0f, -2.0f, 2.0f);
	}
}


void output(GLfloat x, GLfloat y, GLfloat z, float r, float g, float b, void* font, char *string)
{
	glColor3f(r, g, b);
	//glRasterPos2f(x, y);
	glRasterPos3f(x, y, z);
	int len, i;
	len = (int)strlen(string);
	for (i = 0; i < len; i++) {
		glutBitmapCharacter(font, string[i]);
	}
}


void texture_Scene()
{
	//draw lightbulbs
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, tex_ids[MOON]);
	glTranslatef(light0_pos[0], light0_pos[1], light0_pos[2]);
	gluSphere(moon, 1.0f, 20, 20);
	glPopMatrix();
	glPushMatrix();
	//glBindTexture(GL_TEXTURE_2D, tex_ids[MOON]);
	glTranslatef(light1_pos[0], light1_pos[1], light1_pos[2]);
	gluSphere(moon, 1.0f, 20, 20);
	glPopMatrix();
	glPushMatrix();
	//glBindTexture(GL_TEXTURE_2D, tex_ids[MOON]);
	glTranslatef(light2_pos[0], light2_pos[1], light2_pos[2]);
	gluSphere(moon, 1.0f, 20, 20);
	glPopMatrix();
	glPushMatrix();
	//glBindTexture(GL_TEXTURE_2D, tex_ids[MOON]);
	glTranslatef(light3_pos[0], light3_pos[1], light3_pos[2]);
	gluSphere(moon, 1.0f, 20, 20);
	glPopMatrix();
}

// Scene render function
void render_Scene()
{

	//Lighting
	set_PointLightAttenuated(GL_LIGHT0, &white_light, light0_pos, light0_attenuation);
	set_PointLightAttenuated(GL_LIGHT1, &white_light, light1_pos, light1_attenuation);
	set_PointLightAttenuated(GL_LIGHT2, &white_light, light2_pos, light2_attenuation);
	set_PointLightAttenuated(GL_LIGHT3, &white_light, light3_pos, light3_attenuation);

	//FLAT ROOF WITH ACCENTS
	glPushMatrix();
	set_material(GL_FRONT, &faux_wood);
	glCallList(ROOF);
	glPopMatrix();

	//BASIC FLOOR QUADRIC
	glPushMatrix();
	set_material(GL_FRONT, &white_rubber);
	glCallList(FLOOR);
	glPopMatrix();

	//BASIC WALLS
	glPushMatrix();
	set_material(GL_FRONT, &faux_wood);
	glCallList(WALLS);
	glPopMatrix();

	//BASIC DOOR
	glPushMatrix();
	glCallList(DOOR);
	glPopMatrix();

#ifdef FAN
	//fan blades
	glPushMatrix();
	set_material(GL_FRONT, &faux_wood);
	glTranslatef(0.0f, 14.25f, 0.0f);
	glPushMatrix();
	glRotatef(fan_theta, 0, 1, 0);
	glRotatef(30, 1, 0, 0);
	glTranslatef(0.25f, 0.0f, 0.0f);
	draw_fan_blade();
	glPopMatrix();

	glPushMatrix();
	glRotatef(72 + fan_theta, 0, 1, 0);
	glRotatef(30, 1, 0, 0);
	glTranslatef(0.25f, 0.0f, 0.0f);
	draw_fan_blade();
	glPopMatrix();

	glPushMatrix();
	glRotatef(144 + fan_theta, 0, 1, 0);
	glRotatef(30, 1, 0, 0);
	glTranslatef(0.25f, 0.0f, 0.0f);
	draw_fan_blade();
	glPopMatrix();

	glPushMatrix();
	glRotatef(216 + fan_theta, 0, 1, 0);
	glRotatef(30, 1, 0, 0);
	glTranslatef(0.25f, 0.0f, 0.0f);
	draw_fan_blade();
	glPopMatrix();

	glPushMatrix();
	glRotatef(288 + fan_theta, 0, 1, 0);
	glRotatef(30, 1, 0, 0);
	glTranslatef(0.25f, 0.0f, 0.0f);
	draw_fan_blade();
	glPopMatrix();
	glPopMatrix();



	//fan light
	if (light_toggle_flag)
	{
		set_material(GL_FRONT, &brass);
	}
	else
	{
		set_material(GL_FRONT, &white_rubber);
	}
	glPushMatrix();
	//gl_color3f(0.0f, 0.0f, 0.0f);
	glTranslatef(0.0f, 14.0f, 0.0f);
	//glRotatef(90, 1, 0, 0);
	glScalef(0.75f, 0.5f, 0.75f);
	gluSphere(fan_light, 0.5f, 20, 20);
	//gluCylinder(fan_light, 0.25f, 0.5, 0.25, 20, 20);
	glPopMatrix();
#endif

	//ENTRANCE
	glPushMatrix();
	glCallList(ENTRANCE);
	glPopMatrix();

	//SECOND FLOOR
	glPushMatrix();
	glCallList(BALCONY);
	glPopMatrix();


	//MID FIRST FLOOR
	glPushMatrix();
	glCallList(MID_FIRST_FLOOR);
	glPopMatrix();


	//STAIRS TO SECOND FLOOR
	glPushMatrix();
	glCallList(STAIRS);
	glPopMatrix();

	//FIRST FLOOR -- BAR
	glPushMatrix();
	glCallList(BAR);
	glPopMatrix();

	//FIRST FLOOR -- REAR
	glPushMatrix();
	glCallList(FIRST_FLOOR_REAR);
	glPopMatrix();

}


void render_display_lists()
{
	table_list();
	ceiling_list();
	floor_list();
	wall_list();
	door_list();
	window_list();
	art_list();
	mirror_list();
	roof_list();
	entrance_list();
	stair_list();
	mid_first_floor_list();
	balcony_list();
	chair_list();
	bar_list();
	first_floor_rear_list();
}


//Function to render a cube with a choice of
//texture based or material based faces 
//(independently). Specify your choice for 
//each side by inputting the texture ID of
//your desired texture for that face, or 
//by entering NO_TEXTURES to use materials
//instead
void hybridcube(int top, int bottom, int left, int right, int front, int back)
{
	if (top == NO_TEXTURES)
	{
		glUseProgram(shaderProg);
		// Top face
		rquad(cube[4], cube[7], cube[6], cube[5], vnorm[4], vnorm[7], vnorm[6], vnorm[5]);
	}
	else
	{
		glUseProgram(textureProg);
		glBindTexture(GL_TEXTURE_2D, tex_ids[top]);
		// Top face
		tex_quad(cube[4], cube[7], cube[6], cube[5], vnorm[4], vnorm[7], vnorm[6], vnorm[5]);
	}

	if (bottom == NO_TEXTURES)
	{
		glUseProgram(shaderProg);
		// Bottom face
		rquad(cube[0], cube[1], cube[2], cube[3], vnorm[0], vnorm[1], vnorm[2], vnorm[3]);
	}
	else
	{
		glUseProgram(textureProg);
		glBindTexture(GL_TEXTURE_2D, tex_ids[bottom]);
		// Bottom face
		tex_quad(cube[0], cube[1], cube[2], cube[3], vnorm[0], vnorm[1], vnorm[2], vnorm[3]);
	}

	if (left == NO_TEXTURES)
	{
		glUseProgram(shaderProg);
		// Left face
		rquad(cube[0], cube[3], cube[7], cube[4], vnorm[0], vnorm[3], vnorm[7], vnorm[4]);
	}
	else
	{
		glUseProgram(textureProg);
		glBindTexture(GL_TEXTURE_2D, tex_ids[left]);
		// Left face
		tex_quad(cube[0], cube[3], cube[7], cube[4], vnorm[0], vnorm[3], vnorm[7], vnorm[4]);
	}

	if (right == NO_TEXTURES)
	{
		glUseProgram(shaderProg);
		// Right face
		rquad(cube[1], cube[5], cube[6], cube[2], vnorm[1], vnorm[5], vnorm[6], vnorm[2]);
	}
	else
	{
		glUseProgram(textureProg);
		glBindTexture(GL_TEXTURE_2D, tex_ids[right]);
		// Right face
		tex_quad(cube[1], cube[5], cube[6], cube[2], vnorm[1], vnorm[5], vnorm[6], vnorm[2]);
	}

	if (front == NO_TEXTURES)
	{
		glUseProgram(shaderProg);
		// Front face
		rquad(cube[2], cube[6], cube[7], cube[3], vnorm[2], vnorm[6], vnorm[7], vnorm[3]);
	}
	else
	{
		glUseProgram(textureProg);
		glBindTexture(GL_TEXTURE_2D, tex_ids[front]);
		// Front face
		tex_quad(cube[2], cube[6], cube[7], cube[3], vnorm[2], vnorm[6], vnorm[7], vnorm[3]);
	}

	if (back == NO_TEXTURES)
	{
		glUseProgram(shaderProg);
		// Back face
		rquad(cube[0], cube[4], cube[5], cube[1], vnorm[0], vnorm[4], vnorm[5], vnorm[1]);
	}
	else
	{
		glUseProgram(textureProg);
		glBindTexture(GL_TEXTURE_2D, tex_ids[back]);
		// Back face
		tex_quad(cube[0], cube[4], cube[5], cube[1], vnorm[0], vnorm[4], vnorm[5], vnorm[1]);
	}
}


void texcube()
{
	// Top face
	tex_quad(cube[4], cube[7], cube[6], cube[5], vnorm[4], vnorm[7], vnorm[6], vnorm[5]);

	// Bottom face
	tex_quad(cube[0], cube[1], cube[2], cube[3], vnorm[0], vnorm[1], vnorm[2], vnorm[3]);

	// Left face
	tex_quad(cube[0], cube[3], cube[7], cube[4], vnorm[0], vnorm[3], vnorm[7], vnorm[4]);

	// Right face
	tex_quad(cube[1], cube[5], cube[6], cube[2], vnorm[1], vnorm[5], vnorm[6], vnorm[2]);

	// Front face
	tex_quad(cube[2], cube[6], cube[7], cube[3], vnorm[2], vnorm[6], vnorm[7], vnorm[3]);

	// Back face
	tex_quad(cube[0], cube[4], cube[5], cube[1], vnorm[0], vnorm[4], vnorm[5], vnorm[1]);
}


void tex_quad(GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat v4[], GLfloat n1[], GLfloat n2[], GLfloat n3[], GLfloat n4[])
{
	GLfloat normal[3];

	// Draw face 
	glBegin(GL_POLYGON);
	// Surface normal
	if (normal_mode == SURFACE)
	{
		//Compute surface normal
		cross(v1, v2, v4, normal);
		normalize(normal);

		glNormal3fv(normal);
		glTexCoord2f(0, 0);
		glVertex3fv(v1);
		glTexCoord2f(1, 0);
		glVertex3fv(v2);
		glTexCoord2f(1, 1);
		glVertex3fv(v3);
		glTexCoord2f(0, 1);
		glVertex3fv(v4);
	}
	// Vertex normal
	else
	{
		//Set vertex normals
		glNormal3fv(n1);
		glTexCoord2f(0, 0);
		glVertex3fv(v1);
		glNormal3fv(n2);
		glTexCoord2f(1, 0);
		glVertex3fv(v2);
		glNormal3fv(n3);
		glTexCoord2f(1, 1);
		glVertex3fv(v3);
		glNormal3fv(n4);
		glTexCoord2f(0, 1);
		glVertex3fv(v4);
	}
	glEnd();
}



// Routine to draw cube
void colorcube()
{
	// Top face
	div_quad(cube[4], cube[7], cube[6], cube[5], vnorm[4], vnorm[7], vnorm[6], vnorm[5], div_level);

	// Bottom face
	div_quad(cube[0], cube[1], cube[2], cube[3], vnorm[0], vnorm[1], vnorm[2], vnorm[3], div_level);

	// Left face
	div_quad(cube[0], cube[3], cube[7], cube[4], vnorm[0], vnorm[3], vnorm[7], vnorm[4], div_level);

	// Right face
	div_quad(cube[1], cube[5], cube[6], cube[2], vnorm[1], vnorm[5], vnorm[6], vnorm[2], div_level);

	// Front face
	div_quad(cube[2], cube[6], cube[7], cube[3], vnorm[2], vnorm[6], vnorm[7], vnorm[3], div_level);

	// Back face
	div_quad(cube[0], cube[4], cube[5], cube[1], vnorm[0], vnorm[4], vnorm[5], vnorm[1], div_level);
}

// Routine to perform recursive subdivision
void div_quad(GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat v4[], GLfloat n1[], GLfloat n2[], GLfloat n3[], GLfloat n4[], int n)
{
	GLfloat v1_prime[3], v2_prime[3], v3_prime[3], v4_prime[3], v5_prime[3];
	GLfloat n1_prime[3], n2_prime[3], n3_prime[3], n4_prime[3], n5_prime[3];

	// Recurse until n = 0
	if (n > 0)
	{
		//Compute midpoints
		for (int i = 0; i<3; i++)
		{
			v1_prime[i] = (v4[i] + v1[i]) / 2.0f;
			v2_prime[i] = (v1[i] + v2[i]) / 2.0f;
			v3_prime[i] = (v2[i] + v3[i]) / 2.0f;
			v4_prime[i] = (v3[i] + v4[i]) / 2.0f;
			v5_prime[i] = (v1[i] + v2[i] + v3[i] + v4[i]) / 4.0f;
			n1_prime[i] = (n4[i] + n1[i]) / 2.0f;
			n2_prime[i] = (n1[i] + n2[i]) / 2.0f;
			n3_prime[i] = (n2[i] + n3[i]) / 2.0f;
			n4_prime[i] = (n3[i] + n4[i]) / 2.0f;
			n5_prime[i] = (n1[i] + n2[i] + n3[i] + n4[i]) / 4.0f;
		}

		//Subdivide polygon
		div_quad(v1, v2_prime, v5_prime, v1_prime, n1, n2_prime, n5_prime, n1_prime, n - 1);
		div_quad(v2_prime, v2, v3_prime, v5_prime, n2_prime, n2, n3_prime, n5_prime, n - 1);
		div_quad(v1_prime, v5_prime, v4_prime, v4, n1_prime, n5_prime, n4_prime, n4, n - 1);
		div_quad(v5_prime, v3_prime, v3, v4_prime, n5_prime, n3_prime, n3, n4_prime, n - 1);
	}
	else
	{
		//Otherwise render quad
		rquad(v1, v2, v3, v4, n1, n2, n3, n4);
	}
}

// Routine to draw quadrilateral face
void rquad(GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat v4[], GLfloat n1[], GLfloat n2[], GLfloat n3[], GLfloat n4[])
{
	GLfloat normal[3];

	// Draw face 
	glBegin(GL_POLYGON);
	// Surface normal
	if (normal_mode == SURFACE)
	{
		//Compute surface normal
		cross(v1, v2, v4, normal);
		normalize(normal);

		glNormal3fv(normal);
		glVertex3fv(v1);
		glVertex3fv(v2);
		glVertex3fv(v3);
		glVertex3fv(v4);
	}
	// Vertex normal
	else
	{
		//Set vertex normals
		glNormal3fv(n1);
		glVertex3fv(v1);
		glNormal3fv(n2);
		glVertex3fv(v2);
		glNormal3fv(n3);
		glVertex3fv(v3);
		glNormal3fv(n4);
		glVertex3fv(v4);
	}
	glEnd();
}

void first_floor_rear_list()
{
	glNewList(FIRST_FLOOR_REAR, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);

	set_material(GL_FRONT, &faux_wood);
	//FIRST FLOOR REAR -- under z pos stairs
	glPushMatrix();
	glTranslatef(-45.5f, 10.0f, 27.5f);
	glScalef(9.0f, 20.0f, 15.0f);
	colorcube();
	glPopMatrix();
	//FIRST FLOOR REAR -- under z neg stairs
	glPushMatrix();
	glTranslatef(-45.5f, 10.0f, -27.5f);
	glScalef(9.0f, 20.0f, 15.0f);
	colorcube();
	glPopMatrix();
	//FIRST FLOOR REAR -- Z POS
	glPushMatrix();
	glTranslatef(-50.0f, 10.0f, -42.5f);
	glScalef(50.0f, 20.0f, 15.0f);
	colorcube();
	glPopMatrix();
	//FIRST FLOOR REAR -- Z NEG
	glPushMatrix();
	glTranslatef(-50.0f, 10.0f, 42.5f);
	glScalef(50.0f, 20.0f, 15.0f);
	colorcube();
	glPopMatrix();
	//FIRST FLOOR REAR -- MID
	glPushMatrix();
	glTranslatef(-62.5f, 10.0f, 0.0f);
	glScalef(25.0f, 20.0f, 70.0f);
	colorcube();
	glPopMatrix();

	//HANGING WALL ART STAGE BACKDROP
	glUseProgram(textureProg);
	glBindTexture(GL_TEXTURE_2D, tex_ids[WALL_ART]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-49.0f, 2.5f, -20.0f);
	glTexCoord2f(0, 1);
	glVertex3f(-49.0f, 20.f, -20.0f);
	glTexCoord2f(1, 1);
	glVertex3f(-49.0f, 20.0f, 20.0f);
	glTexCoord2f(1, 0);
	glVertex3f(-49.0f, 2.5f, 20.0f);
	glEnd();
	glUseProgram(shaderProg);

	glPopAttrib();
	glEndList();
}

void bar_list()
{
	glNewList(BAR, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);

	glPushMatrix();
	glTranslatef(25.0f, 6.0f, -42.5f);
	glPushMatrix();
	glScalef(40.0f, 5.0f, 2.5f);
	colorcube();
	glPopMatrix();
	glTranslatef(0.0f, 3.0f, 0.0f);
	glPushMatrix();
	glScalef(41.0f, 1.0f, 4.0f);
	colorcube();
	glPopMatrix();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(7.5f, 4.0f, -40.0f);
	glRotatef(-90, 0, 1, 0);
	glCallList(CHAIR);
	glTranslatef(0.0f, 0.0f, -2.5f);
	glCallList(CHAIR);
	glTranslatef(0.0f, 0.0f, -2.5f);
	glCallList(CHAIR);
	glTranslatef(0.0f, 0.0f, -2.5f);
	glCallList(CHAIR);
	glTranslatef(0.0f, 0.0f, -2.5f);
	glCallList(CHAIR);
	glTranslatef(0.0f, 0.0f, -2.5f);
	glCallList(CHAIR);
	glTranslatef(0.0f, 0.0f, -2.5f);
	glCallList(CHAIR);
	glTranslatef(0.0f, 0.0f, -2.5f);
	glCallList(CHAIR);
	glTranslatef(0.0f, 0.0f, -2.5f);
	glCallList(CHAIR);
	glTranslatef(0.0f, 0.0f, -2.5f);
	glCallList(CHAIR);
	glTranslatef(0.0f, 0.0f, -2.5f);
	glCallList(CHAIR);
	glTranslatef(0.0f, 0.0f, -2.5f);
	glCallList(CHAIR);
	glTranslatef(0.0f, 0.0f, -2.5f);
	glCallList(CHAIR);
	glTranslatef(0.0f, 0.0f, -2.5f);
	glCallList(CHAIR);
	glTranslatef(0.0f, 0.0f, -2.5f);
	glCallList(CHAIR);
	glPopMatrix();

	glPopAttrib();
	glEndList();
}


void balcony_list()
{
	glNewList(BALCONY, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);

	//SECOND FLOOR -- Z NEG
	set_material(GL_FRONT, &faux_wood);
	glPushMatrix();
	glTranslatef(0.0f, SECOND_FLOOR, 42.5f);
	glScalef(150.0f, 0.25f, 15.0f);
	colorcube();
	glPopMatrix();
	//SECOND FLOOR Z NEG SIDING
	glPushMatrix();
	glTranslatef(17.5f, SECOND_FLOOR + 1.0f, -35.0f);
	glScalef(115.0f, 2.0f, 0.25f);
	colorcube();
	glPopMatrix();
	//Z NEG RAILING HORIZONTAL
	glPushMatrix();
	glTranslatef(17.5f, SECOND_FLOOR + 4.0f, -35.0f);
	glScalef(115.0f, 0.25f, 0.25f);
	colorcube();
	glPopMatrix();
	//Z NEG RAILING VERTICAL
	glPushMatrix();
	glTranslatef(-44.875f, SECOND_FLOOR + 3.0f, -35.0f);
	for (int i = 0; i < 22; i++)
	{
		glTranslatef(5.0f, 0.0f, 0.0f);
		glPushMatrix();
		glScalef(0.25f, 2.25f, 0.25f);
		colorcube();
		glPopMatrix();
	}
	glPopMatrix();

	//SECOND FLOOR -- Z POS
	glPushMatrix();
	glTranslatef(0.0f, SECOND_FLOOR, -42.5f);
	glScalef(150.f, 0.25f, 15.0f);
	colorcube();
	glPopMatrix();
	//SECOND FLOOR Z POS SIDING
	glPushMatrix();
	glTranslatef(17.5f, SECOND_FLOOR + 1.0f, 35.0f);
	glScalef(115.0f, 2.0f, 0.25f);
	colorcube();
	glPopMatrix();
	//Z POS RAILING
	glPushMatrix();
	glTranslatef(17.5f, SECOND_FLOOR + 4.0f, 35.0f);
	glScalef(115.0f, 0.25f, 0.25f);
	colorcube();
	glPopMatrix();
	//Z POS RAILING VERTICAL
	glPushMatrix();
	glTranslatef(-44.875f, SECOND_FLOOR + 3.0f, 35.0f);
	for (int i = 0; i < 22; i++)
	{
		glTranslatef(5.0f, 0.0f, 0.0f);
		glPushMatrix();
		glScalef(0.25f, 2.25f, 0.25f);
		colorcube();
		glPopMatrix();
	}
	glPopMatrix();

	//SECOND FLOOR BALCONY
	glPushMatrix();
	glTranslatef(-57.5f, SECOND_FLOOR, 0.0f);
	glScalef(35.0f, 0.25f, 70.0f);
	colorcube();
	glPopMatrix();
	//BALCONY SIDING (with room for stairs)
	glPushMatrix();
	glTranslatef(-40.0f, SECOND_FLOOR + 1.0f, 0.0f);
	glScalef(0.25f, 2.0f, 45.0f);
	colorcube();
	glPopMatrix();
	//BALCONY RAILING HORIZONTAL
	glPushMatrix();
	glTranslatef(-40.0f, SECOND_FLOOR + 4.0f, 0.0f);
	glScalef(0.25f, 0.25f, 45.0f);
	colorcube();
	glPopMatrix();
	//BALCONY RAILING VERTICAL
	glPushMatrix();
	glTranslatef(-40.0f, SECOND_FLOOR + 3.0f, -27.375f);
	for (int i = 0; i < 10; i++)
	{
		glTranslatef(0.0f, 0.0f, 4.975f);
		glPushMatrix();
		glScalef(0.25f, 2.25f, 0.25f);
		colorcube();
		glPopMatrix();
	}
	glPopMatrix();

	//SECOND FLOOR ENTRANCE WALL BUMPOUT
	//set_material(GL_FRONT, &emerald);
	glUseProgram(textureProg);
	glBindTexture(GL_TEXTURE_2D, tex_ids[WALLPAPER_ROT]);
	//set_material(GL_FRONT, &faux_wood);
	glPushMatrix();
	glTranslatef(70.125f, 27.5f, 0.0f);
	glScalef(9.75f, 25.0f, 100.0f);
	texcube();
	glPopMatrix();

	//BALCONY WALL BUMPOUT
	//set_material(GL_FRONT, &emerald);
	//set_material(GL_FRONT, &faux_wood);
	glBindTexture(GL_TEXTURE_2D, tex_ids[WALLPAPER_NARROW]);
	glPushMatrix();
	glTranslatef(-67.5f, SECOND_FLOOR + 10.0f, 0.0f);
	glScalef(15.0f, 20.0f, 100.0f);
	texcube();
	glPopMatrix();

	glUseProgram(shaderProg);

	//ENTRANCE SIDING
	glPushMatrix();
	set_material(GL_FRONT, &faux_wood);
	glTranslatef(65.1f, SECOND_FLOOR + 1.0f, 0.0f);
	glScalef(0.25f, 2.0f, 70.0f);
	colorcube();
	glPopMatrix();


	glPopAttrib();
	glEndList();
}


void mid_first_floor_list()
{
	glNewList(MID_FIRST_FLOOR, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);

	glUseProgram(textureProg);
	//FIRST FLOOR WALKWAY Z NEG
	glBindTexture(GL_TEXTURE_2D, tex_ids[WOOD_MID_SIDES]);
	set_material(GL_FRONT, &faux_wood);
	glPushMatrix();
	glTranslatef(20.0f, 2.0f, -42.5f);
	glScalef(90.0f, 4.0f, 15.0f);
	//colorcube();
	hybridcube(WOOD_MID_SIDES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES);
	glPopMatrix();
	//FIRST FLOOR WALKWAY Z POS
	glPushMatrix();
	glTranslatef(20.0f, 2.0f, 42.5f);
	glScalef(90.0f, 4.0f, 15.0f);
	//colorcube();
	//texcube();
	hybridcube(WOOD_MID_SIDES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES);
	glPopMatrix();
	//FIRST FLOOR MID ENTRANCE FLOOR
	glUseProgram(textureProg);
	glBindTexture(GL_TEXTURE_2D, tex_ids[WOOD_MID_ENTRANCE]);
	glPushMatrix();
	glTranslatef(35.0f, 1.25f, 0.0f);
	glScalef(20.0f, 2.5f, 80.0f);
	//colorcube();
	//texcube();
	hybridcube(WOOD_MID_ENTRANCE, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES);
	glPopMatrix();
	//FIRST FLOOR MID Z NEG FLOOR
	glBindTexture(GL_TEXTURE_2D, tex_ids[WOOD_MID_SIDES]);
	glPushMatrix();
	glTranslatef(5.0f, 1.25f, -30.0f);
	glScalef(40.0f, 2.5f, 20.0f);
	//colorcube();
	//texcube();
	hybridcube(WOOD_MID_SIDES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES);
	glPopMatrix();
	//FIRST FLOOR MID Z POS FLOOR
	glPushMatrix();
	glTranslatef(5.0f, 1.25f, 30.0f);
	glScalef(40.0f, 2.5f, 20.0f);
	//colorcube();
	//texcube();
	hybridcube(WOOD_MID_SIDES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES);
	glPopMatrix();
	//FIRST FLOOR MID STAGE FLOOR
	glBindTexture(GL_TEXTURE_2D, tex_ids[WOOD_MID_ENTRANCE]);
	glPushMatrix();
	glTranslatef(-20.0f, 1.25f, 0.0f);
	glScalef(15.0f, 2.5f, 10.0f);
	//colorcube();
	texcube();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-38.75f, 1.25f, 0.0f);
	glScalef(22.5f, 2.5f, 40.0f);
	//colorcube();
	//texcube();
	hybridcube(WOOD_MID_ENTRANCE, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES);
	glPopMatrix();
	//STAGE STAIRS
	glPushMatrix();	//master push
	glTranslatef(-12.0f, 1.8f, 0.0f);
	glPushMatrix();
	glScalef(1.0f, 0.5f, 10.0f);
	//colorcube();	//stair (stage)
	texcube();
	glPopMatrix();
	glTranslatef(1.0f, -0.75f, 0.0f);
	glPushMatrix();
	glScalef(1.0f, 0.5f, 10.0f);
	//colorcube();	//stair
	texcube();
	glPopMatrix();
	glTranslatef(1.0f, -0.75f, 0.0f);
	glPushMatrix();
	glScalef(1.0f, 0.5f, 10.0f);
	//colorcube();	//stair
	texcube();
	glPopMatrix();
	glPopMatrix();	//master pop
	//stage stair siding Z POS
	glPushMatrix();
	glTranslatef(-11.25f, 1.2f, 4.875f);
	glRotatef(-35, 0, 0, 1);
	glScalef(3.75f, 1.0f, 0.25f);
	//colorcube();
	texcube();
	glPopMatrix();
	//stage stair siding Z NEG
	glPushMatrix();
	glTranslatef(-11.25f, 1.2f, -4.875f);
	glRotatef(-35, 0, 0, 1);
	glScalef(3.75f, 1.0f, 0.25f);
	//colorcube();
	texcube();
	glPopMatrix();

	glUseProgram(shaderProg);

	//FIRST FLOOR Z POS POOL
	glPushMatrix();
	set_material(GL_FRONT, &cyan_rubber);
	glTranslatef(-20.0f, 1.0f, 12.0f);
	glScalef(10.0f, 2.0f, 10.0f);
	colorcube();
	glPopMatrix();
	//FIRST FLOOR Z NEG POOL
	glPushMatrix();
	glTranslatef(-20.0f, 1.0f, -12.0f);
	glScalef(10.0f, 2.0f, 10.0f);
	colorcube();
	glPopMatrix();

	//FIRST FLOOR MID TABLES
	///////////////////
	glPushMatrix();
	glTranslatef(35.0f, 2.5f, 20.0f);
	glCallList(TABLE);
	glPopMatrix();
	///////////////////
	glPushMatrix();
	glTranslatef(22.5f, 2.5f, 30.0f);
	glCallList(TABLE);
	glPopMatrix();
	//////////////////
	glPushMatrix();
	glTranslatef(35.0f, 2.5f, 30.0f);
	glRotatef(90, 0, 1, 0);
	glCallList(TABLE);
	glPopMatrix();
	//////////////////
	glPushMatrix();
	glTranslatef(12.5f, 2.5f, 30.0f);
	glRotatef(90, 0, 1, 0);
	glCallList(TABLE);
	glPopMatrix();
	///////////////////
	glPushMatrix();
	glTranslatef(35.0f, 2.5f, -20.0f);
	glCallList(TABLE);
	glPopMatrix();
	////////////////////
	glPushMatrix();
	glTranslatef(22.5f, 2.5f, -30.0f);
	glCallList(TABLE);
	glPopMatrix();
	///////////////////	
	glPushMatrix();
	glTranslatef(35.0f, 2.5f, -30.0f);
	glRotatef(90, 0, 1, 0);
	glCallList(TABLE);
	glPopMatrix();
	///////////////////	
	glPushMatrix();
	glTranslatef(12.5f, 2.5f, -30.0f);
	glRotatef(90, 0, 1, 0);
	glCallList(TABLE);
	glPopMatrix();
	///////////////////


	//Z NEG PILLARS
	glPushMatrix();	//master push
	/////////////////
	glTranslatef(55.25f, 12.0f, -35.25f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	colorcube();
	glPopMatrix();
	/////////////////
	glTranslatef(-12.5f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	colorcube();
	glPopMatrix();
	////////////////
	glTranslatef(-12.5f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	colorcube();
	glPopMatrix();
	////////////////
	glTranslatef(-12.5f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	colorcube();
	glPopMatrix();
	////////////////
	glTranslatef(-12.5f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	colorcube();
	glPopMatrix();
	////////////////
	glTranslatef(-12.5f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	colorcube();
	glPopMatrix();
	////////////////
	glTranslatef(-12.5f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	colorcube();
	glPopMatrix();
	////////////////
	glPopMatrix();	//master pop
	//Z POS PILLARS
	glPushMatrix();	//master push
	/////////////////
	glTranslatef(55.25f, 12.0f, 35.25f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	colorcube();
	glPopMatrix();
	/////////////////
	glTranslatef(-12.5f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	colorcube();
	glPopMatrix();
	////////////////
	glTranslatef(-12.5f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	colorcube();
	glPopMatrix();
	////////////////
	glTranslatef(-12.5f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	colorcube();
	glPopMatrix();
	////////////////
	glTranslatef(-12.5f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	colorcube();
	glPopMatrix();
	////////////////
	glTranslatef(-12.5f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	colorcube();
	glPopMatrix();
	////////////////
	glTranslatef(-12.5f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	colorcube();
	glPopMatrix();
	////////////////
	glPopMatrix();	//master pop


	glPopAttrib();
	glEndList();
}


void stair_list()
{
	glNewList(STAIRS, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);

	//Z NEG STAIRS
	set_material(GL_FRONT, &faux_wood);
	glPushMatrix();
	glTranslatef(-14.0f, 2.0f, -27.5f);
	for (int i = 0; i < 17; i++)
	{
		//stair 2 - n
		glTranslatef(-1.5f, 1.0f, 0.0f);
		glPushMatrix();
		glScalef(2.0f, 0.5f, 10.0f);
		colorcube();
		glPopMatrix();
	}
	glPopMatrix();

	//Z POS STAIRS
	glPushMatrix();
	glTranslatef(-14.0f, 2.0f, 27.5f);
	for (int i = 0; i < 17; i++)
	{
		//stair 2 - n
		glTranslatef(-1.5f, 1.0f, 0.0f);
		glPushMatrix();
		glScalef(2.0f, 0.5f, 10.0f);
		colorcube();
		glPopMatrix();
	}
	glPopMatrix();

	//STAIR RAILS
	glPushMatrix();	//master push
	glTranslatef(-27.5f, 11.0f, -22.5f);
	glPushMatrix();
	glRotatef(-35, 0, 0, 1);
	glScalef(30.0f, 2.0f, 0.25f);
	colorcube();
	glPopMatrix();
	///////////////////
	glTranslatef(0.0f, 0.0f, -10.0f);
	glPushMatrix();
	glRotatef(-35, 0, 0, 1);
	glScalef(30.0f, 2.0f, 0.25f);
	colorcube();
	glPopMatrix();
	///////////////////
	glTranslatef(0.0f, 0.0f, 55.0f);
	glPushMatrix();
	glRotatef(-35, 0, 0, 1);
	glScalef(30.0f, 2.0f, 0.25f);
	colorcube();
	glPopMatrix();
	//////////////////
	glTranslatef(0.0f, 0.0f, 10.0f);
	glPushMatrix();
	glRotatef(-35, 0, 0, 1);
	glScalef(30.0f, 2.0f, 0.25f);
	colorcube();
	glPopMatrix();
	glPopMatrix();	//master pop

	//UNDER STAIR FLOOR -- Z NEG
	glPushMatrix();
	glTranslatef(-28.0f, 1.25f, -27.5f);
	glScalef(26.0f, 2.5f, 15.0f);
	colorcube();
	glPopMatrix();
	//UNDER STAIR FLOOR -- Z POS
	glPushMatrix();
	glTranslatef(-28.0f, 1.25f, 27.5f);
	glScalef(26.0f, 2.5f, 15.0f);
	colorcube();
	glPopMatrix();

	glPopAttrib();
	glEndList();
}



void entrance_list()
{
	glNewList(ENTRANCE, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);

	//Entrance Wood Base
	glPushMatrix();	//master push
	glUseProgram(textureProg);
	glBindTexture(GL_TEXTURE_2D, tex_ids[WOOD_MID_ENTRANCE]);
	//set_material(GL_FRONT, &faux_wood);
	glTranslatef(70.0f, 2.0f, 0.0f);
	glPushMatrix();
	glScalef(10.0f, 4.0f, 100.0f);
	texcube();
	glPopMatrix();
	//walkway
	glBindTexture(GL_TEXTURE_2D, tex_ids[WOOD_MID_SIDES]);
	glTranslatef(-15.0f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(20.0f, 4.0f, 10.0f);
	texcube();
	glPopMatrix();
	//railing
	glUseProgram(shaderProg);
	set_material(GL_FRONT, &faux_wood);
	glTranslatef(0, 4.0f, -5.0f);
	glPushMatrix();
	glScalef(20.0f, 1.0f, 0.25f);
	colorcube();
	glPopMatrix();
	//railing
	glTranslatef(0.0f, 2.0f, 0.0f);
	glPushMatrix();
	glScalef(20.0f, 1.0f, 0.25f);
	colorcube();
	glPopMatrix();
	//railing
	glTranslatef(0.0f, -2.0f, 10.0f);
	glPushMatrix();
	glScalef(20.0f, 1.0f, 0.25f);
	colorcube();
	glPopMatrix();
	//railing
	glTranslatef(0.0f, 2.0f, 0.0f);
	glPushMatrix();
	glScalef(20.0f, 1.0f, 0.25f);
	colorcube();
	glPopMatrix();
	// back bannister 1
	glTranslatef(10.0f, 1.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 10.0f, 0.5f);
	colorcube();
	glPopMatrix();
	//back bannister 2
	glTranslatef(0.0f, 0.0f, -10.0f);
	glPushMatrix();
	glScalef(0.5f, 10.0f, 0.5f);
	colorcube();
	glPopMatrix();
	//entrance small stairs
	glTranslatef(-20.5f, -5.75f, 5.0f);
	glPushMatrix();
	glScalef(1.0f, 1.5f, 10.0f);
	colorcube();
	glPopMatrix();
	glTranslatef(-1.0f, -0.25f, 0.0f);
	glPushMatrix();
	glScalef(1.0f, 1.0f, 10.0f);
	colorcube();
	glPopMatrix();
	glTranslatef(-1.0f, -0.25f, 0.0f);
	glPushMatrix();
	glScalef(1.0f, 0.5f, 10.0f);
	colorcube();
	glPopMatrix();
	//mid bannister 1
	glTranslatef(2.75f, 6.0f, 5.0f);
	glPushMatrix();
	glScalef(0.5f, 10.0f, 0.5f);
	colorcube();
	glPopMatrix();
	//mid bannister 2
	glTranslatef(0.0f, 0.0f, -10.0f);
	glPushMatrix();
	glScalef(0.5f, 10.0f, 0.5f);
	colorcube();
	glPopMatrix();
	//front bannister 1
	glTranslatef(-3.0f, -1.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 12.0f, 0.5f);
	colorcube();
	glPopMatrix();
	//front bannister 2
	glTranslatef(0.0f, 0.0f, 10.0f);
	glPushMatrix();
	glScalef(0.5f, 12.0f, 0.5f);
	colorcube();
	glPopMatrix();
	//ARCHWAY -- support beam 1
	glTranslatef(11.0f, 6.0f, 0.0f);
	glPushMatrix();
	glScalef(23.0f, 0.5f, 0.5f);
	colorcube();
	glPopMatrix();
	//support beam 2
	glTranslatef(0.0f, 0.0f, -10.0f);
	glPushMatrix();
	glScalef(23.0f, 0.5f, 0.5f);
	colorcube();
	glPopMatrix();
	//ARCHWAY ROOF
	glUseProgram(textureProg);
	glTranslatef(-13.0f, 0.0f, 5.0f);
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, tex_ids[TILE]);
	glBegin(GL_QUADS);
	glTexCoord3f(0.0f, 0.0f, -6.0f);
	glVertex3f(0.0f, 0.0f, -6.0f);
	glTexCoord3f(25.0f, 0.0f, -6.0f);
	glVertex3f(25.0f, 0.0f, -6.0f);
	glTexCoord3f(25.0f, 4.0f, 0.0f);
	glVertex3f(25.0f, 4.0f, 0.0f);
	glTexCoord3f(0.0f, 4.0f, 0.0f);
	glVertex3f(0.0f, 4.0f, 0.0f);
	glEnd();
	glBegin(GL_QUADS);
	glTexCoord3f(0.0f, 4.0f, 0.0f);
	glVertex3f(0.0f, 4.0f, 0.0f);
	glTexCoord3f(25.0f, 4.0f, 0.0f);
	glVertex3f(25.0f, 4.0f, 0.0f);
	glTexCoord3f(25.0f, 0.0f, 6.0f);
	glVertex3f(25.0f, 0.0f, 6.0f);
	glTexCoord3f(0.0f, 0.0f, 6.0f);
	glVertex3f(0.0f, 0.0f, 6.0f);
	glEnd();
	glPopMatrix();
	glUseProgram(shaderProg);

	glPopMatrix();	//master pop

	//PILLARS -- Z NEG
	glPushMatrix();	//master push
	glTranslatef(65.25f, 22.0f, -15.25f);
	glPushMatrix();
		glScalef(0.5f, 36.0f, 0.5f);
		colorcube();
	glPopMatrix();
	/////////////////
	glTranslatef(0.0f, 0.0f, -10.0f);
	glPushMatrix();
		glScalef(0.5f, 36.0f, 0.5f);
		colorcube();
	glPopMatrix();
	/////////////////
	glTranslatef(0.0f, 0.0f, -10.0f);
	glPushMatrix();
	glScalef(0.5f, 36.0f, 0.5f);
	colorcube();
	glPopMatrix();
	//PILLARS -- Z POS
	glTranslatef(0.0f, 0.0f, 50.5f);
	glPushMatrix();
	glScalef(0.5f, 36.0f, 0.5f);
	colorcube();
	glPopMatrix();
	////////////////
	glTranslatef(0.0f, 0.0f, 10.0f);
	glPushMatrix();
	glScalef(0.5f, 36.0f, 0.5f);
	colorcube();
	glPopMatrix();
	///////////////
	glTranslatef(0.0f, 0.0f, 10.0f);
	glPushMatrix();
	glScalef(0.5f, 36.0f, 0.5f);
	colorcube();
	glPopMatrix();
	///////////////
	glPopMatrix();	//master pop


	//ENTRANCE TABLES
	glPushMatrix();
	glTranslatef(70.0f, 4.0f, -10.0f);
	glRotatef(90, 0, 1, 0);
	glCallList(TABLE);
	glPopMatrix();
	////////////////////
	glPushMatrix();
	glTranslatef(70.0f, 4.0f, -20.0f);
	glRotatef(90, 0, 1, 0);
	glCallList(TABLE);
	glPopMatrix();
	////////////////////
	glPushMatrix();
	glTranslatef(70.0f, 4.0f, -30.0f);
	glRotatef(90, 0, 1, 0);
	glCallList(TABLE);
	glPopMatrix();
	////////////////////
	glPushMatrix();
	glTranslatef(70.0f, 4.0f, 10.0f);
	glRotatef(90, 0, 1, 0);
	glCallList(TABLE);
	glPopMatrix();
	////////////////////
	glPushMatrix();
	glTranslatef(70.0f, 4.0f, 20.0f);
	glRotatef(90, 0, 1, 0);
	glCallList(TABLE);
	glPopMatrix();
	///////////////////
	glPushMatrix();
	glTranslatef(70.0f, 4.0f, 30.0f);
	glRotatef(90, 0, 1, 0);
	glCallList(TABLE);
	glPopMatrix();

	//PATIO -- FIRST ROW OF TABLES
	glPushMatrix();	//master push
	///////////////////
	glTranslatef(55.0f, 0.0f, 30.0f);
	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	glCallList(TABLE);
	glPopMatrix();
	////////////////////
	glTranslatef(5.0f, 0.0f, -10.0f);
	glPushMatrix();
	glRotatef(45, 0, 1, 0);
	glCallList(TABLE);
	glPopMatrix();
	/////////////////////
	glTranslatef(-10.0f, 0.0f, -5.0f);
	glPushMatrix();
	glCallList(TABLE);
	glPopMatrix();
	////////////////////
	glTranslatef(0.0f, 0.0f, -30.0f);
	glPushMatrix();
	glRotatef(0, 0, 1, 0);
	glCallList(TABLE);
	glPopMatrix();
	////////////////////
	glTranslatef(10.0f, 0.0f, -5.0f);
	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	glCallList(TABLE);
	glPopMatrix();
	//////////////////
	glTranslatef(-5.0f, 0.0f, -10.0f);
	glPushMatrix();
	glCallList(TABLE);
	glPopMatrix();
	/////////////////
	glPopMatrix();	//master pop


	glPopAttrib();
	glEndList();
}

//Display List to draw the table and chairs
void table_list()
{
	glNewList(TABLE, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);

	glUseProgram(shaderProg);
	//TABLE
	glPushMatrix();
	set_material(GL_FRONT, &faux_wood);
	//top
	glPushMatrix();
	glTranslatef(0.0f, 2.25f, 0.0f);
	glScalef(2.25f, 0.5f, 2.25f);
	colorcube();
	glPopMatrix();
	//leg 1
	glPushMatrix();
	glTranslatef(1.0f, 1.0f, 1.0f);
	glScalef(0.25f, 2.0f, 0.25f);
	colorcube();
	glPopMatrix();
	//leg 2
	glPushMatrix();
	glTranslatef(-1.0f, 1.0f, 1.0f);
	glScalef(0.25f, 2.0f, 0.25f);
	colorcube();
	glPopMatrix();
	//leg 3
	glPushMatrix();
	glTranslatef(1.0f, 1.0f, -1.0f);
	glScalef(0.25f, 2.0f, 0.25f);
	colorcube();
	glPopMatrix();
	//leg4
	glPushMatrix();
	glTranslatef(-1.0f, 1.0f, -1.0f);
	glScalef(0.25f, 2.0f, 0.25f);
	colorcube();
	glPopMatrix();

	glPopMatrix();

	//SODA CAN
	glUseProgram(textureProg);	//go to the texture program
	glPushMatrix();
	glTranslatef(0.0f, 3.25f, 0.5f);
	glRotatef(90, 1, 0, 0);
	glBindTexture(GL_TEXTURE_2D, tex_ids[DR_PEPPER]);
	gluCylinder(soda_can, 0.15f, 0.15f, 0.75f, 20, 20);
	glBindTexture(GL_TEXTURE_2D, tex_ids[LID]);
	gluDisk(soda_can, 0.0f, 0.15f, 20, 20);
	glPopMatrix();
	glUseProgram(shaderProg);	//switch back to the shader program

	//FRUIT BOWL
	set_material(GL_FRONT, &white_rubber);
	glPushMatrix();
	glTranslatef(-0.5f, 2.5f, -0.5f);
	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	gluCylinder(bowl, 0.25f, 0.5f, 0.5f, 20, 20);
	glPopMatrix();
	////////////////
	glTranslatef(0.0f, 0.01f, 0.0f);
	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	gluDisk(bowl, 0.0f, 0.25f, 20, 20);
	glPopMatrix();
	////////////////
	glPopMatrix();

	//SPHERICAL FRUIT
	set_material(GL_FRONT, &yellow_rubber);
	glPushMatrix();
	GLUquadricObj *fruit = gluNewQuadric();
	//gl_color3f(1.0f, 1.0f, 0.0f);
	glTranslatef(-0.5f, 2.75f, -0.4f);
	gluSphere(bowl, 0.1f, 10, 10);
	glTranslatef(0.1f, 0.2f, 0.0f);
	gluSphere(bowl, 0.12, 10, 10);
	glTranslatef(-0.25f, 0.0f, 0.1f);
	gluSphere(bowl, 0.11, 10, 10);
	glTranslatef(0.1f, 0.0f, -0.25f);
	gluSphere(bowl, 0.13, 10, 10);
	glPopMatrix();

	//NEG X CHAIR
	glPushMatrix();
	glTranslatef(-3.0f, 0.0f, 0.0f);
	glRotatef(180, 0, 1, 0);
	glCallList(CHAIR);
	glPopMatrix();
	//POS X CHAIR
	glPushMatrix();
	glTranslatef(3.0f, 0.0f, 0.0f);
	glRotatef(30, 0, 1, 0);
	glCallList(CHAIR);
	glPopMatrix();
	//NEG Z CHAIR
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -3.0f);
	glRotatef(90, 0, 1, 0);
	glCallList(CHAIR);
	glPopMatrix();
	//POS Z CHAIR
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 3.0f);
	glRotatef(-135, 0, 1, 0);
	glCallList(CHAIR);
	glPopMatrix();

	glPopAttrib();
	glEndList();
}

void chair_list()
{
	glNewList(CHAIR, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);

	//CHAIR 1
	set_material(GL_FRONT, &faux_wood);
	//seat
	glPushMatrix();
	//gl_color3f(0.75f, 0.25f, 0.25f);
	glTranslatef(0.0f, 1.1f, 0.0f);
	glScalef(1.0f, 0.2f, 1.0f);
	colorcube();
	glPopMatrix();
	//leg 1
	glPushMatrix();
	//gl_color3f(0.75f, 0.25f, 0.25f);
	glTranslatef(0.4f, 0.5f, 0.4f);
	glScalef(0.2f, 1.0f, 0.2f);
	colorcube();
	glPopMatrix();
	//leg 2
	glPushMatrix();
	//gl_color3f(0.75f, 0.25f, 0.25f);
	glTranslatef(0.4f, 0.5f, -0.4f);
	glScalef(0.2f, 1.0f, 0.2f);
	colorcube();
	glPopMatrix();
	//leg 3
	glPushMatrix();
	//gl_color3f(0.75f, 0.25f, 0.25f);
	glTranslatef(-0.4f, 0.5f, 0.4f);
	glScalef(0.2f, 1.0f, 0.2f);
	colorcube();
	glPopMatrix();
	//leg 4
	glPushMatrix();
	//gl_color3f(0.75f, 0.25f, 0.25f);
	glTranslatef(-0.4f, 0.5f, -0.4f);
	glScalef(0.2f, 1.0f, 0.2f);
	colorcube();
	glPopMatrix();
	//back
	glPushMatrix();
	//gl_color3f(0.75f, 0.25f, 0.25f);
	glTranslatef(0.4f, 2.2f, 0.0f);
	glScalef(0.2f, 2.0f, 1.0f);
	colorcube();
	glPopMatrix();


	glPopAttrib();
	glEndList();
}


void roof_list()
{
	glNewList(ROOF, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);

	//ROOF
	glPushMatrix();

	glBegin(GL_QUADS);
	glVertex3f(HOUSE_X, HOUSE_Y, HOUSE_Z);
	glVertex3f(HOUSE_X, HOUSE_Y, -HOUSE_Z);
	glVertex3f(-HOUSE_X, HOUSE_Y, -HOUSE_Z);
	glVertex3f(-HOUSE_X, HOUSE_Y, HOUSE_Z);
	glEnd();
	glPopMatrix();

	//ROOF TERRACE (LONGWAYS)
	glPushMatrix();
	glTranslatef(0.0f, HOUSE_Y - 0.25f, 50.0f);
	for (int i = 0; i < 21; i++)
	{
		glPushMatrix();
			glScalef(150.0f, 0.5f, 0.5f);
			colorcube();
		glPopMatrix();
		glTranslatef(0.0f, 0.0f, -5.0f);
	}
	glPopMatrix();
	//ROOF TERRACE (SHORTWAYS)
	glPushMatrix();
	glTranslatef(75.0f, HOUSE_Y - 0.25f, 0.0f);
	for (int i = 0; i < 31; i++)
	{
		glPushMatrix();
			glScalef(0.5f, 0.5f, 100.0f);
			colorcube();
		glPopMatrix();
		glTranslatef(-5.0f, 0.0f, 0.0f);
	}
	glPopMatrix();


	glPopAttrib();
	glEndList();
}

void ceiling_list()
{
	glNewList(CEILING, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);

	//CEILING
	//left roof (from outside facing front door)
	glPushMatrix();
	//gl_color3f(0.75f, 0.1f, 0.3f);
		glBegin(GL_QUADS);
		glVertex3f(HOUSE_X, MID_CEILING, -HOUSE_Z / 2);
		glVertex3f(HOUSE_X, HOUSE_Y, -HOUSE_Z);
		glVertex3f(-HOUSE_X, HOUSE_Y, -HOUSE_Z);
		glVertex3f(-HOUSE_X, MID_CEILING, -HOUSE_Z / 2);
		glEnd();
	glPopMatrix();

	//right roof (from outside facing front door)
	glPushMatrix();
	//gl_color3f(0.75f, 0.1f, 0.3f);
		glBegin(GL_QUADS);
		glVertex3f(HOUSE_X, MID_CEILING, HOUSE_Z / 2);
		glVertex3f(HOUSE_X, HOUSE_Y, HOUSE_Z);
		glVertex3f(-HOUSE_X, HOUSE_Y, HOUSE_Z);
		glVertex3f(-HOUSE_X, MID_CEILING, HOUSE_Z / 2);
		glEnd();
	glPopMatrix();

	//center roof
	glPushMatrix();
	//gl_color3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_QUADS);
		glVertex3f(HOUSE_X, MID_CEILING, HOUSE_Z / 2);
		glVertex3f(-HOUSE_X, MID_CEILING, HOUSE_Z / 2);
		glVertex3f(-HOUSE_X, MID_CEILING, -HOUSE_Z / 2);
		glVertex3f(HOUSE_X, MID_CEILING, -HOUSE_Z / 2);
		glEnd();
	glPopMatrix();

	//front trapezoid
	glPushMatrix();
	//gl_color3f(0.0f, 0.0f, 0.0f);
		glBegin(GL_QUADS);
		glVertex3f(HOUSE_X, HOUSE_Y, -HOUSE_Z);
		glVertex3f(HOUSE_X, MID_CEILING, -HOUSE_Z / 2);
		glVertex3f(HOUSE_X, MID_CEILING, HOUSE_Z / 2);
		glVertex3f(HOUSE_X, HOUSE_Y, HOUSE_Z);
		glEnd();
	glPopMatrix();

	//back trapezoid
	glPushMatrix();
	//gl_color3f(0.5f, 0.5f, 0.5f);
		glBegin(GL_QUADS);
		glVertex3f(-HOUSE_X, HOUSE_Y, HOUSE_Z);
		glVertex3f(-HOUSE_X, MID_CEILING, HOUSE_Z / 2);
		glVertex3f(-HOUSE_X, MID_CEILING, -HOUSE_Z / 2);
		glVertex3f(-HOUSE_X, HOUSE_Y, -HOUSE_Z);
		glEnd();
	glPopMatrix();

	//fan base
	glPushMatrix();
		GLUquadricObj *fan_base = gluNewQuadric();
		glTranslatef(0.0f, 15.0f, 0.0f);
		glRotatef(90, 1, 0, 0);
		gluCylinder(fan_base, 0.75f, 0.5f, 0.5f, 20, 20);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0.0f, 14.5f, 0.0f);
		glRotatef(90, 1, 0, 0);
		gluDisk(fan_base, 0.25f, 0.5f, 20, 20);
	glPopMatrix();

	//fan support
	glPushMatrix();
		GLUquadricObj *fan_support = gluNewQuadric();
		//gl_color3f(1.0f, 1.0f, 1.0f);
		glTranslatef(0.0f, 14.5f, 0.0f);
		glRotatef(90, 1, 0, 0);
		gluCylinder(fan_support, 0.25f, 0.25f, 0.5f, 20, 20);
	glPopMatrix();




	glPopAttrib();
	glEndList();
}



void draw_fan_blade()
{
	glBegin(GL_POLYGON);
	glVertex3f(0.0f, 0.0f, 0.1f);
	glVertex3f(2.5f, 0.0f, 0.5f);
	glVertex3f(2.75f, 0.0f, 0.5f);
	glVertex3f(3.0f, 0.0f, 0.25f);
	glVertex3f(3.0f, 0.0f, -0.25f);
	glVertex3f(2.75f, 0.0f, -0.5f);
	glVertex3f(2.5f, 0.0f, -0.5f);
	glVertex3f(0.0f, 0.0f, -0.1f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex3f(0.0f, 0.1f, -0.1f);
	glVertex3f(0.0f, 0.1f, 0.1f);
	glVertex3f(2.5f, 0.1f, 0.5f);
	glVertex3f(2.75f, 0.1f, 0.5f);
	glVertex3f(3.0f, 0.1f, 0.25f);
	glVertex3f(3.0f, 0.1f, -0.25f);
	glVertex3f(2.75f, 0.1f, -0.5f);
	glVertex3f(2.5f, 0.1f, -0.5f);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(0.0f, 0.0f, 0.1f);
	glVertex3f(0.0f, 0.1f, 0.1f);
	glVertex3f(0.0f, 0.1f, -0.1f);
	glVertex3f(0.0f, 0.0f, -0.1f);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(0.0f, 0.0f, -0.1f);
	glVertex3f(0.0f, 0.1f, -0.1f);
	glVertex3f(2.5f, 0.1f, -0.5f);
	glVertex3f(2.5f, 0.0f, -0.5f);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(2.5f, 0.0f, -0.5f);
	glVertex3f(2.5f, 0.1f, -0.5f);
	glVertex3f(2.75f, 0.1f, -0.5f);
	glVertex3f(2.75f, 0.0f, -0.5f);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(2.75f, 0.0f, -0.5f);
	glVertex3f(2.75f, 0.1f, -0.5f);
	glVertex3f(3.0f, 0.1f, -0.25f);
	glVertex3f(3.0f, 0.0f, -0.25f);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(3.0f, 0.0f, -0.25f);
	glVertex3f(3.0f, 0.1f, -0.25f);
	glVertex3f(3.0f, 0.1f, 0.25f);
	glVertex3f(3.0f, 0.0f, 0.25f);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(3.0f, 0.0f, 0.25f);
	glVertex3f(3.0f, 0.1f, 0.25f);
	glVertex3f(2.75f, 0.1f, 0.5f);
	glVertex3f(2.75f, 0.0f, 0.5f);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(2.75f, 0.0f, 0.5f);
	glVertex3f(2.75, 0.1f, 0.5f);
	glVertex3f(2.5f, 0.1f, 0.5f);
	glVertex3f(2.5f, 0.0f, 0.5f);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(2.5f, 0.0f, 0.5f);
	glVertex3f(2.5f, 0.1f, 0.5f);
	glVertex3f(0.0f, 0.1f, 0.1f);
	glVertex3f(0.0f, 0.0f, 0.1f);
	glEnd();
}



void floor_list()
{

	glNewList(FLOOR, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);

	//MID FLOOR
	glUseProgram(textureProg);
	glBindTexture(GL_TEXTURE_2D, tex_ids[FLOOR_TILE]);
	glPushMatrix();
	//gl_color3f(0.0f, 0.25f, 0.5f);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(25.0f, 0.0f, 20.0f);
		glTexCoord2f(0, 10);
		glVertex3f(25.0f, 0.0f, -20.0f);
		glTexCoord2f(5, 10);
		glVertex3f(-27.5f, 0.0f, -20.0f);
		glTexCoord2f(5, 0);
		glVertex3f(-27.5f, 0.0f, 20.0f);
		glEnd();
	glPopMatrix();

	//Z NEG PATIO FLOOR
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(45.0f, 0.0f, -35.0f);
	glTexCoord2f(0, 3);
	glVertex3f(65.0f, 0.0f, -35.0f);
	glTexCoord2f(5, 3);
	glVertex3f(65.0f, 0.0f, -5.0f);
	glTexCoord2f(5, 0);
	glVertex3f(45.0f, 0.0f, -5.0f);
	glEnd();
	glPopMatrix();

	//Z POS PATIO FLOOR
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(45.0f, 0.0f, 35.0f);
	glTexCoord2f(0, 3);
	glVertex3f(65.0f, 0.0f, 35.0f);
	glTexCoord2f(5, 3);
	glVertex3f(65.0f, 0.0f, 5.0f);
	glTexCoord2f(5, 0);
	glVertex3f(45.0f, 0.0f, 5.0f);
	glEnd();
	glPopMatrix();

	glUseProgram(shaderProg);

	//glPushMatrix();
	//gl_color3f(0.0f, 1.0f, 0.0f);
		//glBegin(GL_QUADS);
		//glVertex3f(5.0f, 0.01f, 5.0f);
		//glVertex3f(-5.0f, 0.01f, 5.0f);
		//glVertex3f(-5.0f, 0.01f, -5.0f);
		//glVertex3f(5.0f, 0.01f, -5.0f);
		//glEnd();
	//glPopMatrix();

	glPopAttrib();
	glEndList();
}


void wall_list()
{
	glNewList(WALLS, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);

	glUseProgram(textureProg);
	glBindTexture(GL_TEXTURE_2D, tex_ids[WALLPAPER]);
	//WALLS
	glPushMatrix();
	//gl_color3f(0.5f, 0.25f, 0.25f);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(HOUSE_X, WALL_Y_LOWER, HOUSE_Z);
		glTexCoord2f(1, 0);
		glVertex3f(HOUSE_X, WALL_Y_UPPER, HOUSE_Z);
		glTexCoord2f(1, 1);
		glVertex3f(HOUSE_X, WALL_Y_UPPER, -HOUSE_Z);
		glTexCoord2f(0, 1);
		glVertex3f(HOUSE_X, WALL_Y_LOWER, -HOUSE_Z);
		glEnd();
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, tex_ids[WALLPAPER_NARROW]);
	glPushMatrix();
	//gl_color3f(0.5f, 0.25f, 0.25f);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(-HOUSE_X, WALL_Y_LOWER, HOUSE_Z);
		glTexCoord2f(1, 0);
		glVertex3f(-HOUSE_X, WALL_Y_UPPER, HOUSE_Z);
		glTexCoord2f(1, 1);
		glVertex3f(HOUSE_X, WALL_Y_UPPER, HOUSE_Z);
		glTexCoord2f(0, 1);
		glVertex3f(HOUSE_X, WALL_Y_LOWER, HOUSE_Z);
		glEnd();
	glPopMatrix();

	glPushMatrix();
	//gl_color3f(0.5f, 0.25f, 0.25f);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(-HOUSE_X, WALL_Y_LOWER, -HOUSE_Z);
		glTexCoord2f(1, 0);
		glVertex3f(-HOUSE_X, WALL_Y_UPPER, -HOUSE_Z);
		glTexCoord2f(1, 1);
		glVertex3f(HOUSE_X, WALL_Y_UPPER, -HOUSE_Z);
		glTexCoord2f(0, 1);
		glVertex3f(HOUSE_X, WALL_Y_LOWER, -HOUSE_Z);
		glEnd();
	glPopMatrix();

	glUseProgram(shaderProg);

	glPushMatrix();
	//gl_color3f(0.5f, 0.25f, 0.25f);
	glBegin(GL_QUADS);
	glVertex3f(-HOUSE_X, WALL_Y_LOWER, HOUSE_Z);
	glVertex3f(-HOUSE_X, WALL_Y_UPPER, HOUSE_Z);
	glVertex3f(-HOUSE_X, WALL_Y_UPPER, -HOUSE_Z);
	glVertex3f(-HOUSE_X, WALL_Y_LOWER, -HOUSE_Z);
	glEnd();
	glPopMatrix();

	glPopAttrib();
	glEndList();
}

void door_list()
{
	glNewList(DOOR, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);

	//DOOR 1
	set_material(GL_FRONT, &faux_wood);
	glPushMatrix();
	//gl_color3f(0.0f, 1.0f, 0.0f);
		glTranslatef(75.0f, 8.0f, -1.505f);
		glScalef(0.3f, 8.0f, 3.0f);
		colorcube();
	glPopMatrix();

	//handle
	set_material(GL_FRONT, &chrome);
	glPushMatrix();
	glTranslatef(74.75f, 8.0f, -0.2f);
	glRotatef(90, 0, 1, 0);
	GLUquadricObj *door_handle = gluNewQuadric();
	gluCylinder(door_handle, 0.075f, 0.075f, 0.15f, 10, 10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(74.7f, 8.0f, -0.2f);
	glScalef(0.5f, 1.0f, 1.0f);
	glutSolidSphere(0.15f, 20, 20);
	glPopMatrix();


	//DOOR 2
	set_material(GL_FRONT, &faux_wood);
	glPushMatrix();
	//gl_color3f(0.0f, 1.0f, 0.0f);
	glTranslatef(75.0f, 8.0f, 1.505f);
	glScalef(0.3f, 8.0f, 3.0f);
	colorcube();
	glPopMatrix();

	//handle
	set_material(GL_FRONT, &chrome);
	glPushMatrix();
	glTranslatef(74.75f, 8.0f, 0.2f);
	glRotatef(90, 0, 1, 0);
	gluCylinder(door_handle, 0.075f, 0.075f, 0.15f, 10, 10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(74.7f, 8.0f, 0.2f);
	glScalef(0.5f, 1.0f, 1.0f);
	glutSolidSphere(0.15f, 20, 20);
	glPopMatrix();


	glPopAttrib();
	glEndList();
}

void window_list()
{
	glNewList(WINDOW, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);

	glPushMatrix();
		glTranslatef(0.0f, 6.0f, 10.0f);
		glScalef(10.0f, 5.0f, 0.3f);
		colorcube();
	glPopMatrix();

	glPopAttrib();
	glEndList();
}

void art_list()
{
	glNewList(ART, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);

	glPushMatrix();
		glTranslatef(-10.0f, 5.0f, 0.0f);
		glScalef(0.3f, 7.5f, 5.0f);
		colorcube();
	glPopMatrix();

	glPopAttrib();
	glEndList();
}

void mirror_list()
{
	glNewList(MIRROR, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);

	glPushMatrix();
	glTranslatef(0.0f, 5.0f, -10.0f);
	glScalef(2.0f, 2.0f, 0.3f);
	colorcube();
	glPopMatrix();

	glPopAttrib();
	glEndList();
}