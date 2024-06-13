/*
    Author:00957144
    This program simulates the robot(motion and animation).
    About rendering and shading.
    Operate the robot to walk, run, jump, rotate body and joints, grasp and bend.
    There are rocks, mines, a teleportation, floor, an analyzer and a screen in the scene.
    Teleportation: When robot walk into, it can move to other teleportation immediately.
    Analyzer and screen: When robot grasp a mine and put mine into analyzer, the screen would show mine information.
 */
#include <math.h>
#include <stdio.h>
#include <GL/glut.h>
#include <stdlib.h> 
#include <iostream>
#include <vector>
#include <string>
#include <time.h>
using namespace std;
const double pi = acos(-1);
// Vertices of the box
float points[][3] = { {-0.5, -0.5, -0.5}, {0.5, -0.5, -0.5}, {0.5, 0.5, -0.5},
    {-0.5, 0.5, -0.5}, {-0.5, -0.5, 0.5}, {0.5, -0.5, 0.5}, {0.5, 0.5, 0.5}, {-0.5, 0.5, 0.5} };
// face of box, each face composing of 4 vertices
int face[][4] = { {0, 3, 2, 1}, {0, 1, 5, 4}, {1, 2, 6, 5}, {4, 5, 6, 7}, {2, 3, 7, 6}, {0, 4, 7, 3} };
// indices of the box faces 
int cube[6] = { 0, 1, 2, 3, 4, 5 };
// Define normals of faces
float  normal[][4] = { {0.0, 0.0, -1.0}, {0.0, -1.0, 0.0}, {1.0, 0.0, 0.0},
              {0.0, 0.0, 1.0}, {0.0, 1.0, 0.0}, {-1.0, 0.0, 0.0} };


// material properties for robot
float  robot_ambient[] = { 132.0 / 255,193.0 / 255,1.0,1.0 };
float  robot_diffuse[] = { 150.0 / 255,220.0 / 255,1.0,1.0 };
float  robot_specular[] = { 0.508273f, 0.508273f, 0.508273f, 1.0 };
float  robot_shininess = 89.6f;
// material properties for robot joints
float  joint_ambient[] = { 70.0 / 255,140.0 / 255,217.0 / 255, 1.0 };
float  joint_diffuse[] = { 70.0 / 255,140.0 / 255,217.0 / 255, 1.0 };
float  joint_specular[] = { 0.508273f, 0.508273f, 0.508273f, 1.0 };
float  joint_shininess = 89.6f;
// material properties for floor
float  floor_diffuse[] = { 151 / 255.0, 124 / 255.0, 0.0, 1.0 };
float  floor_ambient[] = { 20 / 255.0, 20 / 255.0, 0.0, 1.0 };
float  floor_specular[] = { 0.0, 0.0, 0.0, 1.0 };
float  floor_shininess = 4.0;

// light source:disco
float  disco_position[] = { 20, 15.0, 0.0, 1.0 };
float  disco_direction[] = { 0, -1.0, 1.0, 0.0 };
float  disco_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
float  disco_specular[] = { 0.7, 0.7, 0.7, 1.0 };
float  disco_cutoff = 60.0;
float  disco_exponent = 8.0;
// light source:sun
float  light_sun_position[] = { 35, 30, 0,0 };// directional light
float  light_sun_direction[] = {0.0, -1.0, 0.0, 0.0 };
float  light_sun_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
float  light_sun_specular[] = { 0.1, 0.1, 0.1, 1.0 };
// light source:fluorescent lamp
float  lamp_position[] = { 25, 10, 0,1 };// point light
float  lamp_diffuse[] = { 0.8, 0.8, 1, 1.0 };
float  lamp_specular[] = { 0.1, 0.1, 0.1, 1.0 };
// light source:flashlight
float  flashlight_position[] = { 0, 0, 0,1 };// point light
GLfloat  flashlight_direction[2][4] = { { 0.0, 0.0, 1.0, 0.0 },{ 1.0, 0.0, 0.0, 0.0 } };
float  flashlight_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
float  flashlight_specular[] = { 0.1, 0.1, 0.1, 1.0 };
float  flashlight_cutoff = 15;
float  flashlight_exponent = 8.0;
// light source:meteor
float  meteor_position[] = { -100, 120, -30,1 };// point light
float  meteor_diffuse[] = { 0.1, 0.1, 0.1, 1.0 };
float  meteor_specular[] = { 0.05, 0.05, 0.05, 1.0 };
float  meteor_cutoff = 15;
float  meteor_exponent = 100.0;

float  global_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
// light source angle
float  lit_angle = 0.0;
float  sun_angle = 0.0;
// light on control
bool fluorescent_lamp_on = 1;
bool flashlight_on = 1;
bool disco_on = 1;
bool light_effect = 1;
// light direction control
int flashlight_dir = 0;

// color setting
double joint_color[3] = { 38.0 / 255,127.0 / 255,217.0 / 255 };
double body_color[3] = { 132.0 / 255,193.0 / 255,255.0 / 255 };
double finger_color[3] = { 190.0 / 255,190.0 / 255,190.0 / 255 };
double arm_color[3] = { 175.0 / 255,193.0 / 255,228.0 / 255 };

// quadric object
GLUquadricObj* sphere = NULL, * cylind = NULL, * disk = NULL, * bubble = NULL;

// joint angle setting
double arm_angle[2][2] = { 0 };// arm
double elbow_angle[2] = { 0 };// elbow
double wrist_angle[2] = { 0 };// wrist
double finger_angle[2] = { 30,30 };// fingers
double leg_angle[2] = { 0 };// leg
double knee_angle[2] = { 0 };// knee
double ankle_angle[2] = { 0 };// ankle
double head_angle = 0;// head
double position[3] = { 25,2.7 - 0.3,25 };// robot position
double rotation[3] = { 0,0,0 };// robot body angle

#define LEFT 0
#define RIGHT 1
#define Step  0.5

int width = 676, height = 676; // window size

struct node {// struct to represent items
    double dx, dy, dz, size, r, g, b;
    float  ambient[4] = { 0 }, specular[4] = { 0 }, diffuse[4] = { 0 };
    GLfloat shiness = 0.0;
    bool show = 1;
    string name = "";
    node() { dx = dy = dz = size = r = g = b = 0; }
    node(double sz, double x, double y, double z, double r_, double g_, double b_) {
        dx = x, dy = y, dz = z, size = sz, r = r_, g = g_, b = b_;
        if (r_ > 1)r = r_ / 255.0, g = g_ / 255.0, b = b_ / 255.0;
        for (int i = 0; i < 4; i++)ambient[i] = specular[i] = diffuse[i] = 0;
    }
    node(double sz, double x, double y, double z, double r_, double g_, double b_, string s,float ambient_[],float specular_[], float diffuse_[], float shiness_) {
        dx = x, dy = y, dz = z, size = sz, r = r_, g = g_, b = b_, name = s;
        if (r_ > 1)r = r_ / 255.0, g = g_ / 255.0, b = b_ / 255.0;
        for (int i = 0; i < 4; i++) {
            //ambient_[0]=r, ambient_[1]=g, ambient_[2]=b;
            ambient[i] = ambient_[i];
            specular[i] = specular_[i];
            diffuse[i] = diffuse_[i];
            shiness = shiness_;
        }
    }
};
vector<node>rocks, mines;
node robot, tele_L, tele_R, analyzer;
// initial variables
int step = 0, cur_step = 0;
int LR = 0;
int take_mine = -1, show_mine = -1;
bool first = 1;
bool walk_start = 0;
bool walk_run = 0;// walk:0 run:1
bool first_line = 0;
int bubble_cnt = -1;
int word_cnt = 0;
int mine_fall_cnt = 0;
int stamp = 0;
string str;

// Translation and rotations of eye coordinate system
float   eyeDx = 0.0, eyeDy = 0.0, eyeDz = 0.0;
float   eyeAngx = 0.0, eyeAngy = 0.0, eyeAngz = 0.0;
double  Eye[3] = { 30, 11, 5 }, Focus[3] = { 35.0, 10.0, 10.0 },
Vup[3] = { 0.0, 1.0, 0.0 };
float   u[3][3] = { {1.0,0.0,0.0}, {0.0,1.0,0.0}, {0.0,0.0,-1.0} };
float   eye[3];
float   cv, sv; /* cos(5.0) and sin(5.0) */
// Drawing style 0:4-windows, 1:x direction, 2:y direction, 3:z-dirtection, 4:perspective
int style = 0;
// variables for define view volume
double fovy = 60, aspect = (double)width / (double)height, zNear = 5, zFar = 70.0;
#define nl 0
#define nr 1
#define nb 2
#define nt 3
#define nn 4
#define nf 5
double ncw[6] = { -40,40,-40,40,0,100 };// l r b t n f for parallel projection zoom in/out

// pre declare
void display();
void stand_posture();
double dist(node& a, node& b);
void draw_view_volume();

