//GENERAL HOUSE CONSTANTS
#define HOUSE_X_TOTAL		150.0f
#define HOUSE_X				HOUSE_X_TOTAL / 2
#define HOUSE_Y				40.0f
#define HOUSE_Z_TOTAL		100.0f
#define HOUSE_Z				HOUSE_Z_TOTAL / 2

#define SECOND_FLOOR		20.0f

//FLOOR CONSTANTS
#define FLOOR_Y				0.0f

//WALL CONSTANTS
#define WALL_Y_UPPER				40.0f
#define WALL_Y_LOWER				0.0f

#define WALKSPD				300.0f
#define FLY					//comment this out to remove flying and add boundary clipping
#define FAN

GLfloat roof[][3] = { { HOUSE_X, HOUSE_Y, HOUSE_Z }, { HOUSE_X, HOUSE_Y, -HOUSE_Z }, { -HOUSE_X, HOUSE_Y, -HOUSE_Z },
{ -HOUSE_X, HOUSE_Y, HOUSE_Z } };

GLfloat window_right[][3] = { { 45.0f, 19.0f, -HOUSE_Z + 0.1f }, {45.0f, 11.0f, -HOUSE_Z + 0.1f},
{ 35.0f, 11.0f, -HOUSE_Z + 0.1f }, {35.0f, 19.0f, -HOUSE_Z + 0.1f} };

GLfloat window_left[][3] = { { 15.0f, 19.0f, -HOUSE_Z + 0.1f }, { 15.0f, 11.0f, -HOUSE_Z + 0.1f },
{ 5.0f, 11.0f, -HOUSE_Z + 0.1f }, { 5.0f, 19.0f, -HOUSE_Z + 0.1f } };


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