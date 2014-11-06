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
f -> toggle fan(s) on/off

Enjoy!

Unfinished Business:
-Everything is yellow
-Shoji blinds for bar windows
-Mirror on z-pos side
-Second story lights
-Entrance Railing Lattice
-Stage Railings
-Texture ceiling with genwood texture
-Bar shelf backdrop
-Add rocks and sand lines into mid sand
-Decorate/embellish the walls and floors with smaller objects

Known Issues:
-Y position mouselook is unbounded
-Movement speed is altered based on where the camera is pointing
-Mouse pointer is not trapped inside window

Git Repo:
https://github.com/pholtz/HOBL.git
git add ./* <-- in working directory
git commit -a -m "message"
git push -u origin master
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

//Modulated Shader
GLchar* mvertexFile = "modvert.vs";
GLchar* mfragmentFile = "modfrag.fs";
GLuint modProg;
GLuint number_of_lights;
GLint textureSampler;

//TEXTURE STUFF
/////////////////////////////
// Texture constants
#define TENNIS_BALL 0
#define DR_PEPPER 1
#define LID 2
#define TILE 3
#define WOOD_MID_SIDES 4
#define WOOD_MID_ENTRANCE 5
#define WALL_ART 6
#define WALLPAPER 7
#define WALLPAPER_NARROW 8
#define WALLPAPER_ROT 9
#define FLOOR_TILE 10
#define FALLING_SNOW 11
#define FROSTED_GLASS 12
#define STAGE_WOOD 13
#define STAGE_WOOD_ROT 14
#define TABLE_WOOD 15
#define TABLE_WOOD_LEG 16
#define WOOD_PILLAR 17
#define WOOD_STAIR 18
#define WOOD_STAIR_ROT 19
#define RAILING_ART 20
#define SAND 21
#define LIGHT_PANEL 22
#define STAGE_WOOD_NARROW 23
#define TAN_WALLPAPER 24
#define WOOD_PILLAR_ROT 25
#define POOL 26
#define DARK_WOOD 27
#define DARK_WOOD_PILLAR 28
#define DARK_WOOD_ROT 29
#define PAPER_WINDOW 30
#define LOWER_BAR 31
#define BAR_WOOD 32
#define WOOD_FLOOR_SIDING 33
#define RED_FELT 34
#define DARK_WOOD_STAIR 35
#define DARK_WOOD_STAIR_ROT 36
#define DARK_WOOD_PILLAR_ROT 37
#define WOOD_MID_UNDERSTAIR 38
#define CHAND_PAPER 39
#define TABLECLOTH 40
#define SHOJI 41
#define SILVER_METAL 42
#define WOOD_FLOOR_SIDING_PLANK 43
#define DARK_WOOD_PLANK 44
#define DARK_WOOD_PLANK_ROT 45
#define WOOD_PLANK 46
#define WOOD_PLANK_ROT 47
#define WALL_PANEL 48
#define STAGE_WOOD_LONG_SIDES 49
#define STAGE_WOOD_SHORT_SIDES 50
#define STAGE_WOOD_STAIR 51
#define STAGE_WOOD_STAIR_ROT 52
#define WOOD_SMALL 53
#define DARK_WOOD_SMALL 54
#define ORANGE 55
#define STAGE_WOOD_PILLAR 56
#define STAGE_WOOD_PILLAR_ROT 57
#define STAGE_WOOD_PLANK 58
#define CLEAR 59
#define NO_TEXTURES 60
// Texture indices
GLuint tex_ids[NO_TEXTURES];
// Texture files
char texture_files[NO_TEXTURES][20] = {"tb.jpg", "drp.jpg", "dlid.jpg", "tile.png", 
"wdflrms.jpg", "wdflrme.jpg", "jwart.jpg", "wallpaper.jpg", "wallpaper2.jpg", "wallpaperrot.jpg", "fltile.jpg", 
"dsnow.jpg", "frostglass.jpg", "stagewd.jpg", "stagewdrot.jpg", "wdgen.jpg", "wdgenleg.jpg", "wdpillar.jpg", 
"wdstair.jpg", "wdstairrot.jpg", "railingart.png", "sand.jpg", "lpanel.jpg", "stagewdnar.jpg", "tanwall.jpg", 
"wdpillarrot.jpg", "pool.jpg", "dwood.jpg", "dpillar.jpg", "dwoodrot.jpg", "paperwin.jpg", "blueglass.jpg", 
"barwd.jpg", "wdflrsides.jpg", "redfelt.jpg", "dstair.jpg", "dstairrot.jpg", "dpillarrot.jpg", "wdflrus.jpg", 
"chandppr.jpg", "tablecloth.jpg","shoji.png", "smetal.jpg", "wdflrsidesplank.jpg", "dplank.jpg", "dplankrot.jpg", 
"wdplank.jpg", "wdplankrot.jpg","wallpanel.jpg", "stagewdlsides.jpg", "stagewdssides.jpg", "stagewdstair.jpg", 
"stagewdstairrot.jpg", "wdsmall.jpg", "dwoodsmall.jpg", "orange.jpg", "stagewdp.jpg", "stagewdprot.jpg", 
"stagewdplank.jpg", "white.png"};
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
#define ROOF 6
#define ENTRANCE 7
#define STAIRS 8
#define MID_FIRST_FLOOR 9
#define BALCONY 10
#define CHAIR 11
#define BAR 12
#define FIRST_FLOOR_REAR 13
#define BAR_CHAIR 14

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
int normal_mode = VERTEX;

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
GLfloat light4_pos[4] = { 10.0f, 20.0f, 0.0f, 1.0f };
GLfloat light0_attenuation[3] = {1.0f, 0.0f, 0.0f};
GLfloat light1_attenuation[3] = {1.0f, 0.0f, 0.0f};
GLfloat light2_attenuation[3] = {1.0f, 0.0f, 0.0f};
GLfloat light3_attenuation[3] = {1.0f, 0.0f, 0.0f};
GLfloat light4_attenuation[3] = {1.0f, 0.0f, 0.0f};
GLfloat ambient_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat light_off[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
//lighting controls
GLboolean light0_toggle = true;
GLboolean light1_toggle = true;
GLboolean light2_toggle = true;
GLboolean light3_toggle = true;
GLboolean light4_toggle = true;
//////////
GLint num_lights = 5;

//animation variables
GLfloat fan_theta = 0;
GLfloat time = 0;
GLfloat lasttime = 0;
GLfloat fps = 0;
GLboolean fan_anim_flag = 0;
GLboolean light_toggle_flag = 0;

//declare quadrics
GLUquadricObj *fan_light;
GLUquadricObj *lightbulb;
GLUquadricObj *bowl;
GLUquadricObj *soda_can;
GLUquadricObj* moon;
GLUquadricObj* window;
GLUquadricObj* quadric;

//FUNCTION PROTOTYPES
void display();
void render_Scene();
void keyfunc(unsigned char key, int x, int y);
void passive(int x, int y);
void idlefunc();
void reshape(int w, int h);
void output_fps(char *string);
void draw_fan_blade();
void colorcube();
void rquad(GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat v4[], GLfloat n1[], GLfloat n2[], GLfloat n3[], GLfloat n4[]);
void div_quad(GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat v4[], GLfloat n1[], GLfloat n2[], GLfloat n3[], GLfloat n4[], int n);
void render_display_lists();
bool load_textures();
void render_lights();
void texcube();
void tex_quad(GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat v4[], GLfloat n1[], GLfloat n2[], GLfloat n3[], GLfloat n4[]);
void hybridcube(int top, int bottom, int left, int right, int front, int back);
void draw_chandelier();
void draw_fan();

//display list prototypes
void table_list();
void floor_list();
void wall_list();
void door_list();
void roof_list();
void entrance_list();
void stair_list();
void mid_first_floor_list();
void balcony_list();
void chair_list();
void bar_list();
void first_floor_rear_list();
void bar_chair_list();

int main(int argc, char *argv[])
{
	// Initialize GLUT
	glutInit(&argc,argv);

	// Initialize the window with double buffering and RGB colors
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// Set the window size to image size
	//glutInitWindowSize(512,512);
	//glutInitWindowSize(1280, 1020);
	glutInitWindowSize(1920, 1080);

	// Create window
	glutCreateWindow("House of Blue Leaves");
	glutFullScreen();	//make the window fullscreen

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

	//Enable alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//TEXTURE STUFF
	/////////////////////////////////////////////////
	// TODO: Create moon quadric with texture coords
	moon = gluNewQuadric();
	gluQuadricDrawStyle(moon, GLU_FILL);
	gluQuadricTexture(moon, GL_TRUE);

	soda_can = gluNewQuadric();
	gluQuadricDrawStyle(soda_can, GLU_FILL);
	gluQuadricTexture(soda_can, GL_TRUE);

	window = gluNewQuadric();
	gluQuadricDrawStyle(window, GLU_FILL);
	gluQuadricTexture(window, GL_TRUE);

	quadric = gluNewQuadric();
	gluQuadricDrawStyle(quadric, GLU_FILL);
	gluQuadricTexture(quadric, GL_TRUE);

	fan_light = gluNewQuadric();
	lightbulb = gluNewQuadric();
	bowl = gluNewQuadric();

	// Load textures
	if (!load_textures())
	{
		exit(0);
	}

	//Set up Modular Shader
	modProg = load_shaders(mvertexFile, mfragmentFile);
	glUseProgram(modProg);
	///////////////////////
	textureSampler = glGetUniformLocation(modProg, "textureMap");
	glUniform1i(textureSampler, 0);
	///////////////////////
	number_of_lights = glGetUniformLocation(modProg, "num_lights");

	//Render the display lists
	render_display_lists();

	// Begin event loop
	glutMainLoop();
	return 0;
}

// Display callback
void display()
{
	// Set number of lights
	glUniform1i(number_of_lights, num_lights);

	// Reset background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//perspective projection
	glFrustum(-0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 200.0f);

	// Set modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//set the camera position
	gluLookAt(eye[X], eye[Y], eye[Z], at[X], at[Y], at[Z], up[X], up[Y], up[Z]);

	//output the fps string
	output_fps(fps_str);

	// Render scene
	render_Scene();

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

	if (key == '1')
	{
		light0_toggle = !light0_toggle;
	}
	else if (key == '2')
	{
		light1_toggle = !light1_toggle;
	}
	else if (key == '3')
	{
		light2_toggle = !light2_toggle;
	}
	else if (key == '4')
	{
		light3_toggle = !light3_toggle;
	}
	else if (key == '5')
	{
		light4_toggle = !light4_toggle;
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

	if ((int)time % 10 == 0)
	{
		sprintf(fps_str, "%3.2f", fps);
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


void output_fps(char *string)
{
	//////////////////////
	//    FPS Printer   //
	//////////////////////
	// Save previous matrices, switch to 2D, and turn off lighting for text
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos2f(-0.975f, 0.97f);
	for (int i = 0; i < (int)strlen(string); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
	}

	// Restore previous matrices and turn on lighting
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	/////////////////////////
}


void render_lights()
{
	//draw lightbulbs
	glPushMatrix();
	set_material(GL_FRONT_AND_BACK, &clear);
	glBindTexture(GL_TEXTURE_2D, tex_ids[TENNIS_BALL]);
	glTranslatef(light0_pos[0], light0_pos[1], light0_pos[2]);
	gluSphere(moon, 1.0f, 20, 20);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(light1_pos[0], light1_pos[1], light1_pos[2]);
	gluSphere(moon, 1.0f, 20, 20);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(light2_pos[0], light2_pos[1], light2_pos[2]);
	gluSphere(moon, 1.0f, 20, 20);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(light3_pos[0], light3_pos[1], light3_pos[2]);
	gluSphere(moon, 1.0f, 20, 20);
	glPopMatrix();

	//Lighting
	//4 corner lights
	if (light0_toggle)
	{
		set_PointLightAttenuated(GL_LIGHT0, &white_light, light0_pos, light0_attenuation);
	}
	else
	{
		set_PointLightAttenuated(GL_LIGHT0, &black_light, light0_pos, light0_attenuation);
	}

	if (light1_toggle)
	{
		set_PointLightAttenuated(GL_LIGHT1, &white_light, light1_pos, light1_attenuation);
	}
	else
	{
		set_PointLightAttenuated(GL_LIGHT1, &black_light, light1_pos, light1_attenuation);
	}

	if (light2_toggle)
	{
		set_PointLightAttenuated(GL_LIGHT2, &white_light, light2_pos, light2_attenuation);
	}
	else
	{
		set_PointLightAttenuated(GL_LIGHT2, &black_light, light2_pos, light2_attenuation);
	}

	if (light3_toggle)
	{
		set_PointLightAttenuated(GL_LIGHT3, &white_light, light3_pos, light3_attenuation);
	}
	else
	{
		set_PointLightAttenuated(GL_LIGHT3, &black_light, light3_pos, light3_attenuation);
	}

	if (light4_toggle)
	{
		set_PointLightAttenuated(GL_LIGHT4, &white_light, light4_pos, light4_attenuation);
	}
	else
	{
		set_PointLightAttenuated(GL_LIGHT4, &black_light, light4_pos, light4_attenuation);
	}
}

// Scene render function
void render_Scene()
{
	//Set up the lights and light markers
	render_lights();

	//FLAT ROOF WITH ACCENTS
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, tex_ids[CLEAR]);
	set_material(GL_FRONT, &faux_wood);
	glCallList(ROOF);
	glPopMatrix();

	//BASIC FLOOR QUADRIC
	glPushMatrix();
	set_material(GL_FRONT, &clear);
	glCallList(FLOOR);
	glPopMatrix();

	//BASIC WALLS
	glPushMatrix();
	set_material(GL_FRONT, &clear);
	glCallList(WALLS);
	glPopMatrix();

	//BASIC DOOR
	glPushMatrix();
	set_material(GL_FRONT, &clear);
	glCallList(DOOR);
	glPopMatrix();

	//FAN(S)
	glPushMatrix();
	set_material(GL_FRONT, &clear);
	glTranslatef(25.0f, 5.0f, HOUSE_Z - 7.5f);
	draw_fan();
	glTranslatef(20.0f, 0.0f, 0.0f);
	draw_fan();
	glPopMatrix();

	//ENTRANCE
	glPushMatrix();
	set_material(GL_FRONT, &clear);
	glCallList(ENTRANCE);
	glPopMatrix();

	//SECOND FLOOR
	glPushMatrix();
	set_material(GL_FRONT, &clear);
	glCallList(BALCONY);
	glPopMatrix();


	//MID FIRST FLOOR
	glPushMatrix();
	set_material(GL_FRONT, &clear);
	glCallList(MID_FIRST_FLOOR);
	glPopMatrix();


	//STAIRS TO SECOND FLOOR
	glPushMatrix();
	set_material(GL_FRONT, &clear);
	glCallList(STAIRS);
	glPopMatrix();

	//FIRST FLOOR -- BAR
	glPushMatrix();
	set_material(GL_FRONT, &clear);
	glCallList(BAR);
	glPopMatrix();

	//FIRST FLOOR -- REAR
	glPushMatrix();
	set_material(GL_FRONT, &clear);
	glCallList(FIRST_FLOOR_REAR);
	glPopMatrix();

}


void render_display_lists()
{
	table_list();
	floor_list();
	wall_list();
	door_list();
	roof_list();
	entrance_list();
	stair_list();
	mid_first_floor_list();
	balcony_list();
	chair_list();
	bar_list();
	first_floor_rear_list();
	bar_chair_list();
}




////////////////////////////////
//BEGIN DISPLAY LIST FUNCTIONS//
////////////////////////////////
void first_floor_rear_list()
{
	glNewList(FIRST_FLOOR_REAR, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);

	//FIRST FLOOR REAR -- under z pos stairs
	glPushMatrix();
	glTranslatef(-48.5f, 10.0f, 27.5f);
	glScalef(15.0f, 20.0f, 15.0f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, WALLPAPER, NO_TEXTURES, TAN_WALLPAPER);
	glPopMatrix();
	//FIRST FLOOR REAR -- under z neg stairs pillar
	glPushMatrix();
	glTranslatef(-41.0f, 10.0f, 20.0f);
	glScalef(0.5f, 20.0f, 0.5f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, WOOD_PILLAR, WOOD_PILLAR, WOOD_PILLAR, WOOD_PILLAR);
	glPopMatrix();
	//FIRST FLOOR REAR -- under z neg stairs
	glPushMatrix();
	glTranslatef(-48.5f, 10.0f, -27.5f);
	glScalef(15.0f, 20.0f, 15.0f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, WALLPAPER, TAN_WALLPAPER, NO_TEXTURES);
	glPopMatrix();
	//FIRST FLOOR REAR -- under z neg stairs pillar
	glPushMatrix();
	glTranslatef(-41.0f, 10.0f, -20.0f);
	glScalef(0.5f, 20.0f, 0.5f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, WOOD_PILLAR, WOOD_PILLAR, WOOD_PILLAR, WOOD_PILLAR);
	glPopMatrix();

	//FIRST FLOOR REAR -- Z POS
	glPushMatrix();
	glTranslatef(-50.0f, 10.0f, -42.5f);
	glScalef(50.0f, 20.0f, 15.0f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, WALL_PANEL, WALLPAPER_NARROW, NO_TEXTURES);
	glPopMatrix();
	//FIRST FLOOR REAR -- Z NEG
	glPushMatrix();
	glTranslatef(-50.0f, 10.0f, 42.5f);
	glScalef(50.0f, 20.0f, 15.0f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, WALL_PANEL, NO_TEXTURES, WALLPAPER_NARROW);
	glPopMatrix();
	///// pillars
	glPushMatrix();
	glTranslatef(-25.0f, 10.0f, 35.0f);
	glScalef(0.5f, 20.0f, 0.5f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, WOOD_PILLAR, WOOD_PILLAR, WOOD_PILLAR, WOOD_PILLAR);
	glPopMatrix();
	///// pillars
	glPushMatrix();
	glTranslatef(-25.0f, 10.0f, -35.0f);
	glScalef(0.5f, 20.0f, 0.5f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, WOOD_PILLAR, WOOD_PILLAR, WOOD_PILLAR, WOOD_PILLAR);
	glPopMatrix();


	//glUseProgram(lightProg);
	set_material(GL_FRONT_AND_BACK, &faux_wood);

	//FIRST FLOOR REAR -- MID
	glPushMatrix();
	glTranslatef(-65.5f, 10.0f, 0.0f);
	glScalef(19.0f, 20.0f, 70.0f);
	colorcube();
	glPopMatrix();

	//HANGING WALL ART STAGE BACKDROP
	//glUseProgram(textureProg);
	glBindTexture(GL_TEXTURE_2D, tex_ids[WALL_ART]);
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-55.0f, 2.5f, -20.0f);
	glTexCoord2f(0, 1);
	glVertex3f(-55.0f, 20.f, -20.0f);
	glTexCoord2f(1, 1);
	glVertex3f(-55.0f, 20.0f, 20.0f);
	glTexCoord2f(1, 0);
	glVertex3f(-55.0f, 2.5f, 20.0f);
	glEnd();
	glPopMatrix();
	//glUseProgram(lightProg);

	glPopAttrib();
	glEndList();
}


void bar_list()
{
	glNewList(BAR, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);

	//BAR
	//glUseProgram(textureProg);
	glPushMatrix();	//master push
	glTranslatef(25.0f, 6.0f, -41.0f);
	glPushMatrix();
	glScalef(40.0f, 5.0f, 2.5f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, DARK_WOOD_STAIR, DARK_WOOD_STAIR, LOWER_BAR, DARK_WOOD_ROT);
	glPopMatrix();
	glTranslatef(0.0f, 2.75f, 0.0f);
	glScalef(42.0f, 0.5f, 4.0f);
	hybridcube(DARK_WOOD_ROT, NO_TEXTURES, DARK_WOOD_PLANK_ROT, DARK_WOOD_PLANK_ROT, DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT);
	glPopMatrix();
	glPopMatrix();	//master pop
	//////////////
	glPushMatrix();
	glTranslatef(5.0f, 6.0f, -39.75f);
	glScalef(0.25f, 5.0f, 0.25f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR);
	glPopMatrix();
	//////////////
	glPushMatrix();
	glTranslatef(5.0f, 6.0f, -42.25f);
	glScalef(0.25f, 5.0f, 0.25f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR);
	glPopMatrix();
	//////////////
	glPushMatrix();
	glTranslatef(45.0f, 6.0f, -39.75f);
	glScalef(0.25f, 5.0f, 0.25f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR);
	glPopMatrix();
	/////////////
	glPushMatrix();
	glTranslatef(45.0f, 6.0f, -42.25f);
	glScalef(0.25f, 5.0f, 0.25f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR);
	glPopMatrix();
	//////////////

	//Z NEG WINDOW (MID CIRCLE)
	glPushMatrix();
	//glUseProgram(textureProg);
	glBindTexture(GL_TEXTURE_2D, tex_ids[FALLING_SNOW]);
	glTranslatef(25.0f, 15.0f, -HOUSE_Z + 0.1f);
	gluDisk(window, 0.0f, 4.0f, 50, 50);
	//glUseProgram(lightProg);
	set_material(GL_FRONT, &black_plastic);
	gluDisk(quadric, 4.0f, 4.25f, 50, 50);
	glPushMatrix();
	glScalef(7.9f, 0.25f, 0.2f);
	colorcube();
	glPopMatrix();
	glPushMatrix();
	glScalef(0.25f, 7.9f, 0.2f);
	colorcube();
	glPopMatrix();
	//glUseProgram(textureProg);
	glPopMatrix();

	//Z NEG WINDOW (RIGHT PANE)
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, tex_ids[FROSTED_GLASS]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(45.0f, 19.0f, -HOUSE_Z + 0.1f);
	glTexCoord2f(0, 1);
	glVertex3f(45.0f, 11.0f, -HOUSE_Z + 0.1f);
	glTexCoord2f(1, 1);
	glVertex3f(35.0f, 11.0f, -HOUSE_Z + 0.1f);
	glTexCoord2f(1, 0);
	glVertex3f(35.0f, 19.0f, -HOUSE_Z + 0.1f);
	glEnd();
	glPopMatrix();
	glPushMatrix();
	//glUseProgram(lightProg);
	glTranslatef(35.0f, 15.0f, -HOUSE_Z + 0.1f);
	for (int i = 0; i < 6; i++)
	{
		glPushMatrix();
		glScalef(0.1f, 8.0f, 0.1f);
		colorcube();
		glPopMatrix();
		glTranslatef(2.0f, 0.0f, 0.0f);
	}
	glPopMatrix();
	////////////////
	glPushMatrix();
	glTranslatef(40.0f, 19.0f, -HOUSE_Z + 0.1f);
	for (int i = 0; i < 5; i++)
	{
		glPushMatrix();
		glScalef(10.0f, 0.1f, 0.1f);
		colorcube();
		glPopMatrix();
		glTranslatef(0.0f, -2.0f, 0.0f);
	}
	glPopMatrix();
	//glUseProgram(textureProg);

	//Z NEG WINDOW (LEFT PANE)
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, tex_ids[FROSTED_GLASS]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(15.0f, 19.0f, -HOUSE_Z + 0.1f);
	glTexCoord2f(0, 1);
	glVertex3f(15.0f, 11.0f, -HOUSE_Z + 0.1f);
	glTexCoord2f(1, 1);
	glVertex3f(5.0f, 11.0f, -HOUSE_Z + 0.1f);
	glTexCoord2f(1, 0);
	glVertex3f(5.0f, 19.0f, -HOUSE_Z + 0.1f);
	glEnd();
	glPopMatrix();
	//////////////
	glPushMatrix();
	//glUseProgram(lightProg);
	glTranslatef(5.0f, 15.0f, -HOUSE_Z + 0.1f);
	for (int i = 0; i < 6; i++)
	{
		glPushMatrix();
		glScalef(0.1f, 8.0f, 0.1f);
		colorcube();
		glPopMatrix();
		glTranslatef(2.0f, 0.0f, 0.0f);
	}
	glPopMatrix();
	////////////////
	glPushMatrix();
	glTranslatef(10.0f, 19.0f, -HOUSE_Z + 0.1f);
	for (int i = 0; i < 5; i++)
	{
		glPushMatrix();
		glScalef(10.0f, 0.1f, 0.1f);
		colorcube();
		glPopMatrix();
		glTranslatef(0.0f, -2.0f, 0.0f);
	}
	glPopMatrix();
	//glUseProgram(textureProg);


	//BAR BUMPOUTS
	glPushMatrix();	//master push
	glTranslatef(17.5f, 12.0f, -HOUSE_Z + 1.5f);
	glPushMatrix();
	glScalef(0.4f, 16.0f, 3.0f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, WALLPAPER_ROT, WALLPAPER_ROT, NO_TEXTURES, NO_TEXTURES);
	glPopMatrix();
	glTranslatef(0.0f, 0.0f, 1.6f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR);
	glPopMatrix();
	glPopMatrix();	//master pop
	///////////////
	glPushMatrix();
	glTranslatef(32.5f, 12.0f, -HOUSE_Z + 1.5f);
	glPushMatrix();
	glScalef(0.4f, 16.0f, 3.0f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, WALLPAPER_ROT, WALLPAPER_ROT, NO_TEXTURES, NO_TEXTURES);
	glPopMatrix();
	glTranslatef(0.0f, 0.0f, 1.6f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR);
	glPopMatrix();	//master pop
	glPopMatrix();	//master pop


	//BAR CHAIRS
	glPushMatrix();
	glTranslatef(7.5f, 4.0f, -38.0f);
	glRotatef(-90, 0, 1, 0);
	///////////////
	glPushMatrix();
	glRotatef(45, 0, 1, 0);
	glCallList(BAR_CHAIR);
	glPopMatrix();
	///////////////
	glTranslatef(0.0f, 0.0f, -3.0f);
	///////////////
	glPushMatrix();
	glRotatef(10, 0, 1, 0);
	glCallList(BAR_CHAIR);
	glPopMatrix();
	///////////////
	glTranslatef(0.0f, 0.0f, -3.0f);
	///////////////
	glPushMatrix();
	glRotatef(0, 0, 1, 0);
	glCallList(BAR_CHAIR);
	glPopMatrix();
	///////////////
	glTranslatef(0.0f, 0.0f, -3.0f);
	///////////////
	glPushMatrix();
	glRotatef(-30, 0, 1, 0);
	glCallList(BAR_CHAIR);
	glPopMatrix();
	///////////////
	glTranslatef(0.0f, 0.0f, -3.0f);
	///////////////
	glPushMatrix();
	glRotatef(12, 0, 1, 0);
	glCallList(BAR_CHAIR);
	glPopMatrix();
	///////////////
	glTranslatef(0.0f, 0.0f, -3.0f);
	///////////////
	glPushMatrix();
	glRotatef(0, 0, 1, 0);
	glCallList(BAR_CHAIR);
	glPopMatrix();
	///////////////
	glTranslatef(0.0f, 0.0f, -3.0f);
	///////////////
	glPushMatrix();
	glRotatef(-9, 0, 1, 0);
	glCallList(BAR_CHAIR);
	glPopMatrix();
	///////////////
	glTranslatef(0.0f, 0.0f, -3.0f);
	///////////////
	glPushMatrix();
	glRotatef(0, 0, 1, 0);
	glCallList(BAR_CHAIR);
	glPopMatrix();
	///////////////
	glTranslatef(0.0f, 0.0f, -3.0f);
	///////////////
	glPushMatrix();
	glRotatef(0, 0, 1, 0);
	glCallList(BAR_CHAIR);
	glPopMatrix();
	///////////////
	glTranslatef(0.0f, 0.0f, -3.0f);
	///////////////
	glPushMatrix();
	glRotatef(16, 0, 1, 0);
	glCallList(BAR_CHAIR);
	glPopMatrix();
	///////////////
	glTranslatef(0.0f, 0.0f, -3.0f);
	///////////////
	glPushMatrix();
	glRotatef(0, 0, 1, 0);
	glCallList(BAR_CHAIR);
	glPopMatrix();
	///////////////
	glTranslatef(0.0f, 0.0f, -3.0f);
	///////////////
	glPushMatrix();
	glRotatef(-30, 0, 1, 0);
	glCallList(BAR_CHAIR);
	glPopMatrix();
	///////////////
	glPopMatrix();

	glPopAttrib();
	glEndList();
}


void balcony_list()
{
	glNewList(BALCONY, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);

	//SECOND FLOOR -- Z NEG
	//glUseProgram(textureProg);
	set_material(GL_FRONT, &faux_wood);
	glPushMatrix();
	glTranslatef(0.0f, SECOND_FLOOR, 42.5f);
	glScalef(150.0f, 0.25f, 15.0f);
	hybridcube(DARK_WOOD_ROT, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES);
	glPopMatrix();
	//Z NEG SIDING-FLOOR BUFFER
	glPushMatrix();
	glTranslatef(17.5f, SECOND_FLOOR + 0.25f, 35.25f);
	glScalef(115.0f, 0.5f, 0.5f);
	hybridcube(DARK_WOOD_PILLAR_ROT, NO_TEXTURES, DARK_WOOD_SMALL, DARK_WOOD_SMALL, DARK_WOOD_PILLAR_ROT, NO_TEXTURES);
	glPopMatrix();
	//SECOND FLOOR Z NEG SIDING
	glPushMatrix();
	glTranslatef(17.5f, SECOND_FLOOR + 1.0f, -35.0f);
	glScalef(115.0f, 2.0f, 0.25f);
	hybridcube(DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_SMALL, DARK_WOOD_SMALL, DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT);
	glPopMatrix();
	//Z NEG RAILING HORIZONTAL
	glPushMatrix();
	glTranslatef(17.5f, SECOND_FLOOR + 4.0f, -35.0f);
	glScalef(115.0f, 0.25f, 0.25f);
	hybridcube(DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT, DARK_WOOD_SMALL, DARK_WOOD_SMALL, DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT);
	glPopMatrix();
	//Z NEG MINI RAILINGS
	glPushMatrix();
	glTranslatef(17.5f, SECOND_FLOOR + 2.5f, -35.0f);
	glScalef(115.0f, 0.1f, 0.2f);
	hybridcube(DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT, DARK_WOOD_SMALL, DARK_WOOD_SMALL, DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT);
	glPopMatrix();
	//////////////
	glPushMatrix();
	glTranslatef(17.5f, SECOND_FLOOR + 3.25f, -35.0f);
	glScalef(115.0f, 0.1f, 0.2f);
	hybridcube(DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT, DARK_WOOD_SMALL, DARK_WOOD_SMALL, DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT);
	glPopMatrix();
	//Z NEG RAILING VERTICAL
	//glUseProgram(textureProg);
	glPushMatrix();
	glTranslatef(-41.875f, SECOND_FLOOR + 2.85f, -35.0f);
	for (int i = 0; i < 22; i++)
	{
		//create the vertical railing
		glTranslatef(2.0f, 0.15f, 0.0f);
		glPushMatrix();
		glScalef(0.25f, 2.25f, 0.25f);
		hybridcube(DARK_WOOD_SMALL, DARK_WOOD_SMALL, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR);
		glPopMatrix();

		//create the boxed railing art
		glTranslatef(2.0f, -0.15f, 0.0f);
		glPushMatrix();
		glScalef(0.1f, 0.75f, 0.2f);
		glBindTexture(GL_TEXTURE_2D, tex_ids[DARK_WOOD_SMALL]);
		texcube();
		glPopMatrix();
		///////////
		glTranslatef(0.5f, 0.0f, 0.0f);
		glPushMatrix();
		glScalef(1.0f, 0.75f, 0.15f);
		hybridcube(NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, RAILING_ART, RAILING_ART);
		glPopMatrix();
		///////////
		glTranslatef(0.5f, 0.0f, 0.0f);
		glPushMatrix();
		glScalef(0.1f, 0.75f, 0.2f);
		glBindTexture(GL_TEXTURE_2D, tex_ids[DARK_WOOD_SMALL]);
		texcube();
		glPopMatrix();
	}
	glPopMatrix();

	//SECOND FLOOR -- Z POS
	//glUseProgram(textureProg);
	glPushMatrix();
	glTranslatef(0.0f, SECOND_FLOOR, -42.5f);
	glScalef(150.f, 0.25f, 15.0f);
	hybridcube(DARK_WOOD_ROT, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES);
	glPopMatrix();
	//SECOND FLOOR Z POS SIDING
	glPushMatrix();
	glTranslatef(17.5f, SECOND_FLOOR + 1.0f, 35.0f);
	glScalef(115.0f, 2.0f, 0.25f);
	hybridcube(DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_SMALL, DARK_WOOD_SMALL, DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT);
	glPopMatrix();
	//Z POS SIDING-FLOOR BUFFER
	glPushMatrix();
	glTranslatef(17.5f, SECOND_FLOOR + 0.25f, -35.25f);
	glScalef(115.0f, 0.5f, 0.5f);
	hybridcube(DARK_WOOD_PILLAR_ROT, NO_TEXTURES, DARK_WOOD_SMALL, DARK_WOOD_SMALL, NO_TEXTURES, DARK_WOOD_PILLAR_ROT);
	glPopMatrix();
	//Z POS RAILING
	glPushMatrix();
	glTranslatef(17.5f, SECOND_FLOOR + 4.0f, 35.0f);
	glScalef(115.0f, 0.25f, 0.25f);
	hybridcube(DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT, DARK_WOOD_SMALL, DARK_WOOD_SMALL, DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT);
	glPopMatrix();
	//Z POS MINI RAILINGS
	glPushMatrix();
	glTranslatef(17.5f, SECOND_FLOOR + 2.5f, 35.0f);
	glScalef(115.0f, 0.1f, 0.2f);
	hybridcube(DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT, DARK_WOOD_SMALL, DARK_WOOD_SMALL, DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT);
	glPopMatrix();
	//////////////
	glPushMatrix();
	glTranslatef(17.5f, SECOND_FLOOR + 3.25f, 35.0f);
	glScalef(115.0f, 0.1f, 0.2f);
	hybridcube(DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT, DARK_WOOD_SMALL, DARK_WOOD_SMALL, DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT);
	glPopMatrix();
	//Z POS RAILING VERTICAL
	glPushMatrix();
	glTranslatef(-41.875f, SECOND_FLOOR + 2.85f, 35.0f);
	for (int i = 0; i < 22; i++)
	{
		//create the vertical railing
		glTranslatef(2.0f, 0.15f, 0.0f);
		glPushMatrix();
		glScalef(0.25f, 2.25f, 0.25f);
		hybridcube(DARK_WOOD_SMALL, DARK_WOOD_SMALL, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR);
		glPopMatrix();

		//create the boxed railing art
		glTranslatef(2.0f, -0.15f, 0.0f);
		glPushMatrix();
		glScalef(0.1f, 0.75f, 0.2f);
		glBindTexture(GL_TEXTURE_2D, tex_ids[DARK_WOOD_SMALL]);
		texcube();
		glPopMatrix();
		/////////////
		glTranslatef(0.5f, 0.0f, 0.0f);
		glPushMatrix();
		glScalef(1.0f, 0.75f, 0.15f);
		hybridcube(NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, RAILING_ART, RAILING_ART);
		glPopMatrix();
		////////////
		set_material(GL_FRONT, &faux_wood);
		glTranslatef(0.5f, 0.0f, 0.0f);
		glPushMatrix();
		glScalef(0.1f, 0.75f, 0.2f);
		glBindTexture(GL_TEXTURE_2D, tex_ids[DARK_WOOD_SMALL]);
		texcube();
		glPopMatrix();
	}
	glPopMatrix();

	//SECOND FLOOR BALCONY
	//glUseProgram(textureProg);
	glPushMatrix();
	glTranslatef(-57.5f, SECOND_FLOOR, 0.0f);
	glScalef(35.0f, 0.25f, 70.0f);
	hybridcube(DARK_WOOD, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES);
	glPopMatrix();
	//BALCONY SIDING (with room for stairs)
	glPushMatrix();
	glTranslatef(-40.0f, SECOND_FLOOR + 1.0f, 0.0f);
	glScalef(0.25f, 2.0f, 45.0f);
	hybridcube(DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT, DARK_WOOD_SMALL, DARK_WOOD_SMALL);
	glPopMatrix();
	//BALCONY RAILING HORIZONTAL
	glPushMatrix();
	glTranslatef(-40.0f, SECOND_FLOOR + 4.0f, 0.0f);
	glScalef(0.25f, 0.25f, 45.0f);
	hybridcube(DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_SMALL, DARK_WOOD_SMALL);
	glPopMatrix();
	//Z POS MINI RAILINGS
	glPushMatrix();
	glTranslatef(-40.0f, SECOND_FLOOR + 2.5f, 0.0f);
	glScalef(0.2f, 0.1f, 45.0f);
	hybridcube(DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_SMALL, DARK_WOOD_SMALL);
	glPopMatrix();
	//////////////
	glPushMatrix();
	glTranslatef(-40.0f, SECOND_FLOOR + 3.25f, 0.0f);
	glScalef(0.2f, 0.1f, 45.0f);
	hybridcube(DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_SMALL, DARK_WOOD_SMALL);
	glPopMatrix();
	//BALCONY RAILING VERTICAL
	glPushMatrix();
	glTranslatef(-40.0f, SECOND_FLOOR + 2.85f, -24.35f);
	for (int i = 0; i < 10; i++)
	{
		//create the vertical railing
		glTranslatef(0.0f, 0.15f, 1.9725f);
		glPushMatrix();
		glScalef(0.25f, 2.25f, 0.25f);
		hybridcube(DARK_WOOD_SMALL, DARK_WOOD_SMALL, DARK_WOOD_SMALL, DARK_WOOD_SMALL, DARK_WOOD_SMALL, DARK_WOOD_SMALL);
		glPopMatrix();

		if (i < 9)
		{
			//create the boxed railing art
			glTranslatef(0.0f, -0.15f, 2.0f);
			glPushMatrix();
			glScalef(0.2f, 0.75f, 0.1f);
			glBindTexture(GL_TEXTURE_2D, tex_ids[DARK_WOOD_SMALL]);
			texcube();
			glPopMatrix();
			/////////////
			glTranslatef(0.0f, 0.0f, 0.5f);
			glPushMatrix();
			glScalef(0.15f, 0.75f, 1.0f);
			hybridcube(NO_TEXTURES, NO_TEXTURES, RAILING_ART, RAILING_ART, NO_TEXTURES, NO_TEXTURES);
			glPopMatrix();
			////////////
			glTranslatef(0.0f, 0.0f, 0.5f);
			glPushMatrix();
			glScalef(0.2f, 0.75f, 0.1f);
			//glUseProgram(textureProg);
			glBindTexture(GL_TEXTURE_2D, tex_ids[DARK_WOOD_SMALL]);
			texcube();
			glPopMatrix();
		}
	}
	glPopMatrix();

	//SECOND FLOOR ENTRANCE WALL BUMPOUT
	//glUseProgram(textureProg);
	set_material(GL_FRONT_AND_BACK, &clear);
	glPushMatrix();
	glTranslatef(70.125f, 27.5f, 0.0f);
	glScalef(9.75f, 25.0f, 100.0f);
	//texcube();
	hybridcube(NO_TEXTURES, WALLPAPER_ROT, WALLPAPER_NARROW, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES);
	glPopMatrix();

	//ENTRANCE WINDOWS
	//glUseProgram(textureProg);
	glBindTexture(GL_TEXTURE_2D, tex_ids[PAPER_WINDOW]);
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex3f(65.2f, SECOND_FLOOR + 15.0f, 10.0f);
	glTexCoord2f(0, 0);
	glVertex3f(65.2f, SECOND_FLOOR + 5.0f, 10.0f);
	glTexCoord2f(2, 0);
	glVertex3f(65.2f, SECOND_FLOOR + 5.0f, -10.0f);
	glTexCoord2f(2, 1);
	glVertex3f(65.2f, SECOND_FLOOR + 15.0f, -10.0f);
	glEnd();
	glPopMatrix();


	//BALCONY WALL BUMPOUT
	//set_material(GL_FRONT, &emerald);
	//set_material(GL_FRONT, &faux_wood);
	glBindTexture(GL_TEXTURE_2D, tex_ids[WALLPAPER_NARROW]);
	glPushMatrix();
	glTranslatef(-67.5f, SECOND_FLOOR + 10.0f, 0.0f);
	glScalef(15.0f, 20.0f, 100.0f);
	//texcube();
	hybridcube(NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, WALLPAPER_NARROW, NO_TEXTURES, NO_TEXTURES);
	glPopMatrix();

	//BALCONY WINDOWS (2 inner and 1 outer)
	//glUseProgram(textureProg);
	glBindTexture(GL_TEXTURE_2D, tex_ids[PAPER_WINDOW]);
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex3f(-59.9f, SECOND_FLOOR + 15.0f, 2.5f);
	glTexCoord2f(0, 0);
	glVertex3f(-59.9f, SECOND_FLOOR + 5.0f, 2.5f);
	glTexCoord2f(1, 0);
	glVertex3f(-59.9f, SECOND_FLOOR + 5.0f, 10.0f);
	glTexCoord2f(1, 1);
	glVertex3f(-59.9f, SECOND_FLOOR + 15.0f, 10.0f);
	glEnd();
	glPopMatrix();
	///////////////
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex3f(-59.9f, SECOND_FLOOR + 15.0f, -2.5f);
	glTexCoord2f(0, 0);
	glVertex3f(-59.9f, SECOND_FLOOR + 5.0f, -2.5f);
	glTexCoord2f(1, 0);
	glVertex3f(-59.9f, SECOND_FLOOR + 5.0f, -10.0f);
	glTexCoord2f(1, 1);
	glVertex3f(-59.9f, SECOND_FLOOR + 15.0f, -10.0f);
	glEnd();
	glPopMatrix();
	//////////////
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex3f(-59.9f, SECOND_FLOOR + 15.0f, -37.5f);
	glTexCoord2f(0, 0);
	glVertex3f(-59.9f, SECOND_FLOOR + 5.0f, -37.5f);
	glTexCoord2f(1, 0);
	glVertex3f(-59.9f, SECOND_FLOOR + 5.0f, -47.5f);
	glTexCoord2f(1, 1);
	glVertex3f(-59.9f, SECOND_FLOOR + 15.0f, -47.5f);
	glEnd();
	glPopMatrix();


	//BALCONY DOORS (2 Quads for each set of doors -- 4 quads total)
	glBindTexture(GL_TEXTURE_2D, tex_ids[SHOJI]);
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex3f(-59.9f, SECOND_FLOOR + 15.0f, -25.0f);
	glTexCoord2f(0, 0);
	glVertex3f(-59.9f, SECOND_FLOOR, -25.0f);
	glTexCoord2f(1, 0);
	glVertex3f(-59.9f, SECOND_FLOOR, -35.0f);
	glTexCoord2f(1, 1);
	glVertex3f(-59.9f, SECOND_FLOOR + 15.0f, -35.0f);
	glEnd();
	glPopMatrix();
	//////////////
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex3f(-59.9f, SECOND_FLOOR + 15.0f, -15.0f);
	glTexCoord2f(0, 0);
	glVertex3f(-59.9f, SECOND_FLOOR, -15.0f);
	glTexCoord2f(1, 0);
	glVertex3f(-59.9f, SECOND_FLOOR, -25.0f);
	glTexCoord2f(1, 1);
	glVertex3f(-59.9f, SECOND_FLOOR + 15.0f, -25.0f);
	glEnd();
	glPopMatrix();
	//////////////
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex3f(-59.9f, SECOND_FLOOR + 15.0f, 25.0f);
	glTexCoord2f(0, 0);
	glVertex3f(-59.9f, SECOND_FLOOR, 25.0f);
	glTexCoord2f(1, 0);
	glVertex3f(-59.9f, SECOND_FLOOR, 35.0f);
	glTexCoord2f(1, 1);
	glVertex3f(-59.9f, SECOND_FLOOR + 15.0f, 35.0f);
	glEnd();
	glPopMatrix();
	//////////////
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex3f(-59.9f, SECOND_FLOOR + 15.0f, 15.0f);
	glTexCoord2f(0, 0);
	glVertex3f(-59.9f, SECOND_FLOOR, 15.0f);
	glTexCoord2f(1, 0);
	glVertex3f(-59.9f, SECOND_FLOOR, 25.0f);
	glTexCoord2f(1, 1);
	glVertex3f(-59.9f, SECOND_FLOOR + 15.0f, 25.0f);
	glEnd();
	glPopMatrix();


	//Z NEG WALKWAY DOORS (2 sets for now)
	glPushMatrix();	//first set
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex3f(-30.0f, SECOND_FLOOR + 15.0f, -HOUSE_Z + 0.05f);
	glTexCoord2f(0, 0);
	glVertex3f(-30.0f, SECOND_FLOOR, -HOUSE_Z + 0.05f);
	glTexCoord2f(1, 0);
	glVertex3f(-20.0f, SECOND_FLOOR, -HOUSE_Z + 0.05f);
	glTexCoord2f(1, 1);
	glVertex3f(-20.0f, SECOND_FLOOR + 15.0f, -HOUSE_Z + 0.05f);
	glEnd();
	glPopMatrix();
	//////////////
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex3f(-20.0f, SECOND_FLOOR + 15.0f, -HOUSE_Z + 0.05f);
	glTexCoord2f(0, 0);
	glVertex3f(-20.0f, SECOND_FLOOR, -HOUSE_Z + 0.05f);
	glTexCoord2f(1, 0);
	glVertex3f(-10.0f, SECOND_FLOOR, -HOUSE_Z + 0.05f);
	glTexCoord2f(1, 1);
	glVertex3f(-10.0f, SECOND_FLOOR + 15.0f, -HOUSE_Z + 0.05f);
	glEnd();
	glPopMatrix();
	////////////// 2nd set
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex3f(20.0f, SECOND_FLOOR + 15.0f, -HOUSE_Z + 0.05f);
	glTexCoord2f(0, 0);
	glVertex3f(20.0f, SECOND_FLOOR, -HOUSE_Z + 0.05f);
	glTexCoord2f(1, 0);
	glVertex3f(30.0f, SECOND_FLOOR, -HOUSE_Z + 0.05f);
	glTexCoord2f(1, 1);
	glVertex3f(30.0f, SECOND_FLOOR + 15.0f, -HOUSE_Z + 0.05f);
	glEnd();
	glPopMatrix();
	//////////////
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex3f(30.0f, SECOND_FLOOR + 15.0f, -HOUSE_Z + 0.05f);
	glTexCoord2f(0, 0);
	glVertex3f(30.0f, SECOND_FLOOR, -HOUSE_Z + 0.05f);
	glTexCoord2f(1, 0);
	glVertex3f(40.0f, SECOND_FLOOR, -HOUSE_Z + 0.05f);
	glTexCoord2f(1, 1);
	glVertex3f(40.0f, SECOND_FLOOR + 15.0f, -HOUSE_Z + 0.05f);
	glEnd();
	glPopMatrix();
	//////////////


	//Z POS WALKWAY DOORS (2 sets for now)
	glPushMatrix();	//first set
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex3f(-30.0f, SECOND_FLOOR + 15.0f, HOUSE_Z - 0.05f);
	glTexCoord2f(0, 0);
	glVertex3f(-30.0f, SECOND_FLOOR, HOUSE_Z - 0.05f);
	glTexCoord2f(1, 0);
	glVertex3f(-20.0f, SECOND_FLOOR, HOUSE_Z - 0.05f);
	glTexCoord2f(1, 1);
	glVertex3f(-20.0f, SECOND_FLOOR + 15.0f, HOUSE_Z - 0.05f);
	glEnd();
	glPopMatrix();
	//////////////
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex3f(-20.0f, SECOND_FLOOR + 15.0f, HOUSE_Z - 0.05f);
	glTexCoord2f(0, 0);
	glVertex3f(-20.0f, SECOND_FLOOR, HOUSE_Z - 0.05f);
	glTexCoord2f(1, 0);
	glVertex3f(-10.0f, SECOND_FLOOR, HOUSE_Z - 0.05f);
	glTexCoord2f(1, 1);
	glVertex3f(-10.0f, SECOND_FLOOR + 15.0f, HOUSE_Z - 0.05f);
	glEnd();
	glPopMatrix();
	////////////// 2nd set
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex3f(20.0f, SECOND_FLOOR + 15.0f, HOUSE_Z - 0.05f);
	glTexCoord2f(0, 0);
	glVertex3f(20.0f, SECOND_FLOOR, HOUSE_Z - 0.05f);
	glTexCoord2f(1, 0);
	glVertex3f(30.0f, SECOND_FLOOR, HOUSE_Z - 0.05f);
	glTexCoord2f(1, 1);
	glVertex3f(30.0f, SECOND_FLOOR + 15.0f, HOUSE_Z - 0.05f);
	glEnd();
	glPopMatrix();
	//////////////
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex3f(30.0f, SECOND_FLOOR + 15.0f, HOUSE_Z - 0.05f);
	glTexCoord2f(0, 0);
	glVertex3f(30.0f, SECOND_FLOOR, HOUSE_Z - 0.05f);
	glTexCoord2f(1, 0);
	glVertex3f(40.0f, SECOND_FLOOR, HOUSE_Z - 0.05f);
	glTexCoord2f(1, 1);
	glVertex3f(40.0f, SECOND_FLOOR + 15.0f, HOUSE_Z - 0.05f);
	glEnd();
	glPopMatrix();
	//////////////


	//ENTRANCE SIDING
	glPushMatrix();
	glTranslatef(65.1f, SECOND_FLOOR + 1.0f, 0.0f);
	glScalef(0.25f, 2.0f, 70.0f);
	hybridcube(DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_STAIR_ROT, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES);
	glPopMatrix();
	//glUseProgram(lightProg);

	glPopAttrib();
	glEndList();
}


void mid_first_floor_list()
{
	glNewList(MID_FIRST_FLOOR, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);

	//glUseProgram(textureProg);
	//FIRST FLOOR WALKWAY Z NEG
	glBindTexture(GL_TEXTURE_2D, tex_ids[WOOD_MID_SIDES]);
	set_material(GL_FRONT, &faux_wood);
	glPushMatrix();
	glTranslatef(20.0f, 2.0f, -42.5f);
	glScalef(90.0f, 4.0f, 15.0f);
	hybridcube(WOOD_MID_SIDES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, WOOD_FLOOR_SIDING, NO_TEXTURES);
	glPopMatrix();
	//FIRST FLOOR WALKWAY Z POS
	glPushMatrix();
	glTranslatef(20.0f, 2.0f, 42.5f);
	glScalef(90.0f, 4.0f, 15.0f);
	hybridcube(WOOD_MID_SIDES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, WOOD_FLOOR_SIDING);
	glPopMatrix();
	//FIRST FLOOR MID ENTRANCE FLOOR
	glBindTexture(GL_TEXTURE_2D, tex_ids[WOOD_MID_ENTRANCE]);
	glPushMatrix();
	glTranslatef(35.0f, 1.25f, 0.0f);
	glScalef(20.0f, 2.5f, 80.0f);
	hybridcube(WOOD_MID_ENTRANCE, NO_TEXTURES, WOOD_FLOOR_SIDING, WOOD_FLOOR_SIDING, NO_TEXTURES, NO_TEXTURES);
	glPopMatrix();
	//FIRST FLOOR MID Z NEG FLOOR
	glBindTexture(GL_TEXTURE_2D, tex_ids[WOOD_MID_SIDES]);
	glPushMatrix();
	glTranslatef(5.0f, 1.25f, -30.0f);
	glScalef(40.0f, 2.5f, 20.0f);
	hybridcube(WOOD_MID_SIDES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, WOOD_FLOOR_SIDING, NO_TEXTURES);
	glPopMatrix();
	//FIRST FLOOR MID Z POS FLOOR
	glPushMatrix();
	glTranslatef(5.0f, 1.25f, 30.0f);
	glScalef(40.0f, 2.5f, 20.0f);
	hybridcube(WOOD_MID_SIDES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, WOOD_FLOOR_SIDING);
	glPopMatrix();
	//FIRST FLOOR MID STAGE FLOOR
	glBindTexture(GL_TEXTURE_2D, tex_ids[STAGE_WOOD]);
	glPushMatrix();
	glTranslatef(-26.0f, 1.25f, 0.0f);
	glScalef(15.0f, 2.5f, 10.0f);
	hybridcube(STAGE_WOOD_ROT, NO_TEXTURES, NO_TEXTURES, STAGE_WOOD_SHORT_SIDES, STAGE_WOOD_SHORT_SIDES, STAGE_WOOD_SHORT_SIDES);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-44.75f, 1.25f, 0.0f);
	glScalef(22.5f, 2.5f, 40.0f);
	hybridcube(STAGE_WOOD, NO_TEXTURES, NO_TEXTURES, STAGE_WOOD_LONG_SIDES, NO_TEXTURES, NO_TEXTURES);
	glPopMatrix();

	//STAGE RAILINGS
	glPushMatrix();
	glTranslatef(-26.0f, 4.0f, 5.0f);
	glScalef(15.0f, 0.2f, 0.2f);
	hybridcube(STAGE_WOOD_PILLAR_ROT, STAGE_WOOD_PILLAR_ROT, NO_TEXTURES, NO_TEXTURES, STAGE_WOOD_PILLAR_ROT, STAGE_WOOD_PILLAR_ROT);
	glPopMatrix();
	//////////////
	glPushMatrix();
	glTranslatef(-26.0f, 4.0f, -5.0f);
	glScalef(15.0f, 0.2f, 0.2f);
	hybridcube(STAGE_WOOD_PILLAR_ROT, STAGE_WOOD_PILLAR_ROT, NO_TEXTURES, NO_TEXTURES, STAGE_WOOD_PILLAR_ROT, STAGE_WOOD_PILLAR_ROT);
	glPopMatrix();
	//////////////
	glPushMatrix();
	glTranslatef(-33.5f, 4.0f, -12.5f);
	glScalef(0.2f, 0.2f, 15.0f);
	hybridcube(STAGE_WOOD_PILLAR, STAGE_WOOD_PILLAR, STAGE_WOOD_PILLAR, STAGE_WOOD_PILLAR, NO_TEXTURES, NO_TEXTURES);
	glPopMatrix();
	/////////////
	glPushMatrix();
	glTranslatef(-33.5f, 4.0f, 12.5f);
	glScalef(0.2f, 0.2f, 15.0f);
	hybridcube(STAGE_WOOD_PILLAR, STAGE_WOOD_PILLAR, STAGE_WOOD_PILLAR, STAGE_WOOD_PILLAR, NO_TEXTURES, NO_TEXTURES);
	glPopMatrix();
	/////////////
	glPushMatrix();
	//glUseProgram(textureProg);
	glBindTexture(GL_TEXTURE_2D, tex_ids[STAGE_WOOD_PLANK]);
	glTranslatef(-33.5f, 3.0f, 20.0f);
	for (int i = -3; i <= 3; i++)
	{
		if (i == 0)
		{
			glPushMatrix();
			glScalef(0.2f, 2.0f, 0.2f);
			texcube();
			glPopMatrix();
			glTranslatef(0.0f, 0.0f, -10.0f);
		}
		else
		{
			glPushMatrix();
			glScalef(0.2f, 2.0f, 0.2f);
			texcube();
			glPopMatrix();
			glTranslatef(0.0f, 0.0f, -5.0f);
		}
	}
	glPushMatrix();
	glScalef(0.2f, 2.0f, 0.2f);
	texcube();
	glPopMatrix();
	glPopMatrix();
	/////////////
	glPushMatrix();
	//STAGE WALKWAY RAILINGS
	glPopMatrix();


	//STAGE STAIRS
	set_material(GL_FRONT_AND_BACK, &light_wood);
	glPushMatrix();	//master push
	glTranslatef(-18.0f, 1.8f, 0.0f);
	glPushMatrix();
	glScalef(1.0f, 0.5f, 9.75f);
	hybridcube(STAGE_WOOD_STAIR, STAGE_WOOD_STAIR, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES);
	glPopMatrix();
	glTranslatef(1.0f, -0.75f, 0.0f);
	glPushMatrix();
	glScalef(1.0f, 0.5f, 9.75f);
	hybridcube(STAGE_WOOD_STAIR, STAGE_WOOD_STAIR, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES);
	glPopMatrix();
	glTranslatef(1.0f, -0.75f, 0.0f);
	glPushMatrix();
	glScalef(1.0f, 0.5f, 9.75f);
	hybridcube(STAGE_WOOD_STAIR, STAGE_WOOD_STAIR, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES);
	glPopMatrix();
	glPopMatrix();	//master pop
	//stage stair siding Z POS
	glPushMatrix();
	glTranslatef(-17.25f, 1.1f, 4.875f);
	glRotatef(-35, 0, 0, 1);
	glScalef(3.75f, 1.0f, 0.25f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, STAGE_WOOD_STAIR_ROT, STAGE_WOOD_STAIR_ROT);
	glPopMatrix();
	//stage stair siding Z NEG
	glPushMatrix();
	glTranslatef(-17.25f, 1.1f, -4.875f);
	glRotatef(-35, 0, 0, 1);
	glScalef(3.75f, 1.0f, 0.25f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, STAGE_WOOD_STAIR_ROT, STAGE_WOOD_STAIR_ROT);
	glPopMatrix();




	//FIRST FLOOR MID TABLES
	///////////////////
	glPushMatrix();
	glTranslatef(35.0f, 2.5f, 20.0f);
	glRotatef(45, 0, 1, 0);
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
	glRotatef(200, 0, 1, 0);
	glCallList(TABLE);
	glPopMatrix();
	//////////////////
	glPushMatrix();
	glTranslatef(12.5f, 2.5f, 30.0f);
	glRotatef(150, 0, 1, 0);
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
	glRotatef(30, 0, 1, 0);
	glCallList(TABLE);
	glPopMatrix();
	///////////////////	
	glPushMatrix();
	glTranslatef(35.0f, 2.5f, -30.0f);
	glRotatef(100, 0, 1, 0);
	glCallList(TABLE);
	glPopMatrix();
	///////////////////	
	glPushMatrix();
	glTranslatef(12.5f, 2.5f, -30.0f);
	glRotatef(15, 0, 1, 0);
	glCallList(TABLE);
	glPopMatrix();
	///////////////////


	//Z NEG PILLARS
	//glUseProgram(textureProg);
	glBindTexture(GL_TEXTURE_2D, tex_ids[DARK_WOOD_PILLAR]);
	glPushMatrix();	//master push
	/////////////////
	glTranslatef(55.25f, 12.0f, -35.25f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	texcube();
	glPopMatrix();
	/////////////////
	glTranslatef(-12.5f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	texcube();
	glPopMatrix();
	////////////////
	glTranslatef(-12.5f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	texcube();
	glPopMatrix();
	////////////////
	glTranslatef(-12.5f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	texcube();
	glPopMatrix();
	////////////////
	glTranslatef(-12.5f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	texcube();
	glPopMatrix();
	////////////////
	glTranslatef(-12.5f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	texcube();
	glPopMatrix();
	////////////////
	glTranslatef(-12.5f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	texcube();
	glPopMatrix();
	////////////////
	glPopMatrix();	//master pop
	//Z POS PILLARS
	glPushMatrix();	//master push
	/////////////////
	glTranslatef(55.25f, 12.0f, 35.25f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	texcube();
	glPopMatrix();
	/////////////////
	glTranslatef(-12.5f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	texcube();
	glPopMatrix();
	////////////////
	glTranslatef(-12.5f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	texcube();
	glPopMatrix();
	////////////////
	glTranslatef(-12.5f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	texcube();
	glPopMatrix();
	////////////////
	glTranslatef(-12.5f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	texcube();
	glPopMatrix();
	////////////////
	glTranslatef(-12.5f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	texcube();
	glPopMatrix();
	////////////////
	glTranslatef(-12.5f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 16.0f, 0.5f);
	texcube();
	glPopMatrix();
	////////////////
	glPopMatrix();	//master pop

	//STAGE PILLARS
	glPushMatrix();
	glTranslatef(-40.25f, 11.0f, -10.0f);
	glScalef(0.5f, 18.0f, 0.5f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, WOOD_PILLAR, WOOD_PILLAR, WOOD_PILLAR, WOOD_PILLAR);
	glPopMatrix();
	//////////////
	glPushMatrix();
	glTranslatef(-40.25f, 11.0f, 10.0f);
	glScalef(0.5f, 18.0f, 0.5f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, WOOD_PILLAR, WOOD_PILLAR, WOOD_PILLAR, WOOD_PILLAR);
	glPopMatrix();

	//glUseProgram(lightProg);

	glPopAttrib();
	glEndList();
}


void stair_list()
{
	glNewList(STAIRS, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);

	//Z NEG STAIRS
	//glUseProgram(textureProg);
	glBindTexture(GL_TEXTURE_2D, tex_ids[WOOD_PILLAR]);
	glPushMatrix();
	glTranslatef(-14.0f, 2.0f, -27.5f);
	for (int i = 0; i < 16; i++)
	{
		//stair 2 - n
		glTranslatef(-1.5f, 1.1f, 0.0f);
		glPushMatrix();
		glScalef(2.0f, 0.5f, 10.0f);
		//colorcube();
		//texcube();
		hybridcube(DARK_WOOD_STAIR, DARK_WOOD_STAIR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, NO_TEXTURES, NO_TEXTURES);
		glPopMatrix();
	}
	glPopMatrix();

	//Z POS STAIRS
	glPushMatrix();
	glTranslatef(-14.0f, 2.0f, 27.5f);
	for (int i = 0; i < 16; i++)
	{
		//stair 2 - n
		glTranslatef(-1.5f, 1.1f, 0.0f);
		glPushMatrix();
		glScalef(2.0f, 0.5f, 10.0f);
		//colorcube();
		//texcube();
		hybridcube(DARK_WOOD_STAIR, DARK_WOOD_STAIR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, NO_TEXTURES, NO_TEXTURES);
		glPopMatrix();
	}
	glPopMatrix();

	//STAIR RAILS
	glBindTexture(GL_TEXTURE_2D, tex_ids[WOOD_STAIR]);
	glPushMatrix();	//master push
	glTranslatef(-27.5f, 11.75f, -22.5f);
	glPushMatrix();
	glRotatef(-35.0f, 0, 0, 1);
	glScalef(32.0f, 2.0f, 0.25f);
	hybridcube(DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_STAIR_ROT, DARK_WOOD_STAIR_ROT);
	glPopMatrix();
	//upper railing
	glPushMatrix();
	glTranslatef(0.5f, 3.0f, 0.0f);
	glRotatef(-35.0f, 0, 0, 1);
	glScalef(32.0f, 0.25f, 0.5f);
	hybridcube(DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT, DARK_WOOD_SMALL, DARK_WOOD_SMALL, DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT);
	glPopMatrix();
	//lower railing
	glPushMatrix();
	glTranslatef(0.5f, 2.0f, 0.0f);
	glRotatef(-35.0f, 0, 0, 1);
	glScalef(32.0f, 0.5f, 0.25f);
	hybridcube(DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT, DARK_WOOD_SMALL, DARK_WOOD_SMALL, DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT);
	glPopMatrix();
	//stair vert bars
	glPushMatrix();
	glTranslatef(-11.5f, 9.25f, 0.125f);
	for (int i = 0; i < 6; i++)
	{
		glPushMatrix();
		glScalef(0.25f, 4.0f, 0.25f);
		hybridcube(DARK_WOOD_SMALL, NO_TEXTURES, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR);
		glPopMatrix();
		glTranslatef(5.0f, -3.5f, 0.0f);
	}
	glPopMatrix();
	//glUseProgram(textureProg);
	///////////////////
	glTranslatef(0.0f, 0.0f, -10.0f);
	glPushMatrix();
	glRotatef(-35.0f, 0, 0, 1);
	glScalef(32.0f, 2.0f, 0.25f);
	hybridcube(DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_STAIR_ROT, DARK_WOOD_STAIR_ROT);
	glPopMatrix();
	//upper railing
	glPushMatrix();
	glTranslatef(0.5f, 3.0f, 0.0f);
	glRotatef(-35.0f, 0, 0, 1);
	glScalef(32.0f, 0.25f, 0.5f);
	hybridcube(DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT, DARK_WOOD_SMALL, DARK_WOOD_SMALL, DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT);
	glPopMatrix();
	//lower railing
	glPushMatrix();
	glTranslatef(0.5f, 2.0f, 0.0f);
	glRotatef(-35.0f, 0, 0, 1);
	glScalef(32.0f, 0.5f, 0.25f);
	hybridcube(DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT, DARK_WOOD_SMALL, DARK_WOOD_SMALL, DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT);
	glPopMatrix();
	//stair vert bars
	glPushMatrix();
	glTranslatef(-11.5f, 9.25f, -0.125f);
	for (int i = 0; i < 6; i++)
	{
		glPushMatrix();
		glScalef(0.25f, 4.0f, 0.25f);
		hybridcube(DARK_WOOD_SMALL, NO_TEXTURES, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR);
		glPopMatrix();
		glTranslatef(5.0f, -3.5f, 0.0f);
	}
	glPopMatrix();
	//glUseProgram(textureProg);
	///////////////////
	glTranslatef(0.0f, 0.0f, 55.0f);
	glPushMatrix();
	glRotatef(-35.0f , 0, 0, 1);
	glScalef(32.0f, 2.0f, 0.25f);
	hybridcube(DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_STAIR_ROT, DARK_WOOD_STAIR_ROT);
	glPopMatrix();
	//upper railing
	glPushMatrix();
	glTranslatef(0.5f, 3.0f, 0.0f);
	glRotatef(-35.0f, 0, 0, 1);
	glScalef(32.0f, 0.25f, 0.5f);
	hybridcube(DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT, DARK_WOOD_SMALL, DARK_WOOD_SMALL, DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT);
	glPopMatrix();
	//lower railing
	glPushMatrix();
	glTranslatef(0.5f, 2.0f, 0.0f);
	glRotatef(-35.0f, 0, 0, 1);
	glScalef(32.0f, 0.5f, 0.25f);
	hybridcube(DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT, DARK_WOOD_SMALL, DARK_WOOD_SMALL, DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT);
	glPopMatrix();
	//stair vert bars
	glPushMatrix();
	glTranslatef(-11.5f, 9.25f, -0.125f);
	for (int i = 0; i < 6; i++)
	{
		glPushMatrix();
		glScalef(0.25f, 4.0f, 0.25f);
		hybridcube(DARK_WOOD_SMALL, NO_TEXTURES, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR);
		glPopMatrix();
		glTranslatef(5.0f, -3.5f, 0.0f);
	}
	glPopMatrix();
	//glUseProgram(textureProg);
	//////////////////
	//siding
	glTranslatef(0.0f, 0.0f, 10.0f);
	glPushMatrix();
	glRotatef(-35.0f, 0, 0, 1);
	glScalef(32.0f, 2.0f, 0.25f);
	hybridcube(DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_STAIR_ROT, DARK_WOOD_STAIR_ROT);
	glPopMatrix();
	//upper railing
	glPushMatrix();
	glTranslatef(0.5f, 3.0f, 0.0f);
	glRotatef(-35.0f, 0, 0, 1);
	glScalef(32.0f, 0.25f, 0.5f);
	hybridcube(DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT, DARK_WOOD_SMALL, DARK_WOOD_SMALL, DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT);
	glPopMatrix();
	//lower railing
	glPushMatrix();
	glTranslatef(0.5f, 2.0f, 0.0f);
	glRotatef(-35.0f, 0, 0, 1);
	glScalef(32.0f, 0.5f, 0.25f);
	hybridcube(DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT, DARK_WOOD_SMALL, DARK_WOOD_SMALL, DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT);
	glPopMatrix();
	//stair vert bars
	glPushMatrix();
	glTranslatef(-11.5f, 9.25f, 0.125f);
	for (int i = 0; i < 6; i++)
	{
		glPushMatrix();
		glScalef(0.25f, 4.0f, 0.25f);
		hybridcube(DARK_WOOD_SMALL, NO_TEXTURES, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR);
		glPopMatrix();
		glTranslatef(5.0f, -3.5f, 0.0f);
	}
	glPopMatrix();
	//glUseProgram(textureProg);
	//////////////
	glPopMatrix();	//master pop
	//glUseProgram(lightProg);

	//UNDER STAIR FLOOR -- Z NEG
	//glUseProgram(textureProg);
	glPushMatrix();
	glTranslatef(-28.0f, 1.25f, -27.5f);
	glScalef(26.0f, 2.5f, 15.0f);
	hybridcube(WOOD_MID_UNDERSTAIR, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, WOOD_FLOOR_SIDING, NO_TEXTURES);
	glPopMatrix();
	//UNDER STAIR FLOOR -- Z POS
	glPushMatrix();
	glTranslatef(-28.0f, 1.25f, 27.5f);
	glScalef(26.0f, 2.5f, 15.0f);
	hybridcube(WOOD_MID_UNDERSTAIR, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, WOOD_FLOOR_SIDING);
	glPopMatrix();

	//UNDER STAIR PILLAR SUPPORTS -- Z NEG
	glPushMatrix();
	glTranslatef(-29.5f, 7.5f, -23.0f);
	glScalef(0.5f, 11.0f, 0.5f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR);
	glPopMatrix();
	///////////////
	glPushMatrix();
	glTranslatef(-29.5f, 7.5f, -32.0f);
	glScalef(0.5f, 11.0f, 0.5f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR);
	glPopMatrix();

	//UNDER STAIR PILLAR SUPPORTS -- Z POS
	glPushMatrix();
	glTranslatef(-29.5f, 7.5f, 23.0f);
	glScalef(0.5f, 11.0f, 0.5f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR);
	glPopMatrix();
	///////////////
	glPushMatrix();
	glTranslatef(-29.5f, 7.5f, 32.0f);
	glScalef(0.5f, 11.0f, 0.5f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR);
	glPopMatrix();

	//glUseProgram(lightProg);

	glPopAttrib();
	glEndList();
}


void entrance_list()
{
	glNewList(ENTRANCE, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);

	//glUseProgram(textureProg);

	//ENTRANCE RAILINGS
	glPushMatrix();
	glTranslatef(65.25f, 7.0f, 20.0f);
	glPushMatrix();
	glScalef(0.4f, 0.2f, 30.0f);
	hybridcube(DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT, DARK_WOOD_SMALL, DARK_WOOD_SMALL);
	glPopMatrix();
	glTranslatef(0.0f, -0.5f, 0.0f);
	glPushMatrix();
	glScalef(0.25f, 0.25f, 30.0f);
	hybridcube(DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT, DARK_WOOD_SMALL, DARK_WOOD_SMALL);
	glPopMatrix();
	glPopMatrix();
	////////////// horizontal railings
	glPushMatrix();
	glTranslatef(65.25f, 7.0f, -20.0f);
	glPushMatrix();
	glScalef(0.4f, 0.2f, 30.0f);
	hybridcube(DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT, DARK_WOOD_SMALL, DARK_WOOD_SMALL);
	glPopMatrix();
	glTranslatef(0.0f, -0.5f, 0.0f);
	glPushMatrix();
	glScalef(0.25f, 0.25f, -30.0f);
	hybridcube(DARK_WOOD_PILLAR, DARK_WOOD_PILLAR, DARK_WOOD_PILLAR_ROT, DARK_WOOD_PILLAR_ROT, DARK_WOOD_SMALL, DARK_WOOD_SMALL);
	glPopMatrix();
	glPopMatrix();
	////////////// vertical railings
	glPushMatrix();
	glTranslatef(65.25f, 4.5f, -35.0f);
	for (int i = -3; i <= 3; i++)
	{
		if (i == 0)
		{
			glTranslatef(0.0f, 0.0f, 10.0f);
		}
		else
		{
			glTranslatef(0.0f, 0.0f, 3.3333f);
			glPushMatrix();
			glScalef(0.2f, 4.0f, 0.2f);
			hybridcube(NO_TEXTURES, NO_TEXTURES, DARK_WOOD_PLANK, DARK_WOOD_PLANK, DARK_WOOD_PLANK, DARK_WOOD_PLANK);
			glPopMatrix();
			glTranslatef(0.0f, 0.0f, 3.3333f);
			glPushMatrix();
			glScalef(0.2f, 4.0f, 0.2f);
			hybridcube(NO_TEXTURES, NO_TEXTURES, DARK_WOOD_PLANK, DARK_WOOD_PLANK, DARK_WOOD_PLANK, DARK_WOOD_PLANK);
			glPopMatrix();
			glTranslatef(0.0f, 0.0f, 3.3333f);
		}
	}
	glPopMatrix();
	////////////// lattice
	//glBindTexture(GL_TEXTURE_2D, tex_ids[WOOD_LATTICE]);
	//glPushMatrix();
	//glBegin(GL_QUADS);
	//glTexCoord2f(0, 0);
	//glVertex3f(65.25f, 6.5f, -35.0f);
	//glTexCoord2f(0, 1);
	//glVertex3f(65.25f, 4.0f, -35.0f);
	//glTexCoord2f(1, 1);
	//glVertex3f(65.25f, 4.0f, -25.0f);
	//glTexCoord2f(1, 0);
	//glVertex3f(65.25f, 6.5f, -25.0f);
	//glEnd();
	//glPopMatrix();


	//Entrance Wood Base
	glPushMatrix();	//master push
	glBindTexture(GL_TEXTURE_2D, tex_ids[WOOD_MID_ENTRANCE]);
	set_material(GL_FRONT, &faux_wood);
	glTranslatef(70.0f, 2.0f, 0.0f);
	glPushMatrix();
	glScalef(10.0f, 4.0f, 100.0f);
	hybridcube(WOOD_MID_ENTRANCE, NO_TEXTURES, WOOD_FLOOR_SIDING, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES);
	glPopMatrix();
	//walkway
	glBindTexture(GL_TEXTURE_2D, tex_ids[WOOD_MID_SIDES]);
	glTranslatef(-15.0f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(20.0f, 4.0f, 10.0f);
	hybridcube(WOOD_MID_SIDES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, WOOD_FLOOR_SIDING_PLANK, WOOD_FLOOR_SIDING_PLANK);
	glPopMatrix();
	//railing
	//glUseProgram(textureProg);
	set_material(GL_FRONT, &faux_wood);
	glTranslatef(0, 4.0f, -5.0f);
	glPushMatrix();
	glScalef(19.5f, 1.0f, 0.25f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, WOOD_PLANK_ROT, WOOD_PLANK_ROT);
	glPopMatrix();
	//railing
	glTranslatef(0.0f, 2.0f, 0.0f);
	glPushMatrix();
	glScalef(19.5f, 1.0f, 0.25f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, WOOD_PLANK_ROT, WOOD_PLANK_ROT);
	glPopMatrix();
	//railing
	glTranslatef(0.0f, -2.0f, 10.0f);
	glPushMatrix();
	glScalef(19.5f, 1.0f, 0.25f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, WOOD_PLANK_ROT, WOOD_PLANK_ROT);
	glPopMatrix();
	//railing
	glTranslatef(0.0f, 2.0f, 0.0f);
	glPushMatrix();
	glScalef(19.5f, 1.0f, 0.25f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, NO_TEXTURES, WOOD_PLANK_ROT, WOOD_PLANK_ROT);
	glPopMatrix();
	//glUseProgram(textureProg);
	// back bannister 1
	glTranslatef(10.0f, 1.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 10.0f, 0.5f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, WOOD_PILLAR, WOOD_PILLAR, WOOD_PILLAR, WOOD_PILLAR);
	glPopMatrix();
	//back bannister 2
	glTranslatef(0.0f, 0.0f, -10.0f);
	glPushMatrix();
	glScalef(0.5f, 10.0f, 0.5f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, WOOD_PILLAR, WOOD_PILLAR, WOOD_PILLAR, WOOD_PILLAR);
	glPopMatrix();

	//entrance small stairs
	glTranslatef(-20.5f, -5.75f, 5.0f);
	glPushMatrix();
	glScalef(1.0f, 1.5f, 10.0f);
	hybridcube(WOOD_PLANK, NO_TEXTURES, WOOD_PLANK_ROT, NO_TEXTURES, WOOD_SMALL, WOOD_SMALL);
	glPopMatrix();
	glTranslatef(-1.0f, -0.25f, 0.0f);
	glPushMatrix();
	glScalef(1.0f, 1.0f, 10.0f);
	hybridcube(WOOD_PLANK, NO_TEXTURES, WOOD_PLANK_ROT, NO_TEXTURES, WOOD_SMALL, WOOD_SMALL);
	glPopMatrix();
	glTranslatef(-1.0f, -0.25f, 0.0f);
	glPushMatrix();
	glScalef(1.0f, 0.5f, 10.0f);
	hybridcube(WOOD_PLANK, NO_TEXTURES, WOOD_PLANK_ROT, NO_TEXTURES, WOOD_SMALL, WOOD_SMALL);
	glPopMatrix();

	//mid bannister 1
	glTranslatef(2.75f, 6.0f, 5.0f);
	glPushMatrix();
	glScalef(0.5f, 10.0f, 0.5f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, WOOD_PILLAR, WOOD_PILLAR, WOOD_PILLAR, WOOD_PILLAR);
	glPopMatrix();
	//mid bannister 2
	glTranslatef(0.0f, 0.0f, -10.0f);
	glPushMatrix();
	glScalef(0.5f, 10.0f, 0.5f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, WOOD_PILLAR, WOOD_PILLAR, WOOD_PILLAR, WOOD_PILLAR);
	glPopMatrix();
	//front bannister 1
	glTranslatef(-3.0f, -1.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 12.0f, 0.5f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, WOOD_PILLAR, WOOD_PILLAR, WOOD_PILLAR, WOOD_PILLAR);
	glPopMatrix();
	//front bannister 2
	glTranslatef(0.0f, 0.0f, 10.0f);
	glPushMatrix();
	glScalef(0.5f, 12.0f, 0.5f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, WOOD_PILLAR, WOOD_PILLAR, WOOD_PILLAR, WOOD_PILLAR);
	glPopMatrix();

	//ARCHWAY -- support beam 1
	glBindTexture(GL_TEXTURE_2D, tex_ids[WOOD_PILLAR_ROT]);
	glTranslatef(11.0f, 6.0f, 0.0f);
	glPushMatrix();
	glScalef(23.0f, 0.5f, 0.5f);
	texcube();
	glPopMatrix();
	//////////// central support beam
	glTranslatef(0.0f, 3.5f, -5.0f);
	glPushMatrix();
	glRotatef(45.0f, 1, 0, 0);
	glScalef(24.0f, 0.5f, 0.5f);
	texcube();
	glPopMatrix();
	glTranslatef(0.0f, -3.5f, 5.0f);
	//support beam 2
	glTranslatef(0.0f, 0.0f, -10.0f);
	glPushMatrix();
	glScalef(23.0f, 0.5f, 0.5f);
	texcube();
	glPopMatrix();
	//ARCHWAY ROOF
	//glUseProgram(textureProg);
	glTranslatef(-13.0f, 0.0f, 5.0f);
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, tex_ids[TILE]);
	glBegin(GL_QUADS);
	glTexCoord3f(0.0f, -1.0f, -5.0f);
	glVertex3f(0.0f, -1.0f, -7.5f);
	glTexCoord3f(20.0f, -1.0f, -5.0f);
	glVertex3f(25.0f, -1.0f, -7.5f);
	glTexCoord3f(20.0f, 4.0f, 0.0f);
	glVertex3f(25.0f, 4.0f, 0.0f);
	glTexCoord3f(0.0f, 4.0f, 0.0f);
	glVertex3f(0.0f, 4.0f, 0.0f);
	glEnd();
	glBegin(GL_QUADS);
	glTexCoord3f(0.0f, 4.0f, 0.0f);
	glVertex3f(0.0f, 4.0f, 0.0f);
	glTexCoord3f(20.0f, 4.0f, 0.0f);
	glVertex3f(25.0f, 4.0f, 0.0f);
	glTexCoord3f(20.0f, -1.0f, 5.0f);
	glVertex3f(25.0f, -1.0f, 7.5f);
	glTexCoord3f(0.0f, -1.0f, 5.0f);
	glVertex3f(0.0f, -1.0f, 7.5f);
	glEnd();
	glPopMatrix();

	glTranslatef(1.0f, 1.1f, -3.75f);
	glPushMatrix();
	glRotatef(-33.5f, 1, 0, 0);
	glScalef(0.5f, 0.5f, 9.0f);
	hybridcube(WOOD_PILLAR, WOOD_PILLAR, WOOD_PILLAR_ROT, WOOD_PILLAR_ROT, WOOD_SMALL, WOOD_SMALL);
	glPopMatrix();
	//////////////
	glTranslatef(0.0f, 0.0f, 7.5f);
	glPushMatrix();
	glRotatef(33.5f, 1, 0, 0);
	glScalef(0.5f, 0.5f, 9.0f);
	hybridcube(WOOD_PILLAR, WOOD_PILLAR, WOOD_PILLAR_ROT, WOOD_PILLAR_ROT, WOOD_SMALL, WOOD_SMALL);
	glPopMatrix();
	/////////////
	glTranslatef(23.5f, 0.0f, 0.0f);
	glPushMatrix();
	glRotatef(33.5f, 1, 0, 0);
	glScalef(0.5f, 0.5f, 9.0f);
	hybridcube(WOOD_PILLAR, WOOD_PILLAR, WOOD_PILLAR_ROT, WOOD_PILLAR_ROT, WOOD_SMALL, WOOD_SMALL);
	glPopMatrix();
	////////////
	glTranslatef(0.0f, 0.0f, -7.5f);
	glPushMatrix();
	glRotatef(-33.5f, 1, 0, 0);
	glScalef(0.5f, 0.5f, 9.0f);
	hybridcube(WOOD_PILLAR, WOOD_PILLAR, WOOD_PILLAR_ROT, WOOD_PILLAR_ROT, WOOD_SMALL, WOOD_SMALL);
	glPopMatrix();


	glPopMatrix();	//master pop

	//PILLARS -- Z NEG
	glBindTexture(GL_TEXTURE_2D, tex_ids[DARK_WOOD_PILLAR]);
	glPushMatrix();	//master push
	glTranslatef(65.25f, 22.0f, -15.25f);
	glPushMatrix();
		glScalef(0.5f, 36.0f, 0.5f);
		texcube();
	glPopMatrix();
	/////////////////
	glTranslatef(0.0f, 0.0f, -10.0f);
	glPushMatrix();
		glScalef(0.5f, 36.0f, 0.5f);
		texcube();
	glPopMatrix();
	/////////////////
	glTranslatef(0.0f, 0.0f, -10.0f);
	glPushMatrix();
	glScalef(0.5f, 36.0f, 0.5f);
	texcube();
	glPopMatrix();
	//PILLARS -- Z POS
	glTranslatef(0.0f, 0.0f, 50.5f);
	glPushMatrix();
	glScalef(0.5f, 36.0f, 0.5f);
	texcube();
	glPopMatrix();
	////////////////
	glTranslatef(0.0f, 0.0f, 10.0f);
	glPushMatrix();
	glScalef(0.5f, 36.0f, 0.5f);
	texcube();
	glPopMatrix();
	///////////////
	glTranslatef(0.0f, 0.0f, 10.0f);
	glPushMatrix();
	glScalef(0.5f, 36.0f, 0.5f);
	texcube();
	glPopMatrix();
	///////////////
	glPopMatrix();	//master pop


	//ENTRANCE TABLES
	glPushMatrix();
	glTranslatef(70.0f, 4.0f, -10.0f);
	glRotatef(-45, 0, 1, 0);
	glCallList(TABLE);
	glPopMatrix();
	////////////////////
	glPushMatrix();
	glTranslatef(70.0f, 4.0f, -20.0f);
	glRotatef(-90, 0, 1, 0);
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
	glRotatef(45, 0, 1, 0);
	glCallList(TABLE);
	glPopMatrix();
	////////////////////
	glPushMatrix();
	glTranslatef(70.0f, 4.0f, 20.0f);
	glRotatef(-90, 0, 1, 0);
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


void table_list()
{
	glNewList(TABLE, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);

	////glUseProgram(lightProg);

	//top
	glPushMatrix();
	glTranslatef(0.0f, 2.625f, 0.0f);
	glScalef(2.25f, 0.25f, 2.25f);
	hybridcube(TABLE_WOOD, TABLE_WOOD, TABLE_WOOD_LEG, TABLE_WOOD_LEG, TABLE_WOOD_LEG, TABLE_WOOD_LEG);
	glPopMatrix();
	//tablecloth
	//glUseProgram(textureProg);
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, tex_ids[TABLECLOTH]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(1.126f, 2.76f, 0.5f);
	glTexCoord2f(0, 1);
	glVertex3f(1.126f, 2.76f, -0.5f); 
	glTexCoord2f(2, 1);
	glVertex3f(-1.126f, 2.76f, -0.5f);
	glTexCoord2f(2, 0);
	glVertex3f(-1.126f, 2.76f, 0.5f);
	glEnd();
	glPopMatrix();
	//////////////
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(1.126f, 2.76f, 0.5f);
	glTexCoord2f(0, 1);
	glVertex3f(1.126f, 2.76f, -0.5f);
	glTexCoord2f(1, 1);
	glVertex3f(1.126f, 2.0f, -0.5f);
	glTexCoord2f(1, 0);
	glVertex3f(1.126f, 2.0f, 0.5f);
	glEnd();
	glPopMatrix();
	//////////////
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-1.126f, 2.76f, 0.5f);
	glTexCoord2f(0, 1);
	glVertex3f(-1.126f, 2.76f, -0.5f);
	glTexCoord2f(1, 1);
	glVertex3f(-1.126f, 2.0f, -0.5f);
	glTexCoord2f(1, 0);
	glVertex3f(-1.126f, 2.0f, 0.5f);
	glEnd();
	glPopMatrix();

	//LANTERN
	glPushMatrix();
	set_material(GL_FRONT, &yellow_rubber);
	glTranslatef(0.2f, 3.25f, -0.5f);
	glRotatef(30, 0, 1, 0);
	glScalef(0.5f, 1.0f, 0.5f);
	hybridcube(NO_TEXTURES, NO_TEXTURES, CHAND_PAPER, CHAND_PAPER, CHAND_PAPER, CHAND_PAPER);
	glPopMatrix();


	//leg 1
	glBindTexture(GL_TEXTURE_2D, tex_ids[TABLE_WOOD_LEG]);
	glPushMatrix();
	glTranslatef(1.0f, 1.25f, 1.0f);
	glScalef(0.2f, 2.5f, 0.2f);
	texcube();
	glPopMatrix();
	//leg 2
	glPushMatrix();
	glTranslatef(-1.0f, 1.25f, 1.0f);
	glScalef(0.2f, 2.5f, 0.2f);
	texcube();
	glPopMatrix();
	//leg 3
	glPushMatrix();
	glTranslatef(1.0f, 1.25f, -1.0f);
	glScalef(0.2f, 2.5f, 0.2f);
	texcube();
	glPopMatrix();
	//leg4
	glPushMatrix();
	glTranslatef(-1.0f, 1.25f, -1.0f);
	glScalef(0.2f, 2.5f, 0.2f);
	texcube();
	glPopMatrix();

	//SODA CAN
	//glUseProgram(textureProg);	//go to the texture program
	glPushMatrix();
	glTranslatef(0.4f, 3.3f, 0.5f);
	//////
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	glBindTexture(GL_TEXTURE_2D, tex_ids[DR_PEPPER]);
	gluCylinder(soda_can, 0.15f, 0.15f, 0.5f, 20, 20);
	glPopMatrix();
	//////
	glPushMatrix();
	glTranslatef(0.0f, -0.5f, 0.0f);
	glRotatef(90, 1, 0, 0);
	glBindTexture(GL_TEXTURE_2D, tex_ids[SILVER_METAL]);
	gluCylinder(soda_can, 0.15, 0.125, 0.05, 20, 20);
	glPopMatrix();
	//////
	glPushMatrix();
	glTranslatef(0.0f, 0.05f, 0.0f);
	glRotatef(90, 1, 0, 0);
	glBindTexture(GL_TEXTURE_2D, tex_ids[SILVER_METAL]);
	gluCylinder(soda_can, 0.125, 0.15, 0.05, 20, 20);
	glPopMatrix();
	//////
	glPushMatrix();
	glTranslatef(0.0f, 0.05f, 0.0f);
	glRotatef(90, 1, 0, 0);
	glBindTexture(GL_TEXTURE_2D, tex_ids[LID]);
	gluDisk(soda_can, 0.0f, 0.125f, 20, 20);
	glPopMatrix();
	glPopMatrix();
	//glUseProgram(lightProg);	//switch back to the shader program

	//FRUIT BOWL
	set_material(GL_FRONT_AND_BACK, &white_rubber);
	glPushMatrix();
	glTranslatef(-0.4f, 2.75f, 0.0f);
	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	gluCylinder(bowl, 0.25f, 0.4f, 0.25f, 50, 50);
	glPopMatrix();
	////////////////
	glTranslatef(0.0f, 0.02f, 0.0f);
	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	gluDisk(bowl, 0.0f, 0.26f, 50, 50);
	glPopMatrix();
	////////////////
	glPopMatrix();

	//SPHERICAL FRUIT
	//glUseProgram(textureProg);
	glBindTexture(GL_TEXTURE_2D, tex_ids[ORANGE]);
	glPushMatrix();
	glTranslatef(-0.375f, 2.85f, 0.0f);
	gluSphere(quadric, 0.11f, 10, 10);
	glTranslatef(0.175f, 0.075f, 0.0f);
	gluSphere(quadric, 0.11, 10, 10);
	glTranslatef(-0.35f, 0.0f, 0.0f);
	gluSphere(quadric, 0.11, 10, 10);
	glTranslatef(0.175f, 0.0f, 0.175f);
	gluSphere(quadric, 0.13, 10, 10);
	glTranslatef(0.0f, 0.0f, -0.375f);
	gluSphere(quadric, 0.12, 10, 10);
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

	//glUseProgram(textureProg);
	glBindTexture(GL_TEXTURE_2D, tex_ids[TABLE_WOOD]);

	//seat
	glPushMatrix();
	glTranslatef(0.0f, 1.75f, 0.0f);
	glScalef(1.25f, 0.2f, 1.25f);
	hybridcube(TABLE_WOOD, TABLE_WOOD, TABLE_WOOD_LEG, TABLE_WOOD_LEG, TABLE_WOOD_LEG, TABLE_WOOD_LEG);
	glPopMatrix();
	//////////////
	//new back -- top piece
	glPushMatrix();
	glTranslatef(0.55f, 3.35f, 0.0f);
	glScalef(0.15, 0.25, 1.0f);
	texcube();
	glPopMatrix();
	//////////// side piece
	glPushMatrix();
	glTranslatef(0.55f, 2.625f, -0.55f);
	glScalef(0.15f, 1.75f, 0.15f);
	texcube();
	glPopMatrix();
	///////////// side piece
	glPushMatrix();
	glTranslatef(0.55f, 2.625f, 0.55f);
	glScalef(0.15f, 1.75f, 0.15f);
	texcube();
	glPopMatrix();
	///////////// mid piece
	glPushMatrix();
	glTranslatef(0.55f, 2.75f, 0.0f);
	glScalef(0.15f, 0.5f, 0.5f);
	texcube();
	glPopMatrix();
	///////////// small corner piece
	glPushMatrix();
	glTranslatef(0.55f, 3.125f, 0.375f);
	glScalef(0.15f, 0.25f, 0.25f);
	texcube();
	glPopMatrix();
	///////////// small corner piece
	glPushMatrix();
	glTranslatef(0.55f, 3.125f, -0.375f);
	glScalef(0.15f, 0.25f, 0.25f);
	texcube();
	glPopMatrix();
	///////////// small corner piece
	glPushMatrix();
	glTranslatef(0.55f, 2.375f, 0.375f);
	glScalef(0.15f, 0.25f, 0.25f);
	texcube();
	glPopMatrix();
	///////////// small corner piece
	glPushMatrix();
	glTranslatef(0.55f, 2.375f, -0.375f);
	glScalef(0.15f, 0.25f, 0.25f);
	texcube();
	glPopMatrix();
	////////////// lower back piece
	glPushMatrix();
	glTranslatef(0.55f, 2.05f, 0.0f);
	glScalef(0.15f, 0.4f, 1.0f);
	texcube();
	glPopMatrix();

	//new legs
	glPushMatrix();
	glTranslatef(0.55f, 0.875f, 0.55f);
	glScalef(0.15f, 1.75f, 0.15f);
	texcube();
	glPopMatrix();
	/////////////
	glPushMatrix();
	glTranslatef(0.55f, 0.875f, -0.55f);
	glScalef(0.15f, 1.75f, 0.15f);
	texcube();
	glPopMatrix();
	//////////////
	glPushMatrix();
	glTranslatef(-0.55f, 0.875f, -0.55f);
	glScalef(0.15f, 1.75f, 0.15f);
	texcube();
	glPopMatrix();
	/////////////
	glPushMatrix();
	glTranslatef(-0.55f, 0.875f, 0.55f);
	glScalef(0.15f, 1.75f, 0.15f);
	texcube();
	glPopMatrix();
	///////////////



	//cushion
	glBindTexture(GL_TEXTURE_2D, tex_ids[RED_FELT]);
	//set_material(GL_FRONT_AND_BACK, &red_rubber);
	glPushMatrix();
	glTranslatef(0.0f, 1.825f, 0.0f);
	glScalef(1.0f, 0.25f, 1.0f);
	gluSphere(quadric, 0.625f, 10, 4);
	glPopMatrix();
	/////////
	glPushMatrix();
	glTranslatef(0.54f, 2.75f, 0.0f);
	glScalef(0.15f, 0.4f, 0.4f);
	texcube();
	glPopMatrix();

	//glUseProgram(lightProg);

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
	glTexCoord2f(0, 0);
	glNormal3fv(vnorm[0]);
	glVertex3f(HOUSE_X, HOUSE_Y, HOUSE_Z);
	glTexCoord2f(0, 20);
	glNormal3fv(vnorm[1]);
	glVertex3f(HOUSE_X, HOUSE_Y, -HOUSE_Z);
	glTexCoord2f(30, 20);
	glNormal3fv(vnorm[2]);
	glVertex3f(-HOUSE_X, HOUSE_Y, -HOUSE_Z);
	glTexCoord2f(30, 0);
	glNormal3fv(vnorm[3]);
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
			texcube();
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
			texcube();
		glPopMatrix();
		glTranslatef(-5.0f, 0.0f, 0.0f);
	}
	glPopMatrix();

	//chandeliers
	glPushMatrix();
	glTranslatef(40.0f, 20.0f, 20.0f);
	draw_chandelier();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(40.0f, 20.0f, -20.0f);
	draw_chandelier();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(10.0f, 20.0f, 0.0f);
	draw_chandelier();
	glPopMatrix();


	glPopAttrib();
	glEndList();
}


void draw_chandelier()
{
	//CHANDELIER
	//glUseProgram(textureProg);
	set_material(GL_FRONT_AND_BACK, &clear);
	glBindTexture(GL_TEXTURE_2D, tex_ids[CHAND_PAPER]);
	glPushMatrix();
	glTranslatef(0.0f, 10.0f, 0.0f);
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	gluCylinder(quadric, 1.0f, 1.0f, 8.0f, 8, 100);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(2.0f, -2.0f, 0.0f);
	glRotatef(90, 1, 0, 0);
	gluCylinder(quadric, 1.0f, 1.0f, 4.0f, 8, 100);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-2.0f, -2.0f, 0.0f);
	glRotatef(90, 1, 0, 0);
	gluCylinder(quadric, 1.0f, 1.0f, 4.0f, 8, 100);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.0f, -2.0f, 2.0f);
	glRotatef(90, 1, 0, 0);
	gluCylinder(quadric, 1.0f, 1.0f, 4.0f, 8, 100);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.0f, -2.0f, -2.0f);
	glRotatef(90, 1, 0, 0);
	gluCylinder(quadric, 1.0f, 1.0f, 4.0f, 8, 100);
	glPopMatrix();
	glPopMatrix();
	//glUseProgram(lightProg);

	set_material(GL_FRONT_AND_BACK, &black_plastic);
	glPushMatrix();
	glTranslatef(0.0f, 20.0f, 0.0f);
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	gluCylinder(quadric, 0.05f, 0.05f, 15.0f, 100, 100);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(2.0f, 0.0f, 0.0f);
	glRotatef(90, 1, 0, 0);
	gluCylinder(quadric, 0.05f, 0.05f, 15.0f, 100, 100);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-2.0f, 0.0f, 0.0f);
	glRotatef(90, 1, 0, 0);
	gluCylinder(quadric, 0.05f, 0.05f, 15.0f, 100, 100);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 2.0f);
	glRotatef(90, 1, 0, 0);
	gluCylinder(quadric, 0.05f, 0.05f, 15.0f, 100, 100);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -2.0f);
	glRotatef(90, 1, 0, 0);
	gluCylinder(quadric, 0.05f, 0.05f, 15.0f, 100, 100);
	glPopMatrix();
	glPopMatrix();
}


void draw_fan()
{
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

	// fan support
	glPushMatrix();
	glTranslatef(0.0f, 14.0f, 0.0f);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(quadric, 0.15f, 0.15f, 0.75f, 20, 20);
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
	glTranslatef(0.0f, 14.0f, 0.0f);
	glScalef(0.75f, 0.5f, 0.75f);
	gluSphere(fan_light, 0.5f, 20, 20);
	glPopMatrix();
#endif
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

	//MID FLOOR -- ENTRANCE
	//glUseProgram(textureProg);
	glBindTexture(GL_TEXTURE_2D, tex_ids[FLOOR_TILE]);
	glPushMatrix();
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(25.0f, 0.0f, 20.0f);
		glTexCoord2f(0, 4);
		glVertex3f(25.0f, 0.0f, -20.0f);
		glTexCoord2f(1, 4);
		glVertex3f(17.5f, 0.0f, -20.0f);
		glTexCoord2f(1, 0);
		glVertex3f(17.5f, 0.0f, 20.0f);
		glEnd();
	glPopMatrix();
	//MID FLOOR -- Z POS
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(17.5f, 0.0f, 20.0f);
	glTexCoord2f(0, 3);
	glVertex3f(-7.5f, 0.0f, 20.0f);
	glTexCoord2f(1, 3);
	glVertex3f(-7.5f, 0.0f, 12.5f);
	glTexCoord2f(1, 0);
	glVertex3f(17.5f, 0.0f, 12.5f);
	glEnd();
	glPopMatrix();
	//MID FLOOR -- Z NEG
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(17.5f, 0.0f, -20.0f);
	glTexCoord2f(0, 3);
	glVertex3f(-7.5f, 0.0f, -20.0f);
	glTexCoord2f(1, 3);
	glVertex3f(-7.5f, 0.0f, -12.5f);
	glTexCoord2f(1, 0);
	glVertex3f(17.5f, 0.0f, -12.5f);
	glEnd();
	glPopMatrix();
	//MID FLOOR -- BALCONY
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-7.5f, 0.0f, -20.0f);
	glTexCoord2f(0, 4);
	glVertex3f(-7.5f, 0.0f, 20.0f);
	glTexCoord2f(2, 4);
	glVertex3f(-21.0f, 0.0f, 20.0f);
	glTexCoord2f(2, 0);
	glVertex3f(-21.0f, 0.0f, -20.0f);
	glEnd();
	glPopMatrix();
	//balcony pool area
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-21.0f, 0.0f, -7.0f);
	glTexCoord2f(0, 2);
	glVertex3f(-21.0f, 0.0f, 7.0f);
	glTexCoord2f(2, 2);
	glVertex3f(-31.0f, 0.0f, 7.0f);
	glTexCoord2f(2, 0);
	glVertex3f(-31.0f, 0.0f, -7.0f);
	glEnd();
	glPopMatrix();
	//////////
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-31.0f, 0.0f, -20.0f);
	glTexCoord2f(0, 4);
	glVertex3f(-31.0f, 0.0f, 20.0f);
	glTexCoord2f(0.5, 4);
	glVertex3f(-33.5f, 0.0f, 20.0f);
	glTexCoord2f(0.5, 0);
	glVertex3f(-33.5f, 0.0f, -20.0f);
	glEnd();
	glPopMatrix();
	///////////
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-31.0f, 0.0f, -20.0f);
	glTexCoord2f(0, 0.5);
	glVertex3f(-31.0f, 0.0f, -17.0f);
	glTexCoord2f(2, 0.5);
	glVertex3f(-21.0f, 0.0f, -17.0f);
	glTexCoord2f(2, 0);
	glVertex3f(-21.0f, 0.0f, -20.0f);
	glEnd();
	glPopMatrix();
	///////////
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-31.0f, 0.0f, 20.0f);
	glTexCoord2f(0, 0.5);
	glVertex3f(-31.0f, 0.0f, 17.0f);
	glTexCoord2f(2, 0.5);
	glVertex3f(-21.0f, 0.0f, 17.0f);
	glTexCoord2f(2, 0);
	glVertex3f(-21.0f, 0.0f, 20.0f);
	glEnd();
	glPopMatrix();



	//POOL BORDERS -- Z POS
	glBindTexture(GL_TEXTURE_2D, tex_ids[FLOOR_TILE]);
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-21.0f, -2.0f, 7.0f);
	glTexCoord2f(0, 1);
	glVertex3f(-21.0f, -2.0f, 17.0f);
	glTexCoord2f(0.5, 1);
	glVertex3f(-21.0f, 0.0f, 17.0f);
	glTexCoord2f(0.5, 0);
	glVertex3f(-21.0f, 0.0f, 7.0f);
	glEnd();
	glPopMatrix();
	//////////////
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-21.0f, -2.0f, 17.0f);
	glTexCoord2f(0, 0.5);
	glVertex3f(-21.0f, 0.0f, 17.0f);
	glTexCoord2f(1, 0.5);
	glVertex3f(-31.0f, 0.0f, 17.0f);
	glTexCoord2f(1, 0);
	glVertex3f(-31.0f, -2.0f, 17.0f);
	glEnd();
	glPopMatrix();
	/////////////
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-31.0f, -2.0f, 7.0f);
	glTexCoord2f(0, 1);
	glVertex3f(-31.0f, -2.0f, 17.0f);
	glTexCoord2f(0.5, 1);
	glVertex3f(-31.0f, 0.0f, 17.0f);
	glTexCoord2f(0.5, 0);
	glVertex3f(-31.0f, 0.0f, 7.0f);
	glEnd();
	glPopMatrix();
	//////////////
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-21.0f, -2.0f, 7.0f);
	glTexCoord2f(0, 0.5);
	glVertex3f(-21.0f, 0.0f, 7.0f);
	glTexCoord2f(1, 0.5);
	glVertex3f(-31.0f, 0.0f, 7.0f);
	glTexCoord2f(1, 0);
	glVertex3f(-31.0f, -2.0f, 7.0f);
	glEnd();
	glPopMatrix();
	//////////////

	//POOL BORDERS -- Z NEG
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-21.0f, -2.0f, -7.0f);
	glTexCoord2f(0, 1);
	glVertex3f(-21.0f, -2.0f, -17.0f);
	glTexCoord2f(0.5, 1);
	glVertex3f(-21.0f, 0.0f, -17.0f);
	glTexCoord2f(0.5, 0);
	glVertex3f(-21.0f, 0.0f, -7.0f);
	glEnd();
	glPopMatrix();
	//////////////	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-21.0f, -2.0f, -17.0f);
	glTexCoord2f(0, 0.5);
	glVertex3f(-21.0f, 0.0f, -17.0f);
	glTexCoord2f(1, 0.5);
	glVertex3f(-31.0f, 0.0f, -17.0f);
	glTexCoord2f(1, 0);
	glVertex3f(-31.0f, -2.0f, -17.0f);
	glEnd();
	glPopMatrix();
	/////////////
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-31.0f, -2.0f, -7.0f);
	glTexCoord2f(0, 1);
	glVertex3f(-31.0f, -2.0f, -17.0f);
	glTexCoord2f(0.5, 1);
	glVertex3f(-31.0f, 0.0f, -17.0f);
	glTexCoord2f(0.5, 0);
	glVertex3f(-31.0f, 0.0f, -7.0f);
	glEnd();
	glPopMatrix();
	//////////////
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-21.0f, -2.0f, -7.0f);
	glTexCoord2f(0, 0.5);
	glVertex3f(-21.0f, 0.0f, -7.0f);
	glTexCoord2f(1, 0.5);
	glVertex3f(-31.0f, 0.0f, -7.0f);
	glTexCoord2f(1, 0);
	glVertex3f(-31.0f, -2.0f, -7.0f);
	glEnd();
	glPopMatrix();
	//////////////
	
	//FIRST FLOOR Z POS POOL
	//glUseProgram(textureProg);
	glBindTexture(GL_TEXTURE_2D, tex_ids[POOL]);
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-31.0f, -2.0f, 7.0f);
	glTexCoord2f(0, 1);
	glVertex3f(-31.0f, -2.0f, 17.0f);
	glTexCoord2f(1, 1);
	glVertex3f(-21.0f, -2.0f, 17.0f);
	glTexCoord2f(1, 0);
	glVertex3f(-21.0f, -2.0f, 7.0f);
	glEnd();
	glPopMatrix();
	//FIRST FLOOR Z NEG POOL
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-31.0f, -2.0f, -7.0f);
	glTexCoord2f(0, 1);
	glVertex3f(-31.0f, -2.0f, -17.0f);
	glTexCoord2f(1, 1);
	glVertex3f(-21.0f, -2.0f, -17.0f);
	glTexCoord2f(1, 0);
	glVertex3f(-21.0f, -2.0f, -7.0f);
	glEnd();
	glPopMatrix();



	//MID FLOOR -- WOOD INSET (ENTRANCE)
	glBindTexture(GL_TEXTURE_2D, tex_ids[STAGE_WOOD_NARROW]);
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(15.0f, 0.0f, 10.0f);
	glTexCoord2f(0, 1);
	glVertex3f(15.0f, 0.0f, -10.0f);
	glTexCoord2f(1, 1);
	glVertex3f(17.5f, 0.0f, -12.5f);
	glTexCoord2f(1, 0);
	glVertex3f(17.5f, 0.0f, 12.5f);
	glEnd();
	glPopMatrix();
	//MID FLOOR -- WOOD INSET (Z POS)
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(17.5f, 0.0f, 12.5f);
	glTexCoord2f(0, 1);
	glVertex3f(-7.5f, 0.0f, 12.5f);
	glTexCoord2f(1, 1);
	glVertex3f(-5.0f, 0.0f, 10.0f);
	glTexCoord2f(1, 0);
	glVertex3f(15.0f, 0.0f, 10.0f);
	glEnd();
	glPopMatrix();
	//MID FLOOR -- WOOD INSET (Z NEG)
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(17.5f, 0.0f, -12.5f);
	glTexCoord2f(0, 1);
	glVertex3f(-7.5f, 0.0f, -12.5f);
	glTexCoord2f(1, 1);
	glVertex3f(-5.0f, 0.0f, -10.0f);
	glTexCoord2f(1, 0);
	glVertex3f(15.0f, 0.0f, -10.0f);
	glEnd();
	glPopMatrix();
	//MID FLOOR -- WOOD INSET (BALCONY)
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-5.0f, 0.0f, -10.0f);
	glTexCoord2f(0, 1);
	glVertex3f(-5.0f, 0.0f, 10.0f);
	glTexCoord2f(1, 1);
	glVertex3f(-7.5f, 0.0f, 12.5f);
	glTexCoord2f(1, 0);
	glVertex3f(-7.5f, 0.0f, -12.5f);
	glEnd();
	glPopMatrix();




	//Z NEG PATIO FLOOR
	glBindTexture(GL_TEXTURE_2D, tex_ids[FLOOR_TILE]);
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



	//UNDER FLOOR
	//glUseProgram(textureProg);
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, tex_ids[SAND]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-5.0f, -2.5f, 10.0f);
	glTexCoord2f(0, 1);
	glVertex3f(-5.0f, -2.5f, -10.0f);
	glTexCoord2f(1, 1);
	glVertex3f(15.0f, -2.5f, -10.0f);
	glTexCoord2f(1, 0);
	glVertex3f(15.0f, -2.5f, 10.0f);
	glEnd();
	glPopMatrix();

	//UNDER FLOOR SIDING -- ENTRANCE
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, tex_ids[LIGHT_PANEL]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(15.0f, 0.0f, 10.0f);
	glTexCoord2f(0, 3);
	glVertex3f(15.0f, 0.0f, -10.0f);
	glTexCoord2f(1, 3);
	glVertex3f(15.0f, -2.5f, -10.0f);
	glTexCoord2f(1, 0);
	glVertex3f(15.0f, -2.5f, 10.0f);
	glEnd();
	glPopMatrix();

	//UNDER FLOOR SIDING -- Z NEG
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, tex_ids[LIGHT_PANEL]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(15.0f, 0.0f, -10.0f);
	glTexCoord2f(0, 3);
	glVertex3f(-5.0f, 0.0f, -10.0f);
	glTexCoord2f(1, 3);
	glVertex3f(-5.0f, -2.5f, -10.0f);
	glTexCoord2f(1, 0);
	glVertex3f(15.0f, -2.5f, -10.0f);
	glEnd();
	glPopMatrix();

	//UNDER FLOOR SIDING -- Z POS
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, tex_ids[LIGHT_PANEL]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(15.0f, 0.0f, 10.0f);
	glTexCoord2f(0, 3);
	glVertex3f(-5.0f, 0.0f, 10.0f);
	glTexCoord2f(1, 3);
	glVertex3f(-5.0f, -2.5f, 10.0f);
	glTexCoord2f(1, 0);
	glVertex3f(15.0f, -2.5f, 10.0f);
	glEnd();
	glPopMatrix();

	//UNDER FLOOR SIDING -- BALCONY
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, tex_ids[LIGHT_PANEL]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-5.0f, 0.0f, 10.0f);
	glTexCoord2f(0, 3);
	glVertex3f(-5.0f, 0.0f, -10.0f);
	glTexCoord2f(1, 3);
	glVertex3f(-5.0f, -2.5f, -10.0f);
	glTexCoord2f(1, 0);
	glVertex3f(-5.0f, -2.5f, 10.0f);
	glEnd();
	glPopMatrix();


	//MID FLOOR GLASS CROSS BARS
	//glUseProgram(lightProg);
	glBindTexture(GL_TEXTURE_2D, tex_ids[CLEAR]);
	set_material(GL_FRONT_AND_BACK, &chrome);
	glPushMatrix();
	glTranslatef(5.0f, -0.1f, -10.0f);
	for (int i = 0; i < 5; i++)
	{
		glPushMatrix();
		glScalef(20.0f, 0.2f, 0.15f);
		//colorcube();
		texcube();
		glPopMatrix();
		glTranslatef(0.0f, 0.0f, 5.0f);
	}
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-5.0f, -0.1f, 0.0f);
	for (int i = 0; i < 5; i++)
	{
		glPushMatrix();
		glScalef(0.15f, 0.2f, 20.0f);
		//colorcube();
		texcube();
		glPopMatrix();
		glTranslatef(5.0f, 0.0f, 0.0f);
	}
	glPopMatrix();

	//MID FLOOR -- GLASS SURFACE
	glPushMatrix();
	set_material(GL_FRONT_AND_BACK, &glass);
	glDepthMask(GL_FALSE);
	glBegin(GL_QUADS);
	glVertex3f(-5.0f, 0.0f, 10.0f);
	glVertex3f(-5.0f, 0.0f, -10.0f);
	glVertex3f(15.0f, 0.0f, -10.0f);
	glVertex3f(15.0f, 0.0f, 10.0f);
	glEnd();
	glDepthMask(GL_TRUE);
	glPopMatrix();
	

	glPopAttrib();
	glEndList();
}


void wall_list()
{
	glNewList(WALLS, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);

	////glUseProgram(textureProg);
	glUseProgram(modProg);
	set_material(GL_FRONT, &clear);
	glBindTexture(GL_TEXTURE_2D, tex_ids[WALLPAPER]);
	//ENTRANCE WALL
	glPushMatrix();
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(HOUSE_X, WALL_Y_LOWER + 4.0f, HOUSE_Z);
		glTexCoord2f(1, 0);
		glVertex3f(HOUSE_X, WALL_Y_UPPER - 20.0f, HOUSE_Z);
		glTexCoord2f(1, 1);
		glVertex3f(HOUSE_X, WALL_Y_UPPER - 20.0f, -HOUSE_Z);
		glTexCoord2f(0, 1);
		glVertex3f(HOUSE_X, WALL_Y_LOWER + 4.0f, -HOUSE_Z);
		glEnd();
	glPopMatrix();

	//Z POS WALL
	glPushMatrix();
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

	//Z NEG WALL
	glPushMatrix();
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

	////glUseProgram(lightProg);
	glUseProgram(modProg);
	set_material(GL_FRONT, &black_plastic);
	glPushMatrix();
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
	//glUseProgram(textureProg);
	glBindTexture(GL_TEXTURE_2D, tex_ids[TABLE_WOOD]);
	glPushMatrix();
		glTranslatef(75.0f, 8.0f, -1.505f);
		glScalef(0.3f, 8.0f, 3.0f);
		texcube();
	glPopMatrix();

	//DOOR 2
	glPushMatrix();
	glTranslatef(75.0f, 8.0f, 1.505f);
	glScalef(0.3f, 8.0f, 3.0f);
	texcube();
	glPopMatrix();

	//glUseProgram(lightProg);

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


void bar_chair_list()
{
	glNewList(BAR_CHAIR, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);

	//glUseProgram(textureProg);
	glBindTexture(GL_TEXTURE_2D, tex_ids[TABLE_WOOD]);

	//seat
	glPushMatrix();
	glTranslatef(0.0f, 3.0f, 0.0f);
	glScalef(1.5f, 0.2f, 1.5f);
	hybridcube(TABLE_WOOD, TABLE_WOOD, TABLE_WOOD_LEG, TABLE_WOOD_LEG, TABLE_WOOD_LEG, TABLE_WOOD_LEG);
	glPopMatrix();
	//back
	glPushMatrix();
	glTranslatef(0.7f, 4.25f, 0.0f);
	glScalef(0.1f, 0.5f, 1.3f);
	hybridcube(TABLE_WOOD_LEG, TABLE_WOOD_LEG, TABLE_WOOD, TABLE_WOOD, TABLE_WOOD_LEG, TABLE_WOOD_LEG);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, tex_ids[TABLE_WOOD_LEG]);
	//leg 1
	glPushMatrix();
	glTranslatef(0.675f, 2.0f, 0.675f);
	glScalef(0.15f, 4.5f, 0.15f);
	texcube();
	glPopMatrix();
	//leg 2
	glPushMatrix();
	glTranslatef(0.675f, 2.0f, -0.675f);
	glScalef(0.15f, 4.5f, 0.15f);
	texcube();
	glPopMatrix();
	//leg 3
	glPushMatrix();
	glTranslatef(-0.675f, 1.25f, 0.675f);
	glScalef(0.15f, 3.5f, 0.15f);
	texcube();
	glPopMatrix();
	//leg 4
	glPushMatrix();
	glTranslatef(-0.675f, 1.25f, -0.675f);
	glScalef(0.15f, 3.5f, 0.15f);
	texcube();
	glPopMatrix();

	//glUseProgram(lightProg);

	glPopAttrib();
	glEndList();
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
		//glUseProgram(lightProg);
		// Top face
		rquad(cube[4], cube[7], cube[6], cube[5], vnorm[4], vnorm[7], vnorm[6], vnorm[5]);
	}
	else
	{
		//glUseProgram(textureProg);
		glBindTexture(GL_TEXTURE_2D, tex_ids[top]);
		// Top face
		tex_quad(cube[4], cube[7], cube[6], cube[5], vnorm[4], vnorm[7], vnorm[6], vnorm[5]);
	}

	if (bottom == NO_TEXTURES)
	{
		//glUseProgram(lightProg);
		// Bottom face
		rquad(cube[1], cube[2], cube[3], cube[0], vnorm[0], vnorm[1], vnorm[2], vnorm[3]);
	}
	else
	{
		//glUseProgram(textureProg);
		glBindTexture(GL_TEXTURE_2D, tex_ids[bottom]);
		// Bottom face
		tex_quad(cube[1], cube[2], cube[3], cube[0], vnorm[0], vnorm[1], vnorm[2], vnorm[3]);
	}

	if (left == NO_TEXTURES)
	{
		//glUseProgram(lightProg);
		// Left face
		rquad(cube[0], cube[3], cube[7], cube[4], vnorm[0], vnorm[3], vnorm[7], vnorm[4]);
	}
	else
	{
		//glUseProgram(textureProg);
		glBindTexture(GL_TEXTURE_2D, tex_ids[left]);
		// Left face
		tex_quad(cube[3], cube[7], cube[4], cube[0], vnorm[0], vnorm[3], vnorm[7], vnorm[4]);
	}

	if (right == NO_TEXTURES)
	{
		//glUseProgram(lightProg);
		// Right face
		rquad(cube[1], cube[5], cube[6], cube[2], vnorm[1], vnorm[5], vnorm[6], vnorm[2]);
	}
	else
	{
		//glUseProgram(textureProg);
		glBindTexture(GL_TEXTURE_2D, tex_ids[right]);
		// Right face
		tex_quad(cube[1], cube[5], cube[6], cube[2], vnorm[1], vnorm[5], vnorm[6], vnorm[2]);
	}

	if (front == NO_TEXTURES)
	{
		//glUseProgram(lightProg);
		// Front face
		rquad(cube[2], cube[6], cube[7], cube[3], vnorm[2], vnorm[6], vnorm[7], vnorm[3]);
	}
	else
	{
		//glUseProgram(textureProg);
		glBindTexture(GL_TEXTURE_2D, tex_ids[front]);
		// Front face
		tex_quad(cube[2], cube[6], cube[7], cube[3], vnorm[2], vnorm[6], vnorm[7], vnorm[3]);
	}

	if (back == NO_TEXTURES)
	{
		//glUseProgram(lightProg);
		// Back face
		rquad(cube[0], cube[4], cube[5], cube[1], vnorm[0], vnorm[4], vnorm[5], vnorm[1]);
	}
	else
	{
		//glUseProgram(textureProg);
		glBindTexture(GL_TEXTURE_2D, tex_ids[back]);
		// Back face
		tex_quad(cube[0], cube[4], cube[5], cube[1], vnorm[0], vnorm[4], vnorm[5], vnorm[1]);
	}
}

//Function to create a textured cube
void texcube()
{
	// Top face
	tex_quad(cube[4], cube[7], cube[6], cube[5], vnorm[4], vnorm[7], vnorm[6], vnorm[5]);

	// Bottom face
	tex_quad(cube[1], cube[2], cube[3], cube[0], vnorm[0], vnorm[1], vnorm[2], vnorm[3]);

	// Left face
	tex_quad(cube[3], cube[7], cube[4], cube[0], vnorm[0], vnorm[3], vnorm[7], vnorm[4]);

	// Right face
	tex_quad(cube[1], cube[5], cube[6], cube[2], vnorm[1], vnorm[5], vnorm[6], vnorm[2]);

	// Front face
	tex_quad(cube[2], cube[6], cube[7], cube[3], vnorm[2], vnorm[6], vnorm[7], vnorm[3]);

	// Back face
	tex_quad(cube[0], cube[4], cube[5], cube[1], vnorm[0], vnorm[4], vnorm[5], vnorm[1]);
}

//Function to create a textured quad
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