// draw scene and object
/*
void show_mine_info() {// show mine info or initial welcome words on screen
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, width, 0.0, height);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glColor3f(0.0, 1.0, 0.0);

    if (show_mine != -1) {// when robot put mine into analyzer
        glRasterPos2i(200, 505);
        if (first_line) {// screen has show first line("Analysis...")
            str = mines[show_mine].name;
            for (int i = 0; i < min(word_cnt / 3, (int)str.size()); i++)
                glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[i]);
        }
        else {// screen hasn't show first line
            str = "Analysis...                     ";
            for (int i = 0; i < min(word_cnt / 3, (int)str.size()); i++) {
                glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[i]);
                if (i == (int)str.size() - 1)
                    first_line = 1, word_cnt = 0;
            }
        }
    }
    else {// initial screen
        glRasterPos2i(200, 505);
        str = "Hello       ";
        if (first_line) {// screen has show first line("Hello")
            for (int i = 0; i < (int)str.size(); i++)
                glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[i]);
        }
        else {// screen hasn't show first line
            for (int i = 0; i < min(word_cnt / 3, (int)str.size()); i++) {
                glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[i]);
                if (i == (int)str.size() - 1)first_line = 1, word_cnt = 0;
            }
        }
        glRasterPos2i(201, 487);
        str = "Put mine here";
        if (first_line) {// screen has show first line
            for (int i = 0; i < min(word_cnt / 3, (int)str.size()); i++)
                glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[i]);
        }
    }

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}
*/
void draw_sphere(float mat_ambient[], float mat_specular[], float mat_diffuse[], GLfloat mat_shininess,double radius,int slice,int stack) {// draw a unit cube
    // Define some material properties
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    if(mat_specular[0]+ mat_specular[1]+ mat_specular[2]>0)glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    if (mat_shininess > 0)glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
    gluSphere(sphere, radius, slice, stack);
}
void draw_cube(float mat_ambient[], float mat_specular[], float mat_diffuse[], GLfloat mat_shininess) {// draw a unit cube
    // Define some material properties
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
    for (int i = 0; i < 6; i++) {
        glNormal3fv(normal[i]);
        glBegin(GL_POLYGON); // Draw the face
        glVertex3fv(points[face[i][0]]);
        glVertex3fv(points[face[i][1]]);
        glVertex3fv(points[face[i][2]]);
        glVertex3fv(points[face[i][3]]);
        glEnd();
    }
}
void draw_a_rock(node& x) {// draw a rock or mine
    glColor3f(x.r, x.g, x.b);
    glPushMatrix();
    glTranslatef(x.dx, x.dy, x.dz);

    // material properties for rocks
    float  rock_diffuse[] = { x.r, x.g, x.b, 1.0 };
    float  rock_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
    float  rock_specular[] = { 0.0, 0.0, 0.0, 1.0 };
    float rock_shininess = 0;
    if (x.r == x.g && x.g == x.b) {// a rock

    }
    else {
        rock_specular[0] = rock_specular[1] = rock_specular[2] = 1.0;
        rock_shininess = 0.6;
    }
    draw_sphere(rock_ambient, rock_specular, rock_diffuse, rock_shininess, x.size, 12, 12);
    glPopMatrix();
}
/*
void draw_analyzer() {// draw analyzer
    glPushMatrix();// cylinder

    glColor3f(0.6, 0.6, 0.6);
    glTranslatef(40, 0, 45);
    glRotatef(-90, 1, 0, 0);
    gluCylinder(cylind, 2, 2, 1.5, 12, 3);
    glPopMatrix();

    glPushMatrix();// disk
    glColor3f(0.57, 0.57, 0.57);
    glTranslatef(40, 1.5, 45);
    glRotatef(-90, 1, 0, 0);
    gluDisk(disk, 0, 2, 12, 3);
    glPopMatrix();

    glPushMatrix();// cylinder
    glColor3f(0.9, 0.9, 0.9);
    glTranslatef(40, 1.5, 45);
    glRotatef(-90, 1, 0, 0);
    gluCylinder(cylind, 1.5, 1.5, 0.5, 12, 3);
    glPopMatrix();

    glPushMatrix();// disk
    glColor3f(0.8, 0.8, 0.8);
    glTranslatef(40, 2, 45);
    glRotatef(-90, 1, 0, 0);
    
    gluDisk(disk, 0, 1.5, 12, 3);
    glPopMatrix();

    // draw outer screen
    glPushMatrix();
    glTranslatef(40, 10, 55);
    glScalef(15, 10, 1);
    glColor3f(1, 1, 1);
    draw_cube(outscr_ambient, outscr_specular,outscr_diffuse,outscr_shininess);
    glPopMatrix();// end outer screen
    // draw inner screen
    glPushMatrix();
    glTranslatef(40, 10, 54.5);
    glPushMatrix();// draw cube
    glScalef(14.5, 9.5, 1);
    glColor3f(0, 0, 0.15);
    draw_cube(innscr_ambient, innscr_specular, innscr_diffuse, innscr_shininess);
    glPopMatrix();// end cube

    glPushMatrix();// draw disk
    glTranslatef(0, -1.0, -1.0);
    if (show_mine != -1) {// show mine
        glColor3f(mines[show_mine].r, mines[show_mine].g, mines[show_mine].b);
        gluDisk(disk, 0, mines[show_mine].size * 3, 12, 3);
    }
    glPopMatrix();// end disk

    glPopMatrix();// end inner screen
}
*/
void draw_floor() {// draw floor
    // [0,0,0] ~ [50,0,50]
    // Define floor properties
    glMaterialfv(GL_FRONT, GL_AMBIENT, floor_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, floor_diffuse);
    glPushMatrix();
    glTranslatef(25, -1, 25);
    glScalef(50, 2, 50);
    glColor3fv(floor_diffuse);
    for (int i = 0; i < 6; i++) {
        glNormal3fv(normal[i]);
        glBegin(GL_POLYGON);
        glVertex3fv(points[face[i][0]]);
        glVertex3fv(points[face[i][1]]);
        glVertex3fv(points[face[i][2]]);
        glVertex3fv(points[face[i][3]]);
        glEnd();
    }
    glPopMatrix();
}
void draw_flashlight() {
    glPushMatrix();
        // Draw light source
        glPushMatrix();
            GLfloat mat_diffuse[] = { 0.0f,0.50980392f,0.50980392f,1.0f };
            GLfloat mat_specular[] = { 0,0.50196078f,0.50196078f,1.0f };
            GLfloat mat_emission[2][4] = { { 0.01, 0.01, 0.05, 0.0 },{ 0, 0, 0, 0.0 } };

            glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
            glMaterialf(GL_FRONT, GL_SHININESS, 100);
            if (flashlight_on)glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission[0]);
            else glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission[1]);
            glutSolidSphere(0.4, 20, 20);
        glPopMatrix();
        
        if(light_effect)glEnable(GL_LIGHTING);
        glLightfv(GL_LIGHT3, GL_POSITION, flashlight_position);
        glLightfv(GL_LIGHT3, GL_DIFFUSE, flashlight_diffuse);
        glLightfv(GL_LIGHT3, GL_SPECULAR, flashlight_specular);
        glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, flashlight_direction[flashlight_dir]);
        glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, flashlight_cutoff);
        glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, flashlight_exponent);
        
    glPopMatrix();
}
void moveto_flashlight() {
    // move to robot
    glPushMatrix();
        glTranslatef(position[0], position[1], position[2]);
        glRotatef(rotation[1], 0, 1, 0);
        glRotatef(rotation[0], 1, 0, 0);
        // move to arm
        glTranslatef(0.0, 0.3 + 1.5 + 0.9, 0.0);
        // left arm
        glPushMatrix();
            glTranslatef(-1.3, 0, 0.0);// move to left arm coord sys
            glRotatef(180, 1.0, 0.0, 0.0);// turn down
            // left joint1
            glRotatef(arm_angle[0][0], 1, 0, 0);
            glRotatef(arm_angle[0][1], 0, 0, 1);
            // left joint2
            glTranslatef(0.0, 1.3, 0.0);// move to left elbow coord
            glRotatef(elbow_angle[0], 1, 0, 0);
            // left joint3
            glTranslatef(0.0, 0.85, 0.0);// move to left wrist coord sys
            glRotatef(wrist_angle[0], 1, 0, 0); 
            // draw flashlight
            draw_flashlight();

        glPopMatrix();// end left arm
    glPopMatrix();
}
void draw_left_arm() {
    // draw left arm
    glPushMatrix();
    glTranslatef(-1.3, 0, 0.0);// move to left arm coord sys
    glRotatef(180, 1.0, 0.0, 0.0);// turn down
    // draw left joint1
    glRotatef(arm_angle[0][0], 1, 0, 0);
    glRotatef(arm_angle[0][1], 0, 0, 1);
    glPushMatrix();
    glColor3f(joint_color[0], joint_color[1], joint_color[2]);
    draw_sphere(joint_ambient, joint_specular, joint_diffuse, joint_shininess, 0.3, 12, 12);
    glPopMatrix();// end left joint1

    // draw left up arm
    glPushMatrix();
    glTranslatef(0, 0.7, 0.0);// move to left arm middle
    glScalef(0.4, 0.8, 0.4);
    glColor3f(arm_color[0], arm_color[1], arm_color[2]);
    draw_cube(robot_ambient,robot_specular,robot_diffuse,robot_shininess);
    glPopMatrix();// end left up arm

    // draw left joint2
    glTranslatef(0.0, 1.3, 0.0);// move to left elbow coord
    glRotatef(elbow_angle[0], 1, 0, 0);
    glPushMatrix();
    glColor3f(joint_color[0], joint_color[1], joint_color[2]);
    draw_sphere(joint_ambient, joint_specular, joint_diffuse, joint_shininess, 0.2, 12, 12);
    glPopMatrix();// end left joint2

    // draw left down arm
    glPushMatrix();
    glTranslatef(0.0, 0.45, 0.0);
    glScalef(0.3, 0.5, 0.3);
    glColor3f(arm_color[0], arm_color[1], arm_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess);
    glPopMatrix();// end left down arm

    // draw left joint3
    glTranslatef(0.0, 0.85, 0.0);// move to left wrist coord sys
    glRotatef(wrist_angle[0], 1, 0, 0);
    glPushMatrix();
    glColor3f(joint_color[0], joint_color[1], joint_color[2]);
    draw_sphere(joint_ambient, joint_specular, joint_diffuse, joint_shininess, 0.15, 12, 12);
    glPopMatrix();// end left joint3


    // draw left finger
    glPushMatrix();
    glRotatef(-finger_angle[0], 0, 0, 1);
    glTranslatef(0.06, 0.2, 0);
    glScalef(0.12, 0.3, 0.12);
    glColor3f(finger_color[0], finger_color[1], finger_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess);
    glPopMatrix();// end left finger
    // draw right finger
    glPushMatrix();
    glRotatef(finger_angle[0], 0, 0, 1);
    glTranslatef(-0.06, 0.2, 0);
    glScalef(0.12, 0.3, 0.12);
    glColor3f(finger_color[0], finger_color[1], finger_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess);
    glPopMatrix();// end left finger

    glPopMatrix();// end left arm
}
void draw_right_arm() {
    // draw right arm
    glPushMatrix();
    glTranslatef(1.3, 0, 0.0);// move to right arm coord sys
    glRotatef(180, 1.0, 0.0, 0.0);// turn down
    // draw right joint1
    glRotatef(arm_angle[1][0], 1, 0, 0);
    glRotatef(arm_angle[1][1], 0, 0, 1);
    glPushMatrix();
    glColor3f(joint_color[0], joint_color[1], joint_color[2]);
    draw_sphere(joint_ambient, joint_specular, joint_diffuse, joint_shininess, 0.3, 12, 12);
    glPopMatrix();// end right joint1

    // draw right up arm
    glPushMatrix();
    glTranslatef(0, 0.7, 0.0);// move to right arm middle
    glScalef(0.4, 0.8, 0.4);
    glColor3f(arm_color[0], arm_color[1], arm_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess);
    glPopMatrix();// end right up arm

    // draw right joint2
    glTranslatef(0.0, 1.3, 0.0);// move to right elbow coord
    glRotatef(elbow_angle[1], 1, 0, 0);
    glPushMatrix();
    glColor3f(joint_color[0], joint_color[1], joint_color[2]);
    draw_sphere(joint_ambient, joint_specular, joint_diffuse, joint_shininess, 0.2, 12, 12);
    glPopMatrix();// end right joint2

    // draw right down arm
    glPushMatrix();
    glTranslatef(0.0, 0.45, 0.0);
    glScalef(0.3, 0.5, 0.3);
    glColor3f(arm_color[0], arm_color[1], arm_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess);
    glPopMatrix();// end right down arm

    // draw right joint3
    glTranslatef(0.0, 0.85, 0.0);// move to right wrist coord sys
    glRotatef(wrist_angle[1], 1, 0, 0);
    glPushMatrix();
    glColor3f(joint_color[0], joint_color[1], joint_color[2]);
    draw_sphere(joint_ambient, joint_specular, joint_diffuse, joint_shininess, 0.15, 12, 12);
    glPopMatrix();// end right joint3

    // draw take mine
    if (take_mine != -1) {
        glPushMatrix();
        glColor3f(mines[take_mine].r, mines[take_mine].g, mines[take_mine].b);
        gluSphere(sphere, mines[take_mine].size, 12, 12);
        glPopMatrix();
    }

    // draw left finger
    glPushMatrix();
    glRotatef(-finger_angle[1], 0, 0, 1);
    glTranslatef(0.06, 0.2, 0);
    glScalef(0.12, 0.3, 0.12);
    glColor3f(finger_color[0], finger_color[1], finger_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess);
    glPopMatrix();// end left finger
    // draw right finger
    glPushMatrix();
    glRotatef(finger_angle[1], 0, 0, 1);
    glTranslatef(-0.06, 0.2, 0);
    glScalef(0.12, 0.3, 0.12);
    glColor3f(finger_color[0], finger_color[1], finger_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess);
    glPopMatrix();// end left finger

    glPopMatrix();// end right arm
}
void draw_left_leg() {
    // draw left leg
    glPushMatrix();
    glTranslatef(-0.6, 0, 0.0);// move to left leg coord sys
    glRotatef(180, 1.0, 0.0, 0.0);// turn down
    // draw left joint1
    glRotatef(leg_angle[0], 1, 0, 0);
    glPushMatrix();
    glColor3f(joint_color[0], joint_color[1], joint_color[2]);
    draw_sphere(joint_ambient, joint_specular, joint_diffuse, joint_shininess, 0.3, 12, 12);
    glPopMatrix();// end left joint1

    // draw left up leg
    glPushMatrix();
    glTranslatef(0, 0.7, 0.0);// move to left leg middle
    glScalef(0.5, 0.8, 0.5);
    glColor3f(arm_color[0], arm_color[1], arm_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess);
    glPopMatrix();// end left up leg

    // draw left joint2
    glTranslatef(0.0, 1.3, 0.0);// move to left knee coord
    glRotatef(knee_angle[0], 1, 0, 0);
    glPushMatrix();
    glColor3f(joint_color[0], joint_color[1], joint_color[2]);
    draw_sphere(joint_ambient, joint_specular, joint_diffuse, joint_shininess,0.2, 12, 12);
    glPopMatrix();// end left joint2

    // draw left down leg
    glPushMatrix();
    glTranslatef(0.0, 0.45, 0.0);
    glScalef(0.4, 0.5, 0.4);
    glColor3f(arm_color[0], arm_color[1], arm_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess);
    glPopMatrix();// end left down leg

    // draw left joint3
    glTranslatef(0.0, 0.85, 0.0);// move to left ankle coord sys
    glRotatef(ankle_angle[0], 1, 0, 0);
    glPushMatrix();
    glColor3f(joint_color[0], joint_color[1], joint_color[2]);
    draw_sphere(joint_ambient, joint_specular, joint_diffuse, joint_shininess, 0.2, 12, 12);
    glPopMatrix();// end left joint3

    // draw left foot
    glPushMatrix();
    glTranslatef(0.0, 0.1, 0.35);
    glScalef(0.4, 0.2, 0.4);
    glColor3f(arm_color[0], arm_color[1], arm_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess);
    glPopMatrix();// end left foot

    glPopMatrix();// end left leg
}
void draw_right_leg() {
    // draw right leg
    glPushMatrix();
    glTranslatef(0.6, 0, 0.0);// move to right leg coord sys
    glRotatef(180, 1.0, 0.0, 0.0);// turn down
    // draw right joint1
    glRotatef(leg_angle[1], 1, 0, 0);
    glPushMatrix();
    glColor3f(joint_color[0], joint_color[1], joint_color[2]);
    draw_sphere(joint_ambient, joint_specular, joint_diffuse, joint_shininess, 0.3, 12, 12);
    glPopMatrix();// end right joint1

    // draw right up leg
    glPushMatrix();
    glTranslatef(0, 0.7, 0.0);// move to right leg middle
    glScalef(0.5, 0.8, 0.5);
    glColor3f(arm_color[0], arm_color[1], arm_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess);
    glPopMatrix();// end right up leg

    // draw right joint2
    glTranslatef(0.0, 1.3, 0.0);// move to right knee coord
    glRotatef(knee_angle[1], 1, 0, 0);
    glPushMatrix();
    glColor3f(joint_color[0], joint_color[1], joint_color[2]);
    draw_sphere(joint_ambient, joint_specular, joint_diffuse, joint_shininess, 0.2, 12, 12);
    glPopMatrix();// end right joint2

    // draw right down leg
    glPushMatrix();
    glTranslatef(0.0, 0.45, 0.0);
    glScalef(0.4, 0.5, 0.4);
    glColor3f(arm_color[0], arm_color[1], arm_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess);
    glPopMatrix();// end right down leg

    // draw right joint3
    glTranslatef(0.0, 0.85, 0.0);// move to right ankle coord sys
    glRotatef(ankle_angle[1], 1, 0, 0);
    glPushMatrix();
    glColor3f(joint_color[0], joint_color[1], joint_color[2]);
    draw_sphere(joint_ambient, joint_specular, joint_diffuse, joint_shininess, 0.2, 12, 12);
    glPopMatrix();// end right joint3

    // draw right foot
    glPushMatrix();
    glTranslatef(0.0, 0.1, 0.35);
    glScalef(0.4, 0.2, 0.4);
    glColor3f(arm_color[0], arm_color[1], arm_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess);
    glPopMatrix();// end right foot

    glPopMatrix();// end right leg
}
void draw_left_eye() {
    // draw left eye
    glPushMatrix();// cylinder
    glTranslatef(-0.8, 0, 0);
    glRotatef(-180, 1, 0, 0);

    /*
    *float  mat_diffuse[] = { 1.0,0.0, 0.0, 1.0 };
    float  mat_ambient[] = { 0.0,0.0, 0.0, 1.0 };
    float  mat_specular[] = { 1.0,1.0, 1.0, 1.0 };
    float  mat_shininess = 80.0;
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
    */
    glColor3f(1, 1, 1);
    gluCylinder(cylind, 0.2, 0.2, 0.05, 12, 3);
    glPopMatrix();

    glPushMatrix();// disk
    glTranslatef(-0.8, 0, -0.05);
    glColor3f(0, 0, 0);
    gluDisk(disk, 0, 0.2, 12, 3);
    /*
    gluQuadricNormals(disk, GLU_SMOOTH);
    glMaterialfv(GL_FRONT, GL_AMBIENT, floor_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, floor_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, floor_diffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, floor_shininess);
    */
    glPopMatrix();// end left eye
}
void draw_right_eye() {
    // draw right eye
    glPushMatrix();// cylinder
    glTranslatef(0.8, 0, 0);
    glRotatef(-180, 1, 0, 0);
    glColor3f(1, 1, 1);

    gluCylinder(cylind, 0.2, 0.2, 0.05, 12, 3);
    glPopMatrix();

    glPushMatrix();// disk
    glTranslatef(0.8, 0, -0.05);
    glColor3f(0, 0, 0);
    gluDisk(disk, 0, 0.2, 12, 3);
    glPopMatrix();// end right eye
}
void draw_rocks() {// create and draw rocks
    if (first) {// create rocks
        rocks.push_back(node(0.4, 10, 0.3, 10, 0.2, 0.2, 0.2));
        rocks.push_back(node(1.0, 30, 0.2, 20, 0.2, 0.2, 0.2));
        rocks.push_back(node(1.0, 15, 0.2, 15, 0.25, 0.25, 0.25));
        rocks.push_back(node(0.8, 11, 0.2, 9, 0.4, 0.4, 0.4));
        rocks.push_back(node(0.8, 25, 0.2, 17, 0.4, 0.4, 0.4));
        rocks.push_back(node(0.8, 42, 0.2, 33, 0.4, 0.4, 0.4));
        rocks.push_back(node(0.7, 40, 0.2, 5, 0.4, 0.4, 0.4));
        rocks.push_back(node(1.2, 13, 0.2, 10, 0.4, 0.4, 0.4));
    }
    // draw all rocks
    for (int i = 0; i < rocks.size(); i++)draw_a_rock(rocks[i]);
}
void draw_mines() {// create and draw mines
    float  ambient[4] = { 0 }, specular[4] = { 0 }, diffuse[4] = { 0 };
    GLfloat shiness = 0.0;
    if (first) {
        first = 0;
        //mines.push_back(node(0.4, 18, 0.2, 48, 203, 251, 251, "Diamond"));
        {
            float  ambient[4] = { 0.24725,0.1995,0.0745,1.0 }, specular[4] = { 0.628281,0.555802,0.366065,1.0 }, diffuse[4] = { 0.75164,0.60648,0.22648,1.0 };
            GLfloat shiness = 51.2f;
            mines.push_back(node(2, 25, 0.2, 20, 255, 209, 5, "Gold", ambient, specular, diffuse, shiness));
        } 
        float  ambient[4] = { 0.1745f, 0.01175f, 0.01175f, 0.55f }, specular[4] = { 0.727811f, 0.626959f, 0.626959f, 0.55f }, diffuse[4] = { 0.61424f, 0.04136f, 0.04136f, 0.55f };
        GLfloat shiness = 76.8f;
        mines.push_back(node(2, 41, 0.2, 19, 229, 36, 36, "Ruby", ambient, specular, diffuse, shiness));
        
        mines.push_back(node(1, 35, 0.2, 38, 18, 18, 217, "Lapis Lazuli", ambient, specular, diffuse, shiness));
        mines.push_back(node(2, 47, 0.2, 2, 117, 177, 27, "Olivine", ambient, specular, diffuse, shiness));
        mines.push_back(node(2, 7, 0.1, 45, 247, 182, 182, "Rhodonite", ambient, specular, diffuse, shiness));
        mines.push_back(node(1.5, 11, 0.2, 13, 223, 236, 236, "Opal", ambient, specular, diffuse, shiness));
        //mines.push_back(node(0.4, 24, 0.2, 13, 244, 143, 42, "Amber"));

    }
    // draw all mines
    for (int i = 0; i < mines.size(); i++)if (mines[i].show)draw_a_rock(mines[i]);
}
void draw_robot() {// draw a robot
    glPushMatrix();
    glTranslatef(position[0], position[1], position[2]);
    // set position and rotation angle
    robot.dx = position[0];
    robot.dy = position[1];
    robot.dz = position[2];
    glRotatef(rotation[1], 0, 1, 0);
    glRotatef(rotation[0], 1, 0, 0);

    // draw body cube (body coord sys)
    glPushMatrix();
    glTranslatef(0, 1.5 + 0.3, 0);
    glScalef(2.0, 3.0, 2.0);
    glColor3f(body_color[0], body_color[1], body_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess);
    glPopMatrix();

    // draw neck and head
    glPushMatrix();
    // draw neck (neck coord sys)
    glTranslatef(0, 3.0 + 0.3 + 0.3, 0); // move to neck coord sys
    glPushMatrix();
    glScalef(0.5, 0.6, 0.5);
    glColor3f(joint_color[0], joint_color[1], joint_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess);
    glPopMatrix();// end neck

    // draw head (head coord sys)
    glTranslatef(0.0, 0.3 + 0.5, 0.0); // move to head coord sys
    glRotatef(head_angle, 0, 1, 0);
    glPushMatrix();
    glScalef(2.5, 0.8, 2);
    glColor3f(body_color[0], body_color[1], body_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess);
    glPopMatrix();// end head

    // draw eyes
    glTranslatef(0.0, 0.0, -1); // move to draw eye
    draw_left_eye();
    draw_right_eye();
    glPopMatrix(); // end neck and head

    // draw arms
    glPushMatrix();
    glTranslatef(0.0, 0.3 + 1.5 + 0.9, 0.0);
    draw_left_arm();
    draw_right_arm();
    glPopMatrix();// end arms

    // draw legs
    glPushMatrix();
    glTranslatef(0.0, 0.3 + 1.5 - 1.8, 0.0);
    draw_left_leg();
    draw_right_leg();
    glPopMatrix();// end legs
    glPopMatrix();// end robot
}
void bubble_special_effects() {// when mine into analyzer, analyzer will blobloblo~~
    if (bubble == NULL) {
        bubble = gluNewQuadric();
        //gluQuadricDrawStyle(bubble, GLU_SILHOUETTE);
        gluQuadricDrawStyle(bubble, GLU_FILL);
        gluQuadricNormals(bubble, GLU_SMOOTH);
    }
    double dx, dy, dz, sz, g, b, factor;
    for (int i = 0; i < 3; i++) {// three bubble, random position, random color
        glPushMatrix();
        dx = (41.5 - 38.5) * rand() / (RAND_MAX + 1.0) + 38.5;
        dz = (46.5 - 43.5) * rand() / (RAND_MAX + 1.0) + 43.5;
        factor = (0.1 - (-0.1)) * rand() / (RAND_MAX + 1.0);
        dy = bubble_cnt / 20.0 + 1.6 + factor;
        sz = (0.5 - 0.1) * rand() / (RAND_MAX + 1.0) + 0.1;
        g = (255 - 0) * rand() / (RAND_MAX + 1.0) + 0;
        b = (255 - 199) * rand() / (RAND_MAX + 1.0) + 199;
        glColor3f(0.0, g / 255, b / 255);
        glTranslatef(dx, dy, dz);
        gluDisk(bubble, 0, sz, 12, 12);
        glPopMatrix();
    }
}
void draw_teleportation() {// draw teleportation
    // right
    glPushMatrix();// disk down
    glTranslatef(5, 0.01, 25);
    glRotatef(-90, 1, 0, 0);
    glColor3f(204 / 255.0, 255 / 255.0, 255 / 255.0);
    gluDisk(disk, 0, 2.5, 12, 3);
    glPopMatrix();// end disk down

    glPushMatrix();// disk up
    glTranslatef(5, 0.02, 25);
    glRotatef(-90, 1, 0, 0);
    glColor3f(1, 1, 1);
    if (stamp < 50)gluDisk(disk, 0, 2.0, 12, 3);
    else if (stamp < 100)gluDisk(disk, 0, 1.5, 12, 3);
    else gluDisk(disk, 0, 1.0, 12, 3);
    glPopMatrix();// end disk up

    glPushMatrix();// balls
    glColor3f(0, 0.8, 1);
    for (double x = 3.0; x <= 7.0; x += 1.0) {
        for (double y = 0.5; y <= 3; y += 0.5) {
            glPushMatrix();
            if (stamp < 100)glTranslatef(x, y, 25);
            else glTranslatef(x, y + 0.5, 26);
            gluSphere(sphere, 0.05 * (2.5 - abs(y - 1.75)) * (1.5 - abs(x - 5.0) / 2), 12, 3);
            glPopMatrix();
        }
    }
    glPopMatrix();// end balls

    /*****************************/

    // left
    glPushMatrix();// disk down
    glTranslatef(45, 0.01, 25);
    glRotatef(-90, 1, 0, 0);
    glColor3f(204 / 255.0, 255 / 255.0, 255 / 255.0);
    gluDisk(disk, 0, 2.5, 12, 3);
    glPopMatrix();// end disk down

    glPushMatrix();// disk up
    glTranslatef(45, 0.02, 25);
    glRotatef(-90, 1, 0, 0);
    glColor3f(1, 1, 1);
    if (stamp < 50)gluDisk(disk, 0, 2.0, 12, 3);
    else if (stamp < 100)gluDisk(disk, 0, 1.5, 12, 3);
    else gluDisk(disk, 0, 1.0, 12, 3);
    glPopMatrix();// end disk up

    glPushMatrix();// balls
    glColor3f(0, 0.8, 1);
    for (double x = 3.0; x <= 7.0; x += 1.0) {
        for (double y = 0.5; y <= 3; y += 0.5) {
            glPushMatrix();
            if (stamp < 100)glTranslatef(x + 40, y, 25);
            else glTranslatef(x + 40, y + 0.5, 26);
            gluSphere(sphere, 0.05 * (2.5 - abs(y - 1.75)) * (1.5 - abs(x - 5.0) / 2), 12, 3);
            glPopMatrix();
        }
    }
    glPopMatrix();// end balls
}

// motion and detection
void enter_teleportation() {// detection for enter the teleportation
    if (dist(tele_L, robot) - 0.5 < tele_L.size) {// left teleportation
        position[0] = 10;
        stand_posture();
    }
    if (dist(tele_R, robot) - 0.5 < tele_R.size) {// right teleportation
        position[0] = 40;
        stand_posture();
    }
}
void put_mine() {// robot put mine into analyzer
    stand_posture();
    // lift arm
    for (int i = 0; i < 45; i++) {
        arm_angle[1][0] += 2;
        display();
    }
    // mine fall
    mines[show_mine].dx = 40;
    mines[show_mine].dy = 6;
    mines[show_mine].dz = 45;
    mines[show_mine].show = 1;
    take_mine = -1;
    for (int i = 0; i < 45; i++) {
        mines[show_mine].dy -= 0.1;
        display();
    }
    mines[show_mine].show = 0;
    // fall arm
    for (int i = 0; i < 45; i++) {
        arm_angle[1][0] -= 2;
        display();
    }
    bubble_cnt = 0;
}
void detect_mine() {// detection for robot near the analyzer
    if (dist(robot, analyzer) < 5) {
        show_mine = take_mine;
        word_cnt = 0;
        first_line = 0;
        put_mine();
    }
}
double dist(node& a, node& b) {
    return sqrt((a.dx - b.dx) * (a.dx - b.dx) + (a.dz - b.dz) * (a.dz - b.dz));
}
void fall() {// robot out of floor range
    for (int i = 0; i < 30; i++) {// robot fall
        position[1] -= 2;
        display();
    }
    position[0] = position[2] = 25;
    position[1] = 2.7 - 0.3;
    robot.dx = robot.dy = 0;
    stand_posture();
    walk_start = 0;
    cur_step = step = 0;
    display();
}
int valid() {// detection overlapping for robot and obstacle
    if (robot.dx < 0 || robot.dx>50 || robot.dz < 0 || robot.dz>50) {// robot out of floor range
        fall();
        return -1;
    }
    for (int i = 0; i < rocks.size(); i++) {// robot and rock are overlapping
        if (dist(rocks[i], robot) - 0.9 < rocks[i].size)return 0;
    }
    for (int i = 0; i < mines.size(); i++) {// robot and mine are overlapping
        if (take_mine == i)continue;
        if (dist(mines[i], robot) - 0.9 < mines[i].size)return 0;
    }
    if (dist(analyzer, robot) - 2 < analyzer.size)return 0;// // robot and analyzer are overlapping
    return 1;// no overlapping
}
bool move(double dis) {// move for four direction
    if (rotation[1] == 0) position[2] += dis;
    else if (rotation[1] == 180) position[2] -= dis;
    else if (rotation[1] == 90) position[0] += dis;
    else if (rotation[1] == 270)position[0] -= dis;
    int v = valid();
    if (v == 0) {
        if (rotation[1] == 0) {
            position[0] += 0.2;
            position[2] += 0.2;
        }
        else if (rotation[1] == 180) {
            position[0] -= 0.2;
            position[2] -= 0.2;
        }
        else if (rotation[1] == 90) {
            position[0] += 0.2;
            position[2] += 0.2;
        }
        else if (rotation[1] == 270) {
            position[0] -= 0.2;
            position[2] -= 0.2;
        }
    }
    else if (v == -1)return false;
    enter_teleportation();// detection for enter the teleportation
    return true;
}
void stand_posture() {// set to initial stand posture
    memset(arm_angle, 0, sizeof(arm_angle));
    memset(elbow_angle, 0, sizeof(elbow_angle));
    memset(wrist_angle, 0, sizeof(wrist_angle));
    memset(leg_angle, 0, sizeof(leg_angle));
    memset(knee_angle, 0, sizeof(knee_angle));
    walk_start = 0;
    step = cur_step = 0;
}
void jump() {// procedure for jump
    // �ۤU�b�O
    stand_posture();
    walk_start = 0;
    for (int i = 0; i < 20; i++) {
        leg_angle[0] += 2;
        leg_angle[1] += 2;
        knee_angle[0] -= 4;
        knee_angle[1] -= 4;
        ankle_angle[0] += 2.0;
        ankle_angle[1] += 2.0;
        arm_angle[0][0] -= 1;
        arm_angle[1][0] -= 1;
        elbow_angle[0] += 1.5;
        elbow_angle[1] += 1.5;
        position[1] -= 0.1 / 5;
        display();
    }
    // �_��
    for (int i = 0; i < 20; i++) {
        leg_angle[0] -= 2;
        leg_angle[1] -= 2;
        knee_angle[0] += 4;
        knee_angle[1] += 4;
        ankle_angle[0] -= 2.0;
        ankle_angle[1] -= 2.0;
        arm_angle[0][0] += 1;
        arm_angle[1][0] += 1;
        elbow_angle[0] -= 1.5;
        elbow_angle[1] -= 1.5;
        position[1] += 0.5 / 5;
        display();
    }
    // �U�h(-8)
    for (int i = 1; i < 17; i++) {
        position[1] -= (0.5 / 16 * i * i - 0.5 / 16 * (i - 1) * (i - 1)) / 5;
        display();
    }
}
void walk() {// procedure for walk
    if (!walk_start) {
        // �u������(�L����)�~��}�l����
        while (cur_step < min(5, step)) {
            cur_step++;
            leg_angle[0] += 5;
            leg_angle[1] -= 5;
            knee_angle[0] -= 4;
            arm_angle[0][0] -= 2;
            arm_angle[1][0] += 2;
            elbow_angle[0] += 2.5;
            elbow_angle[1] += 2.5;
            if (!move(-0.24)) {
                stand_posture();
                return;
            }
            display();
        }
        if (cur_step == 5)walk_start = 1;

    }
    while (cur_step < min(10, step) && step >= 5) {
        cur_step++;
        leg_angle[0] -= 5;
        knee_angle[0] += 2;
        leg_angle[1] += 5;
        knee_angle[1] -= 2;
        arm_angle[0][0] += 2;
        arm_angle[1][0] -= 2;
        if (!move(-0.24)) {
            stand_posture();
            return;
        }
        display();
    }
    while (cur_step < min(15, step) && step >= 10) {
        cur_step++;
        leg_angle[0] -= 5;
        knee_angle[0] += 2;
        leg_angle[1] += 5;
        knee_angle[1] -= 2;
        arm_angle[0][0] += 2;
        arm_angle[1][0] -= 2;
        if (!move(-0.24)) {
            stand_posture();
            return;
        }
        display();
    }
    while (cur_step < min(20, step) && step >= 15) {
        cur_step++;
        leg_angle[0] += 5;
        knee_angle[0] -= 2;
        leg_angle[1] -= 5;
        knee_angle[1] += 2;
        arm_angle[0][0] -= 2;
        arm_angle[1][0] += 2;
        if (!move(-0.24)) {
            stand_posture();
            return;
        }
        display();
    }
    while (cur_step < min(25, step) && step >= 20) {
        cur_step++;
        leg_angle[0] += 5;
        knee_angle[0] -= 2;
        leg_angle[1] -= 5;
        knee_angle[1] += 2;
        arm_angle[0][0] -= 2;
        arm_angle[1][0] += 2;
        if (!move(-0.24)) {
            stand_posture();
            return;
        }
        display();
    }
}
void walk_stop() {// procedure for walk stop
    if (!walk_start) {
        double l0 = leg_angle[0], l1 = leg_angle[1];
        double k0 = knee_angle[0], k1 = knee_angle[1];
        double e0 = elbow_angle[0], e1 = elbow_angle[1];
        double a00 = arm_angle[0][0], a10 = arm_angle[1][0];
        double a01 = arm_angle[0][1], a11 = arm_angle[1][1];
        double a0 = ankle_angle[0], a1 = ankle_angle[1];
        double w0 = wrist_angle[0], w1 = wrist_angle[1];
        for (int i = 0; i < 20; i++) {
            leg_angle[0] -= l0 / 20;
            leg_angle[1] -= l1 / 20;
            knee_angle[0] -= k0 / 20;
            knee_angle[1] -= k1 / 20;
            arm_angle[0][0] -= a00 / 20;
            arm_angle[1][0] -= a10 / 20;
            arm_angle[0][1] -= a01 / 20;
            arm_angle[1][1] -= a11 / 20;
            elbow_angle[0] -= e0 / 20;
            elbow_angle[1] -= e1 / 20;
            ankle_angle[0] -= a0 / 20;
            ankle_angle[1] -= a1 / 20;
            wrist_angle[0] -= w0 / 20;
            wrist_angle[1] -= w1 / 20;
            display();
        }
        return;
    }
    walk_start = 0;
    double l0 = leg_angle[0], l1 = leg_angle[1];
    double k0 = knee_angle[0], k1 = knee_angle[1];
    double e0 = elbow_angle[0], e1 = elbow_angle[1];
    double a00 = arm_angle[0][0], a10 = arm_angle[1][0];
    double a01 = arm_angle[0][1], a11 = arm_angle[1][1];
    double a0 = ankle_angle[0], a1 = ankle_angle[1];
    double w0 = wrist_angle[0], w1 = wrist_angle[1];
    for (int i = 0; i < 20; i++) {
        leg_angle[0] -= l0 / 20;
        leg_angle[1] -= l1 / 20;
        knee_angle[0] -= k0 / 20;
        knee_angle[1] -= k1 / 20;
        arm_angle[0][0] -= a00 / 20;
        arm_angle[1][0] -= a10 / 20;
        arm_angle[0][1] -= a01 / 20;
        arm_angle[1][1] -= a11 / 20;
        elbow_angle[0] -= e0 / 20;
        elbow_angle[1] -= e1 / 20;
        ankle_angle[0] -= a0 / 20;
        ankle_angle[1] -= a1 / 20;
        wrist_angle[0] -= w0 / 20;
        wrist_angle[1] -= w1 / 20;
        move(-0.04);
        display();
    }
    step = cur_step = 0;
}
void run() {// procedure for run
    if (!walk_start) {
        // �u������(�L����)�~��}�l�]�B
        while (cur_step < min(5, step)) {
            cur_step++;
            leg_angle[0] += 12;
            leg_angle[1] -= 12;
            knee_angle[0] -= 16;
            arm_angle[0][0] -= 6;
            arm_angle[1][0] += 6;
            elbow_angle[0] += 15;
            elbow_angle[1] += 15;
            if (!move(-0.5)) {
                stand_posture();
                return;
            }
            display();
        }
        if (cur_step == 5)walk_start = 1;

    }
    while (cur_step < min(10, step) && step >= 5) {
        cur_step++;
        leg_angle[0] -= 12;
        knee_angle[0] += 8;
        leg_angle[1] += 12;
        knee_angle[1] -= 8;
        arm_angle[0][0] += 6;
        arm_angle[1][0] -= 6;
        if (!move(-0.5)) {
            stand_posture();
            return;
        }
        display();
    }
    while (cur_step < min(15, step) && step >= 10) {
        cur_step++;
        leg_angle[0] -= 12;
        knee_angle[0] += 8;
        leg_angle[1] += 12;
        knee_angle[1] -= 8;
        arm_angle[0][0] += 6;
        arm_angle[1][0] -= 6;
        if (!move(-0.5)) {
            stand_posture();
            return;
        }
        display();
    }
    while (cur_step < min(20, step) && step >= 15) {
        cur_step++;
        leg_angle[0] += 12;
        knee_angle[0] -= 8;
        leg_angle[1] -= 12;
        knee_angle[1] += 8;
        arm_angle[0][0] -= 6;
        arm_angle[1][0] += 6;
        if (!move(-0.5)) {
            stand_posture();
            return;
        }
        display();
    }
    while (cur_step < min(25, step) && step >= 20) {
        cur_step++;
        leg_angle[0] += 12;
        knee_angle[0] -= 8;
        leg_angle[1] -= 12;
        knee_angle[1] += 8;
        arm_angle[0][0] -= 6;
        arm_angle[1][0] += 6;
        if (!move(-0.5)) {
            stand_posture();
            return;
        }
        display();
    }
}
void grasp() {// procedure for grasp
    stand_posture();
    for (int i = 0; i < 35; i++) {// bend body
        rotation[0] -= 2;
        leg_angle[1] += 2;
        leg_angle[0] += 2;
        arm_angle[1][0] += 2;
        display();
    }
    if (take_mine != -1) {// put down mine on hand
        mines[take_mine].show = 1;
        mines[take_mine].dx = position[0];
        mines[take_mine].dz = position[2];
        take_mine = -1;
    }
    else {// grasp mine on floor
        for (int i = 0; i < mines.size(); i++) {
            if (dist(mines[i], robot) - 2 < mines[i].size) {
                mines[i].show = 0;
                take_mine = i;
                break;
            }
        }
    }
    display();
    for (int i = 0; i < 35; i++) {// bend body
        rotation[0] += 2;
        leg_angle[1] -= 2;
        leg_angle[0] -= 2;
        arm_angle[1][0] -= 2;
        display();
    }
}
void bend() {// procedure for bend
    stand_posture();
    for (int i = 0; i < 35; i++) {
        rotation[0] -= 2;
        leg_angle[1] += 2;
        leg_angle[0] += 2;
        display();
    }

    for (int i = 0; i < 35; i++) {
        rotation[0] += 2;
        leg_angle[1] -= 2;
        leg_angle[0] -= 2;
        display();
    }
}
void rotate(double angle) {// procedure for rotate
    stand_posture();
    for (int i = 0; i < 45; i++) {
        rotation[1] += angle * 2 / 90;
        display();
    }
    while (rotation[1] < 0)rotation[1] += 360;
    while (rotation[1] >= 360)rotation[1] -= 360;
}
GLfloat lc[4] = { 0.0,0.0,0.0,0.0 };// disco light color
void change_color() {
    // 讓顏色不會接近灰色的隨機改動
    do {
        int x = rand() % 3;
        lc[x] += 0.02;
        if (lc[x] > 1.0)lc[x] = 0;
    } while (fabs(lc[0] - lc[1]) + fabs(lc[1] - lc[2]) < 0.2);
}
void sun_light_setting() {
    if (sun_angle <= 140) {
        light_sun_diffuse[2] = 1.0 - sun_angle / 140.0;
        if (sun_angle < 60) {// decrease(1.0->0.4)
            light_sun_diffuse[0] = 1.0 - sun_angle / 60.0 * 0.6;
            light_sun_diffuse[1] = 1.0 - sun_angle / 60.0 * 0.6;
        }
        else if(sun_angle<=100){
            light_sun_diffuse[0] = 0.4 + (sun_angle-60) / 40 * 0.4;// increase(0.4->0.8)
            light_sun_diffuse[1] = 0.4 + (sun_angle-60) / 40 * 0.2;// increase(0.4->0.6)
        }
        else {
            light_sun_diffuse[0] = 0.8 - (sun_angle-100) / 40 * 0.8;// decrease(0.8->0.0)
            light_sun_diffuse[1] = 0.6 - (sun_angle-100) / 40 * 0.6;// decrease(0.6->0.0)
        }
    }
    else if(sun_angle>=220){
        double ta = 360 - sun_angle;
        light_sun_diffuse[0] = 1.0-ta / 140.0;
        light_sun_diffuse[1] = 1.0-ta / 140.0;
        light_sun_diffuse[2] = 1.0-ta / 140.0;
    }
    else {
        light_sun_diffuse[0] = 0;
        light_sun_diffuse[1] = 0;
        light_sun_diffuse[2] = 0;
    }
}
void meteor_move() {
    meteor_position[0] += 0.04;
    meteor_position[1] -= 0.02;
    meteor_position[2] += 0.05;
    if (meteor_position[0] >= 200) {
        meteor_position[0] = -100;
        meteor_position[1] = 120;
        meteor_position[2] = -30;
    }
}
void draw_shooting_star() {// meteor
    glPushMatrix();
        glPushMatrix();
            glScalef(0.5, 0.5, 0.5);
            meteor_move(); 
            {
                GLfloat mat_diffuse[] = { 0.1,0.1,0.1,1.0f };
                GLfloat mat_specular[] = { 0.05,0.05,0.05,1.0f };
                GLfloat mat_emission[] = { 0.8, 0.8, 1.0, 0.0 };
                glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
                glMaterialf(GL_FRONT, GL_SHININESS, 180);
                glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
            }
            glTranslatef(meteor_position[0], meteor_position[1], meteor_position[2]);
            glutSolidSphere(1.0, 20, 20);
            glPushMatrix();
                glRotatef(90, 0, 1, 0);
                glPushMatrix();
                    glTranslatef(0, 0, -15);
                    {
                        GLfloat mat_diffuse[] = { 0.02,0.02,0.02,1.0f };
                        GLfloat mat_specular[] = { 0.02,0.02,0.02,1.0f };
                        GLfloat mat_emission[] = { 0.5, 0.5, 0.2, 0.0 };
                        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
                        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
                        glMaterialf(GL_FRONT, GL_SHININESS, 120);
                        glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
                    }
                    gluCylinder(cylind, 0.0, 0.8, 15, 10, 10);
                glPopMatrix();
            glPopMatrix();
        glPopMatrix();
        glLightfv(GL_LIGHT4, GL_DIFFUSE, meteor_diffuse);
        glLightfv(GL_LIGHT4, GL_POSITION, meteor_position);
    glPopMatrix();
}
void draw_light() {
    // light source 1:disco light
    glPushMatrix();
        glTranslatef(25, 0.0, 25);
        glRotatef(lit_angle, 0.0, 1.0, 0.0);
        // Draw light source
        glPushMatrix();
            glTranslatef(disco_position[0], disco_position[1], disco_position[2]);
            glDisable(GL_LIGHTING);
            glDisable(GL_CULL_FACE);
            if (disco_on) {
                change_color();
                glColor3f(lc[0], lc[1], lc[2]);
            }
            else glColor3f(0.6,0.6,0.6);
            glutWireSphere(2, 8, 8);
        glPopMatrix();
        if (light_effect)glEnable(GL_LIGHTING);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lc);
        glLightfv(GL_LIGHT0, GL_POSITION, disco_position);
        glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, disco_direction);
    glPopMatrix();

    // light source 2:sun 
    glPushMatrix();
        glTranslatef(25, 0, 25);
        glPushMatrix();
            glRotatef(sun_angle, 0.0, 0.0, 1.0);
            glTranslatef(0, 35, 0);
            sun_light_setting();
            // Draw light source
            glPushMatrix();
                //glTranslatef(light_sun_position[0], light_sun_position[1], light_sun_position[2]);
                glDisable(GL_LIGHTING);
                glDisable(GL_CULL_FACE);
                glColor3f(1.0, 1.0, 0.0);
                glutSolidSphere(3, 12, 12);
            glPopMatrix();

            if (light_effect)glEnable(GL_LIGHTING);
            glLightfv(GL_LIGHT1, GL_DIFFUSE, light_sun_diffuse);
            glLightfv(GL_LIGHT1, GL_POSITION, light_sun_position);
        glPopMatrix();
    glPopMatrix();

    // light source 3:fluorescent lamp
    glPushMatrix();
        glTranslatef(10, 0, 10);
        // Draw light source
        glPushMatrix();
            glTranslatef(lamp_position[0], lamp_position[1], lamp_position[2]);
            GLfloat mat_diffuse[] = { 0.0f,0.50980392f,0.50980392f,1.0f };
            GLfloat mat_specular[] = { 0,0.50196078f,0.50196078f,1.0f };
            GLfloat mat_emission[] = { 0.0, 0.0, 0.1, 0.0 };
            glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
            glMaterialf(GL_FRONT, GL_SHININESS, 180);
            if(fluorescent_lamp_on)glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
            glutSolidSphere(3, 20, 20);
        glPopMatrix();
        if (light_effect)glEnable(GL_LIGHTING);
        glLightfv(GL_LIGHT2, GL_POSITION, lamp_position);
        glLightfv(GL_LIGHT2, GL_DIFFUSE, lamp_diffuse);
        glLightfv(GL_LIGHT2, GL_SPECULAR, lamp_specular);
    glPopMatrix();
    // light source 3:shooting star
    draw_shooting_star();
}
void draw_obstacle() {
    // Green plastic
    glPushMatrix();
    {
        glTranslatef(18, 3, 20);
        glRotatef(40, 0, 1, 0);
        glScalef(6.0, 6.0, 6.0);
        float mat_ambient[4] = { 0.0f,0.0f,0.0f,1.0f };
        float mat_diffuse[4] = { 0.1f,0.35f,0.1f,1.0f };
        float mat_specular[4] = { 0.45f,0.55f,0.45f,1.0f };
        float mat_shiness = 32.0f;
        draw_cube(mat_ambient, mat_specular, mat_diffuse, mat_shiness);
    }
    glPopMatrix();

    // Yellow rubber
    glPushMatrix();
    {
        glTranslatef(40, 3, 20);
        glRotatef(40, 0, 1, 0);
        glScalef(6.0, 6.0, 6.0);
        float mat_ambient[4] = { 0.05f,0.05f,0.0f,1.0f };
        float mat_diffuse[4] = { 0.5f,0.5f,0.4f,1.0f };
        float mat_specular[4] = { 0.7f,0.7f,0.04f,1.0f };
        float mat_shiness = 10.0f;
        draw_cube(mat_ambient, mat_specular, mat_diffuse, mat_shiness);
    }
    glPopMatrix();
    // Bronze
    glPushMatrix();
    {
        glTranslatef(20, 3, 35);
        glRotatef(0, 0, 1, 0);
        glScalef(6.0, 6.0, 6.0);
        float mat_ambient[4] = { 0.2125f, 0.1275f, 0.054f, 1.0f };
        float mat_diffuse[4] = { 0.714f, 0.4284f, 0.18144f, 1.0f };
        float mat_specular[4] = { 0.393548f, 0.271906f, 0.166721f, 1.0f };
        float mat_shiness = 25.6f;
        draw_cube(mat_ambient, mat_specular, mat_diffuse, mat_shiness);
    }
    glPopMatrix();
    // Jade
    glPushMatrix();
    {
        glTranslatef(10, 3, 35);
        glRotatef(30, 0, 1, 0);
        glScalef(6.0, 6.0, 6.0);
        float mat_ambient[4] = { 0.135f, 0.2225f, 0.1575f, 0.95f };
        float mat_diffuse[4] = { 0.54f, 0.89f, 0.63f, 0.95f };
        float mat_specular[4] = { 0.316228f, 0.316228f, 0.316228f, 0.95f };
        float mat_shiness = 12.8f;
        draw_cube(mat_ambient, mat_specular, mat_diffuse, mat_shiness);
    }
    glPopMatrix();
}
// procedure to draw scene and robot, setting variables, setting light sources
void draw_scene() {
    if (bubble_cnt != -1 && bubble_cnt < 160) {
        if (bubble_cnt % 16 == 0)
            bubble_special_effects();
        bubble_cnt++;
    }
    if (bubble_cnt == 160)bubble_cnt = -1;
    sun_angle += 0.05;
    if (sun_angle >= 360)sun_angle -= 360;
    moveto_flashlight();
    draw_floor();
    draw_obstacle();
    //draw_analyzer();
    draw_rocks();
    draw_mines();
    draw_robot();
    draw_teleportation();
    //show_mine_info();
    word_cnt++;
    stamp++;
    stamp %= 200;
}

// Procedure to make viewing matrix
void make_view(int x) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    switch (x) {
    case 4:       // Perspective
        gluLookAt(eye[0], eye[1], eye[2],
            eye[0] - u[2][0], eye[1] - u[2][1], eye[2] - u[2][2],
            u[1][0], u[1][1], u[1][2]);
        break;

    case 1:       // X direction parallel viewing
        gluLookAt(30.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
        break;
    case 2:       // Y direction parallel viewing
        gluLookAt(0.0, 30.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0);
        break;
    case 3:       // Z direction parallel viewing
        gluLookAt(0.0, 0.0, 30.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
        break;
    }

}
// Procedure to make projection matrix
void make_projection(int x) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // define a view volume
    if (x == 4) {
        gluPerspective(fovy, aspect, zNear, zFar);
    }
    else {
        if (width > height)
            glOrtho(ncw[nl], ncw[nr], ncw[nb],
                ncw[nt],
                ncw[nn], ncw[nf]);
        else
            glOrtho(ncw[nl],
                ncw[nr], ncw[nb], ncw[nt],
                ncw[nn], ncw[nf]);
    }
    glMatrixMode(GL_MODELVIEW);
}
// Procedure to draw view volume
void draw_view_volume() {
    double nearx, neary, nearz = zNear, farx, fary, farz = zFar;
    double radian = 0.5 * fovy * acos(-1) / 180.0;
    neary = nearz * tan(radian);
    nearx = neary * aspect;
    farx = nearx * zFar / zNear;
    fary = neary * zFar / zNear;
    farz = nearz * zFar / zNear;
    glMatrixMode(GL_MODELVIEW);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glBegin(GL_LINES);
    glVertex3f(nearx, neary, nearz);
    glVertex3f(nearx, -neary, nearz);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(nearx, -neary, nearz);
    glVertex3f(-nearx, -neary, nearz);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(-nearx, -neary, nearz);
    glVertex3f(-nearx, neary, nearz);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(-nearx, neary, nearz);
    glVertex3f(nearx, neary, nearz);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(-farx, fary, farz);
    glVertex3f(farx, fary, farz);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(farx, fary, farz);
    glVertex3f(farx, -fary, farz);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(farx, -fary, farz);
    glVertex3f(-farx, -fary, farz);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(-farx, -fary, farz);
    glVertex3f(-farx, fary, farz);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(nearx, neary, nearz);
    glVertex3f(farx, fary, farz);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(-nearx, neary, nearz);
    glVertex3f(-farx, fary, farz);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(nearx, -neary, nearz);
    glVertex3f(farx, -fary, farz);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(-nearx, -neary, nearz);
    glVertex3f(-farx, -fary, farz);
    glEnd();

    glColor4f(1.0, 1.0, 1.0, 0.2);
    glBegin(GL_POLYGON);
    glVertex3f(-farx, fary, farz);
    glVertex3f(farx, fary, farz);
    glVertex3f(farx, -fary, farz);
    glVertex3f(-farx, -fary, farz);
    glEnd();
}
// Procedure to draw axis
void draw_view() {
    glMatrixMode(GL_MODELVIEW);

    // Draw Eye position

    glPushMatrix();
    glTranslatef(eye[0], eye[1], eye[2]);

    glRotatef(eyeAngx, 1, 0, 0);
    glRotatef(eyeAngy, 0, 1, 0);
    glRotatef(eyeAngz, 0, 0, 1);

    draw_view_volume();

    glColor3f(0.0, 1.0, 0.0);
    glutWireSphere(1.0, 10, 10);
    glPopMatrix();

    // Draw eye coord. axes
    glColor3f(1.0, 1.0, 0.0);           // Draw Xe
    glBegin(GL_LINES);
    glVertex3f(eye[0], eye[1], eye[2]);
    glVertex3f(eye[0] + 20.0 * u[0][0], eye[1] + 20.0 * u[0][1], eye[2] + 20.0 * u[0][2]);
    glEnd();

    glColor3f(1.0, 0.0, 1.0);          // Draw Ye
    glBegin(GL_LINES);
    glVertex3f(eye[0], eye[1], eye[2]);
    glVertex3f(eye[0] + 20.0 * u[1][0], eye[1] + 20.0 * u[1][1], eye[2] + 20.0 * u[1][2]);
    glEnd();

    glColor3f(0.0, 1.0, 1.0);          // Draw Ze
    glBegin(GL_LINES);
    glVertex3f(eye[0], eye[1], eye[2]);
    glVertex3f(eye[0] + 20.0 * u[2][0], eye[1] + 20.0 * u[2][1], eye[2] + 20.0 * u[2][2]);
    glEnd();
}

void display() {
    // Clear previous frame and the depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    switch (style) {
    case 0:// 4 view port in a window

        make_view(4);
        make_projection(4);
        glViewport(width / 2, 0, width / 2, height / 2);
        draw_light();
        draw_scene();

        make_view(1);
        make_projection(1);
        glViewport(0, height / 2, width / 2, height / 2);
        draw_light();
        draw_scene();
        //make_view(1);
        //draw_view();

        make_view(2);
        make_projection(2);//
        glViewport(width / 2, height / 2, width / 2, height / 2);
        draw_light();
        draw_scene();
        //make_view(2);
        //draw_view();

        make_view(3);
        make_projection(3);//
        glViewport(0, 0, width / 2, height / 2);
        draw_light();
        draw_scene();
        //make_view(3);
        //draw_view();
        break;

    case 4:// only perspective projection
        glViewport(0, 0, width, height);
        draw_light();
        make_view(4);
        make_projection(4);
        draw_scene();
        break;
    case 1:// only orthographical projection(x-axis)
        glViewport(0, 0, width, height);
        draw_light();
        make_view(1);
        make_projection(1);
        draw_scene();
        break;
    case 2:// only orthographical projection(y-axis)
        glViewport(0, 0, width, height);
        draw_light();
        make_view(2);
        make_projection(2);
        draw_scene();
        break;
    case 3:// only orthographical projection(z-axis)
        glViewport(0, 0, width, height);
        draw_light();
        make_view(3);
        make_projection(3);
        draw_scene();
        break;
    }
    glutSwapBuffers();
}
void my_reshape(int w, int h) {
    width = w;
    height = h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-40.0, 40.0, -40, 40, 0.0, 120.0);

}
void my_keyboard(unsigned char key, int x_, int y_) {
    int i;
    float  x[3], y[3], z[3];
    if (key == 'Q' || key == 'q')
        exit(0);
    /*
    if (key == 'W' || key == 'w')LR = 0;
    else if (key == 'E' || key == 'e')LR = 1;
    else if (key == 'a') {
        arm_angle[LR][0] += 1.0;
        if (arm_angle[LR][0] == 360)arm_angle[LR][0] = 0;
        walk_start = 0;
    }
    else if (key == 'A') {
        arm_angle[LR][0] -= 1.0;
        if (arm_angle[LR][0] < 0)arm_angle[LR][0] += 360;
        walk_start = 0;
    }
    else if (key == 's') {
        if (!LR && arm_angle[LR][1] < 170)arm_angle[LR][1] += 1.0;
        else if (LR && arm_angle[LR][1] > -170)arm_angle[LR][1] -= 1.0;
        walk_start = 0;
    }
    else if (key == 'S') {
        if (!LR && arm_angle[LR][1] > 0)arm_angle[LR][1] -= 1.0;
        else if (LR && arm_angle[LR][1] < 0)arm_angle[LR][1] += 1.0;
        walk_start = 0;
    }
    else if (key == 'd' && elbow_angle[LR] < 120)
        elbow_angle[LR] += 1.0;
    else if (key == 'D' && elbow_angle[LR] > 0)
        elbow_angle[LR] -= 1.0;
    else if (key == 'f' && wrist_angle[LR] < 90) {
        wrist_angle[LR] += 1.0;
        walk_start = 0;
    }
    else if (key == 'F' && wrist_angle[LR] > -90) {
        wrist_angle[LR] -= 1.0;
        walk_start = 0;
    }
    else if (key == 'g' && finger_angle[LR] < 35) finger_angle[LR] += 1.0;
    else if (key == 'G' && finger_angle[LR] > 0) finger_angle[LR] -= 1.0;
    else if (key == 'h' && leg_angle[LR] < 90) {
        leg_angle[LR] += 1.0;
        walk_start = 0;
    }
    else if (key == 'H' && leg_angle[LR] > 0) {
        leg_angle[LR] -= 1.0;
        walk_start = 0;
    }
    else if (key == 'j' && knee_angle[LR] > -90) {
        knee_angle[LR] -= 1.0;
        walk_start = 0;
    }
    else if (key == 'J' && knee_angle[LR] < 0) {
        knee_angle[LR] += 1.0;
        walk_start = 0;
    }
    else if (key == 'k')head_angle += 1.0;
    else if (key == 'K')head_angle -= 1.0;
    */
    if (key == 'z' || key == 'Z') {
        step++;
        if (key == 'z') {
            if (walk_run) {
                walk_run = 0;
                stand_posture();
                cur_step = step = 0;
            }
            walk();
        }
        else {
            if (!walk_run) {
                walk_run = 1;
                stand_posture();
                cur_step = step = 0;
            }
            run();
        }
        if (cur_step == 25) {
            cur_step = step = 5;
        }
    }
    else if (key == 'x' || key == 'X')
        walk_stop();
    else if (key == 'l')rotate(90);
    else if (key == 'L')rotate(-90);
    else if (key == 'c' || key == 'C')jump();
    else if (key == 'v' || key == 'V')bend();
    else if (key == 'b' || key == 'B')grasp();
    else if (key == 'n' || key == 'N') {
        if (take_mine == -1)return;
        detect_mine();
    }
    else if (key == 'r') {// move up
        eyeDy += 0.5;
        for (i = 0; i < 3; i++) eye[i] += 0.5 * u[1][i];
    }
    else if (key == 'R') {// move down
        eyeDy += -0.5;
        for (i = 0; i < 3; i++) eye[i] -= 0.5 * u[1][i];
    }
    else if (key == 't') {// move left
        eyeDx += -0.5;
        for (i = 0; i < 3; i++) eye[i] += 0.5 * u[0][i];
    }
    else if (key == 'T') {// move right
        eyeDx += 0.5;
        for (i = 0; i < 3; i++) eye[i] -= 0.5 * u[0][i];
    }
    else if (key == 'y') {// move forward
        eyeDz += 0.5;
        for (i = 0; i < 3; i++) eye[i] -= 0.5 * u[2][i];
    }
    else if (key == 'Y') {// move backward
        eyeDz += -0.5;
        for (i = 0; i < 3; i++) eye[i] += 0.5 * u[2][i];
    }
    else if (key == 'u') {// pitching
        eyeAngx += 5.0;
        if (eyeAngx > 360.0) eyeAngx -= 360.0;
        y[0] = u[1][0] * cv - u[2][0] * sv;
        y[1] = u[1][1] * cv - u[2][1] * sv;
        y[2] = u[1][2] * cv - u[2][2] * sv;

        z[0] = u[2][0] * cv + u[1][0] * sv;
        z[1] = u[2][1] * cv + u[1][1] * sv;
        z[2] = u[2][2] * cv + u[1][2] * sv;

        for (i = 0; i < 3; i++) {
            u[1][i] = y[i];
            u[2][i] = z[i];
        }
    }
    else if (key == 'U') {
        eyeAngx += -5.0;
        if (eyeAngx < 0.0) eyeAngx += 360.0;
        y[0] = u[1][0] * cv + u[2][0] * sv;
        y[1] = u[1][1] * cv + u[2][1] * sv;
        y[2] = u[1][2] * cv + u[2][2] * sv;

        z[0] = u[2][0] * cv - u[1][0] * sv;
        z[1] = u[2][1] * cv - u[1][1] * sv;
        z[2] = u[2][2] * cv - u[1][2] * sv;

        for (i = 0; i < 3; i++) {
            u[1][i] = y[i];
            u[2][i] = z[i];
        }
    }
    else if (key == 'i') {// heading
        eyeAngy += 5.0;
        if (eyeAngy > 360.0) eyeAngy -= 360.0;
        for (i = 0; i < 3; i++) {
            x[i] = cv * u[0][i] - sv * u[2][i];
            z[i] = sv * u[0][i] + cv * u[2][i];
        }
        for (i = 0; i < 3; i++) {
            u[0][i] = x[i];
            u[2][i] = z[i];
        }
    }
    else if (key == 'I') {
        eyeAngy += -5.0;
        if (eyeAngy < 0.0) eyeAngy += 360.0;
        for (i = 0; i < 3; i++) {
            x[i] = cv * u[0][i] + sv * u[2][i];
            z[i] = -sv * u[0][i] + cv * u[2][i];
        }
        for (i = 0; i < 3; i++) {
            u[0][i] = x[i];
            u[2][i] = z[i];
        }
    }
    else if (key == 'o') {// rolling
        eyeAngz += 5.0;
        if (eyeAngz > 360.0) eyeAngz -= 360.0;
        for (i = 0; i < 3; i++) {
            x[i] = cv * u[0][i] - sv * u[1][i];
            y[i] = sv * u[0][i] + cv * u[1][i];
        }
        for (i = 0; i < 3; i++) {
            u[0][i] = x[i];
            u[1][i] = y[i];
        }
    }
    else if (key == 'O') {
        eyeAngz += -5.0;
        if (eyeAngz < 0.0) eyeAngz += 360.0;
        for (i = 0; i < 3; i++) {
            x[i] = cv * u[0][i] + sv * u[1][i];
            y[i] = -sv * u[0][i] + cv * u[1][i];
        }
        for (i = 0; i < 3; i++) {
            u[0][i] = x[i];
            u[1][i] = y[i];
        }
    }
    else if (key == 'p') {// zoom in
        if (fovy > 10) {
            fovy /= 1.1;
            ncw[nl] += 2;
            ncw[nr] -= 2;
            ncw[nb] += 2;
            ncw[nt] -= 2;
        }
    }
    else if (key == 'P') {// zoom out
        if (fovy * 1.1 < 180) {
            fovy *= 1.1;
            ncw[nl] -= 2;
            ncw[nr] += 2;
            ncw[nb] -= 2;
            ncw[nt] += 2;
        }
    }
    else if ((key - '0') >= 0 && (key - '0') <= 4)
        style = key - '0';
    else if (key == ' ') lit_angle += 5.0;
    else if (key == '5') {// disco
        glEnable(GL_LIGHT0);
        disco_on = 1;
    }
    else if (key == '6') {// disco
        glDisable(GL_LIGHT0);
        disco_on = 0;
    }
    else if (key == '7')glEnable(GL_LIGHT1);// sun
    else if (key == '8')glDisable(GL_LIGHT1);// sun
    else if (key == 'a') {// lamp
        glEnable(GL_LIGHT2);
        fluorescent_lamp_on = 1;
    }
    else if (key == 'A') {// lamp
        glDisable(GL_LIGHT2);
        fluorescent_lamp_on = 0;
    }
    else if (key == 'd') {// flashlight
        glEnable(GL_LIGHT3);
        flashlight_on = 1;
    }
    else if (key == 'D') {// flashlight
        glDisable(GL_LIGHT3);
        flashlight_on = 0;
    }
    else if (key == 's') {// lamp up
        if (lamp_diffuse[0] * 1.1 <= 1.0 && lamp_diffuse[1] * 1.1 <= 1.0 && lamp_diffuse[2] * 1.1 <= 1.0) {
            lamp_diffuse[0] *= 1.1;
            lamp_diffuse[1] *= 1.1;
            lamp_diffuse[2] *= 1.1;
        }
    }
    else if (key == 'S') {// lamp down
        if (lamp_diffuse[0] / 1.1 >=0.01 && lamp_diffuse[1] / 1.1 >= 0.01 && lamp_diffuse[2] / 1.1 >= 0.01) {
            lamp_diffuse[0] /= 1.1;
            lamp_diffuse[1] /= 1.1;
            lamp_diffuse[2] /= 1.1;
        }
    }
    else if (key == 'f') {// flashlight up
        if (flashlight_diffuse[0] * 1.1 <= 1.0 && flashlight_diffuse[1] * 1.1 <= 1.0 && flashlight_diffuse[2] * 1.1 <= 1.0) {
            flashlight_diffuse[0] *= 1.1;
            flashlight_diffuse[1] *= 1.1;
            flashlight_diffuse[2] *= 1.1;
        }
    }
    else if (key == 'F') {// flashlight down
        if (flashlight_diffuse[0] / 1.1 >= 0.01 && flashlight_diffuse[1] / 1.1 >= 0.01 && flashlight_diffuse[2] / 1.1 >= 0.01) {
            flashlight_diffuse[0] /= 1.1;
            flashlight_diffuse[1] /= 1.1;
            flashlight_diffuse[2] /= 1.1;
        }
    }
    else if (key == 'g') {// flashlight increase angle
        if (flashlight_cutoff * 1.1 <= 90)flashlight_cutoff *= 1.1;
    }
    else if (key == 'G') {// flashlight decrease angle
        if (flashlight_cutoff / 1.1 >= 15)flashlight_cutoff /= 1.1;
    }
    else if (key == 'h') flashlight_dir = 0;
    else if (key == 'H') flashlight_dir = 1;
    else if (key == 'j') {
        light_effect = 1;
        glEnable(GL_LIGHTING);
    }
    else if (key == 'J') {
        light_effect = 0;
        glDisable(GL_LIGHTING);
    }
    display();
}
void idle_func() {
    display();
}

void myinit() {
    glClearColor(0.0, 0.0, 0.0, 1.0); // set the background color BLACK 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the Depth & Color Buffers
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_NORMALIZE);   // Enable mornalization
    glShadeModel(GL_SMOOTH);
    
    glEnable(GL_LIGHT0);      // Turn on light0
    glEnable(GL_LIGHT1);      // Turn on light1 
    glEnable(GL_LIGHT2);      // Turn on light2 
    glEnable(GL_LIGHT3);      // Turn on light3 
    glEnable(GL_LIGHT4);      // Turn on light4 
    glEnable(GL_LIGHTING);    // Enable lighting effects

    // Define light0
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, disco_cutoff);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, disco_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, disco_specular);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, disco_exponent);

    // Define light1
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_sun_diffuse);
    //glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, light_sun_cutoff);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_sun_specular);
    //glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 1);

    // Define light2
    glLightfv(GL_LIGHT2, GL_DIFFUSE, lamp_diffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, lamp_specular);

    // Define light3
    glLightfv(GL_LIGHT3, GL_DIFFUSE, flashlight_diffuse);
    glLightfv(GL_LIGHT3, GL_SPECULAR, flashlight_specular);
    glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, flashlight_exponent);
    glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, flashlight_direction[flashlight_dir]);
    glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, flashlight_cutoff);
    glLightfv(GL_LIGHT3, GL_POSITION, flashlight_position);


    // Define some global lighting status
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); /* local viewer */
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient); /*global ambient*/

    // Enable face culling
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glFlush(); // Enforce window system display the results
    if (cylind == NULL) {
        cylind = gluNewQuadric();
        gluQuadricDrawStyle(cylind, GLU_FILL);
        gluQuadricNormals(cylind, GLU_SMOOTH);
    }
    if (disk == NULL) {
        disk = gluNewQuadric();
        gluQuadricDrawStyle(disk, GLU_FILL);
        gluQuadricNormals(disk, GLU_SMOOTH);
    }
    if (sphere == NULL) {
        sphere = gluNewQuadric();
        gluQuadricDrawStyle(sphere, GLU_FILL);
    }
    tele_R = node(2.5, 5, 0.01, 25, 1, 1, 1);
    tele_L = node(2.5, 45, 0.01, 25, 1, 1, 1);
    analyzer = node(2, 40, 0.5, 45, 0.6, 0.6, 0.6);
    /*---- Compute cos(5.0) and sin(5.0) ----*/
    cv = cos(5.0 * pi / 180.0);
    sv = sin(5.0 * pi / 180.0);
    // Copy eye position
    eye[0] = Eye[0];
    eye[1] = Eye[1];
    eye[2] = Eye[2];
}
void main(int argc, char** argv) {
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow("robot");

    myinit(); // Initialize other state varibales

    glutDisplayFunc(display);
    glutIdleFunc(idle_func);
    glutReshapeFunc(my_reshape);
    glutKeyboardFunc(my_keyboard);

    glutMainLoop();
}
