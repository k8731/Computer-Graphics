/*
    Author:00957144
    This program simulates the robot(motion and animation).
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
// color setting
double joint_color[3] = { 38.0/255,127.0/255,217.0/255 };
double body_color[3] = { 132.0/255,193.0/255,255.0/255 };
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
double position[3] = { 25,2.7-0.3,25 };// robot position
double rotation[3] = { 0,0,0 };// robot body angle

#define LEFT 0
#define RIGHT 1
#define Step  0.5

int width = 676, height =676; // window size

struct node {// struct to represent items
    double dx, dy, dz, size,r,g,b;
    bool show = 1;
    string name = "";
    node() { dx = dy = dz = size = r=g=b=0; }
    node(double sz, double x, double y, double z,double r_,double g_,double b_) {
        dx = x, dy = y, dz = z, size = sz, r = r_, g = g_, b = b_;
        if (r_ > 1)r = r_ / 255.0, g = g_ / 255.0, b = b_ / 255.0;
    }
    node(double sz, double x, double y, double z, double r_, double g_, double b_,string s) {
        dx = x, dy = y, dz = z, size = sz, r = r_, g = g_, b = b_, name = s;
        if (r_ > 1)r = r_ / 255.0, g = g_ / 255.0, b = b_ / 255.0;
    }
};
vector<node>rocks,mines;
node robot,tele_L,tele_R, analyzer;
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
double fovy = 40, aspect = (double)width / (double)height, zNear = 5, zFar = 35.0;

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
void draw_cube() {// draw a unit cube
    for (int i = 0; i < 6; i++) {
        glBegin(GL_POLYGON); // Draw the face
        glVertex3fv(points[face[i][0]]);
        glVertex3fv(points[face[i][1]]);
        glVertex3fv(points[face[i][2]]);
        glVertex3fv(points[face[i][3]]);
        glEnd();
    }
}
void draw_a_rock(node &x) {// draw a rock or mine
    glColor3f(x.r, x.g, x.b);
    glPushMatrix();
    glTranslatef(x.dx, x.dy, x.dz);
    gluSphere(sphere, x.size, 12, 12);
    glPopMatrix();
}
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
        draw_cube();
    glPopMatrix();// end outer screen
    // draw inner screen
    glPushMatrix();
        glTranslatef(40, 10, 54.5);
        glPushMatrix();// draw cube
            glScalef(14.5, 9.5, 1);
            glColor3f(0, 0, 0.15);
            draw_cube();
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
void draw_floor() {// draw floor
    // [0,0,0] ~ [50,0,50]
    glColor3f(151 / 255.0, 124 / 255.0, 0);
    glPushMatrix();
    glTranslatef(25, -1, 25);
    glScalef(50, 2, 50);
    for (int i = 0; i < 6; i++) {
        if (i >= 3)glColor3f(151 / 255.0, 124 / 255.0, 0);
        else glColor3f(100 / 255.0, 90 / 255.0, 0);
        glBegin(GL_POLYGON);
        glVertex3fv(points[face[i][0]]);
        glVertex3fv(points[face[i][1]]);
        glVertex3fv(points[face[i][2]]);
        glVertex3fv(points[face[i][3]]);
        glEnd();
    }
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
            gluSphere(sphere, 0.3, 12, 12);
        glPopMatrix();// end left joint1

        // draw left up arm
        glPushMatrix();
            glTranslatef(0, 0.7, 0.0);// move to left arm middle
            glScalef(0.4, 0.8, 0.4);
            glColor3f(arm_color[0], arm_color[1], arm_color[2]);
            draw_cube();
        glPopMatrix();// end left up arm

        // draw left joint2
        glTranslatef(0.0, 1.3, 0.0);// move to left elbow coord
        glRotatef(elbow_angle[0], 1, 0, 0);
        glPushMatrix();
            glColor3f(joint_color[0], joint_color[1], joint_color[2]);
            gluSphere(sphere, 0.2, 12, 12);
        glPopMatrix();// end left joint2

        // draw left down arm
        glPushMatrix();
            glTranslatef(0.0, 0.45, 0.0);
            glScalef(0.3, 0.5, 0.3);
            glColor3f(arm_color[0], arm_color[1], arm_color[2]);
            draw_cube();
        glPopMatrix();// end left down arm

        // draw left joint3
        glTranslatef(0.0, 0.85, 0.0);// move to left wrist coord sys
        glRotatef(wrist_angle[0], 1, 0, 0);
        glPushMatrix();
            glColor3f(joint_color[0], joint_color[1], joint_color[2]);
            gluSphere(sphere, 0.15, 12, 12);
        glPopMatrix();// end left joint3

        // draw left finger
        glPushMatrix();
            glRotatef(-finger_angle[0], 0, 0, 1);
            glTranslatef(0.06, 0.2, 0);
            glScalef(0.12, 0.3, 0.12);
            glColor3f(finger_color[0], finger_color[1], finger_color[2]);
            draw_cube();
        glPopMatrix();// end left finger
        // draw right finger
        glPushMatrix();
            glRotatef(finger_angle[0], 0, 0, 1);
            glTranslatef(-0.06, 0.2, 0);
            glScalef(0.12, 0.3, 0.12);
            glColor3f(finger_color[0], finger_color[1], finger_color[2]);
            draw_cube();
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
    gluSphere(sphere, 0.3, 12, 12);
    glPopMatrix();// end right joint1

    // draw right up arm
    glPushMatrix();
    glTranslatef(0, 0.7, 0.0);// move to right arm middle
    glScalef(0.4, 0.8, 0.4);
    glColor3f(arm_color[0], arm_color[1], arm_color[2]);
    draw_cube();
    glPopMatrix();// end right up arm

    // draw right joint2
    glTranslatef(0.0, 1.3, 0.0);// move to right elbow coord
    glRotatef(elbow_angle[1], 1, 0, 0);
    glPushMatrix();
    glColor3f(joint_color[0], joint_color[1], joint_color[2]);
    gluSphere(sphere, 0.2, 12, 12);
    glPopMatrix();// end right joint2

    // draw right down arm
    glPushMatrix();
    glTranslatef(0.0, 0.45, 0.0);
    glScalef(0.3, 0.5, 0.3);
    glColor3f(arm_color[0], arm_color[1], arm_color[2]);
    draw_cube();
    glPopMatrix();// end right down arm

    // draw right joint3
    glTranslatef(0.0, 0.85, 0.0);// move to right wrist coord sys
    glRotatef(wrist_angle[1], 1, 0, 0);
    glPushMatrix();
    glColor3f(joint_color[0], joint_color[1], joint_color[2]);
    gluSphere(sphere, 0.15, 12, 12);
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
    draw_cube();
    glPopMatrix();// end left finger
    // draw right finger
    glPushMatrix();
    glRotatef(finger_angle[1], 0, 0, 1);
    glTranslatef(-0.06, 0.2, 0);
    glScalef(0.12, 0.3, 0.12);
    glColor3f(finger_color[0], finger_color[1], finger_color[2]);
    draw_cube();
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
    gluSphere(sphere, 0.3, 12, 12);
    glPopMatrix();// end left joint1

    // draw left up leg
    glPushMatrix();
    glTranslatef(0, 0.7, 0.0);// move to left leg middle
    glScalef(0.5, 0.8, 0.5);
    glColor3f(arm_color[0], arm_color[1], arm_color[2]);
    draw_cube();
    glPopMatrix();// end left up leg

    // draw left joint2
    glTranslatef(0.0, 1.3, 0.0);// move to left knee coord
    glRotatef(knee_angle[0], 1, 0, 0);
    glPushMatrix();
    glColor3f(joint_color[0], joint_color[1], joint_color[2]);
    gluSphere(sphere, 0.2, 12, 12);
    glPopMatrix();// end left joint2

    // draw left down leg
    glPushMatrix();
    glTranslatef(0.0, 0.45, 0.0);
    glScalef(0.4, 0.5, 0.4);
    glColor3f(arm_color[0], arm_color[1], arm_color[2]);
    draw_cube();
    glPopMatrix();// end left down leg

    // draw left joint3
    glTranslatef(0.0, 0.85, 0.0);// move to left ankle coord sys
    glRotatef(ankle_angle[0], 1, 0, 0);
    glPushMatrix();
    glColor3f(joint_color[0], joint_color[1], joint_color[2]);
    gluSphere(sphere, 0.2, 12, 12);
    glPopMatrix();// end left joint3

    // draw left foot
    glPushMatrix();
    glTranslatef(0.0, 0.1, 0.35);
    glScalef(0.4, 0.2, 0.4);
    glColor3f(arm_color[0], arm_color[1], arm_color[2]);
    draw_cube();
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
    gluSphere(sphere, 0.3, 12, 12);
    glPopMatrix();// end right joint1

    // draw right up leg
    glPushMatrix();
    glTranslatef(0, 0.7, 0.0);// move to right leg middle
    glScalef(0.5, 0.8, 0.5);
    glColor3f(arm_color[0], arm_color[1], arm_color[2]);
    draw_cube();
    glPopMatrix();// end right up leg

    // draw right joint2
    glTranslatef(0.0, 1.3, 0.0);// move to right knee coord
    glRotatef(knee_angle[1], 1, 0, 0);
    glPushMatrix();
    glColor3f(joint_color[0], joint_color[1], joint_color[2]);
    gluSphere(sphere, 0.2, 12, 12);
    glPopMatrix();// end right joint2

    // draw right down leg
    glPushMatrix();
    glTranslatef(0.0, 0.45, 0.0);
    glScalef(0.4, 0.5, 0.4);
    glColor3f(arm_color[0], arm_color[1], arm_color[2]);
    draw_cube();
    glPopMatrix();// end right down leg

    // draw right joint3
    glTranslatef(0.0, 0.85, 0.0);// move to right ankle coord sys
    glRotatef(ankle_angle[1], 1, 0, 0);
    glPushMatrix();
    glColor3f(joint_color[0], joint_color[1], joint_color[2]);
    gluSphere(sphere, 0.2, 12, 12);
    glPopMatrix();// end right joint3

    // draw right foot
    glPushMatrix();
    glTranslatef(0.0, 0.1, 0.35);
    glScalef(0.4, 0.2, 0.4);
    glColor3f(arm_color[0], arm_color[1], arm_color[2]);
    draw_cube();
    glPopMatrix();// end right foot

    glPopMatrix();// end right leg
}
void draw_left_eye() {
    // draw left eye
    glPushMatrix();// cylinder
    glTranslatef(-0.8, 0, 0);
    glRotatef(-180, 1, 0, 0);
    glColor3f(1, 1, 1);
    gluCylinder(cylind, 0.2, 0.2, 0.05, 12, 3);
    glPopMatrix();

    glPushMatrix();// disk
    glTranslatef(-0.8, 0, -0.05);
    glColor3f(0, 0, 0);
    gluDisk(disk, 0, 0.2, 12, 3);
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
    if (first) {
        first = 0;
        mines.push_back(node(0.4, 18, 0.2, 48, 203, 251, 251, "Diamond"));
        mines.push_back(node(0.5, 25, 0.2, 20, 255, 209, 5, "Gold"));
        mines.push_back(node(0.3, 35, 0.2, 38, 18, 18, 217, "Lapis Lazuli"));
        mines.push_back(node(0.5, 47, 0.2, 2, 117, 177, 27, "Olivine"));
        mines.push_back(node(0.3, 7, 0.1, 45, 247, 182, 182, "Rhodonite"));
        mines.push_back(node(0.4, 24, 0.2, 13, 244, 143, 42, "Amber"));
        mines.push_back(node(0.5, 41, 0.2, 19, 229, 36, 36, "Ruby"));
        mines.push_back(node(0.4, 11, 0.2, 13, 223, 236, 236, "Opal"));
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
    draw_cube();
    glPopMatrix();

    // draw neck and head
    glPushMatrix();
    // draw neck (neck coord sys)
        glTranslatef(0, 3.0 + 0.3 + 0.3, 0); // move to neck coord sys
        glPushMatrix();
            glScalef(0.5, 0.6, 0.5);
            glColor3f(joint_color[0], joint_color[1], joint_color[2]);
            draw_cube();
        glPopMatrix();// end neck

        // draw head (head coord sys)
        glTranslatef(0.0, 0.3 + 0.5, 0.0); // move to head coord sys
        glRotatef(head_angle, 0, 1, 0);
        glPushMatrix();
            glScalef(2.5, 0.8, 2);
            glColor3f(body_color[0], body_color[1], body_color[2]);
            draw_cube();
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
    double dx, dy, dz, sz,g,b,factor;
    for (int i = 0; i < 3; i++) {// three bubble, random position, random color
        glPushMatrix();
        dx = (41.5 - 38.5) * rand() / (RAND_MAX + 1.0) + 38.5;
        dz = (46.5 - 43.5) * rand() / (RAND_MAX + 1.0) + 43.5;
        factor= (0.1 - (-0.1)) * rand() / (RAND_MAX + 1.0);
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
        if(stamp<50)gluDisk(disk, 0, 2.0, 12, 3);
        else if(stamp<100)gluDisk(disk, 0, 1.5, 12, 3);
        else gluDisk(disk, 0, 1.0, 12, 3);
    glPopMatrix();// end disk up

    glPushMatrix();// balls
        glColor3f(0, 0.8, 1);
        for (double x = 3.0; x <= 7.0; x += 1.0) {
            for (double y = 0.5; y <= 3; y += 0.5) {
                glPushMatrix();
                if (stamp < 100)glTranslatef(x, y, 25);
                else glTranslatef(x, y+0.5, 26);
                gluSphere(sphere, 0.05*(2.5-abs(y-1.75))*(1.5-abs(x-5.0)/2), 12, 3);
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
            if(stamp<100)glTranslatef(x + 40, y, 25);
            else glTranslatef(x + 40, y+0.5, 26);
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
    // 蹲下畜力
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
    // 起跳
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
    // 下去(-8)
    for (int i = 1; i < 17; i++) {
        position[1] -= (0.5 / 16 * i * i - 0.5 / 16 * (i - 1) * (i - 1)) / 5;
        display();
    }
}
void walk() {// procedure for walk
    if (!walk_start) {
        // 只有直立(無角度)才能開始走路
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
        // 只有直立(無角度)才能開始跑步
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

// procedure to draw scene and robot, setting variables
void draw_scene() {

    if (bubble_cnt != -1 && bubble_cnt < 160) {
        if (bubble_cnt % 16 == 0)
            bubble_special_effects();
        bubble_cnt++;
    }
    if (bubble_cnt == 160)bubble_cnt = -1;
    draw_floor();
    draw_analyzer();
    draw_rocks();
    draw_mines();
    draw_robot();
    draw_teleportation();
    show_mine_info();
    word_cnt++;
    stamp++;
    stamp %= 200;
}

// Procedure to make viewing matrix
void make_view(int x){
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
void make_projection(int x){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // define a view volume
    if (x == 4) {
        gluPerspective(fovy, aspect, zNear, zFar);
    }
    else {
        /*
        if (width > height)
            glOrtho(-40.0, 40.0, -40.0 * (float)height / (float)width,
                40.0 * (float)height / (float)width,
                -0.0, 100.0);
        else
            glOrtho(-40.0 * (float)width / (float)height,
                40.0 * (float)width / (float)height, -40.0, 40.0,
                -0.0, 100.0);
        */
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
void draw_view(){
    glMatrixMode(GL_MODELVIEW);

    // Draw Eye position
    
    glPushMatrix();
    /*
    double mtx[16];
    mtx[0] = u[0][0];
    mtx[1] = u[0][1];
    mtx[2] = u[0][2];
    mtx[3] = 0.0;
    mtx[4] = u[1][0];
    mtx[5] = u[1][1];
    mtx[6] = u[1][2];
    mtx[7] = 0.0;
    mtx[8] = u[2][0];
    mtx[9] = u[2][1];
    mtx[10] = u[2][2];
    mtx[11] = 0.0;
    mtx[12] = 0.0;
    mtx[13] = 0.0;
    mtx[14] = 0.0;
    mtx[15] = 1.0;
    
    glMultMatrixd(mtx);
    */
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
        draw_scene();

        make_view(1);
        make_projection(1);
        glViewport(0, height / 2, width / 2, height / 2);
        draw_scene();
        make_view(1);
        draw_view();

        make_view(2);
        glViewport(width / 2, height / 2, width / 2, height / 2);
        draw_scene();
        make_view(2);
        draw_view();

        make_view(3);
        glViewport(0, 0, width / 2, height / 2);
        draw_scene();
        make_view(3);
        draw_view();
        break;

    case 4:// only perspective projection
        glViewport(0, 0, width, height);
        make_view(4);
        make_projection(4);
        draw_scene();
        break;
    case 1:// only orthographical projection(x-axis)
        glViewport(0, 0, width, height);
        make_view(1);
        make_projection(1);
        draw_scene();
        break;
    case 2:// only orthographical projection(y-axis)
        glViewport(0, 0, width, height);
        make_view(2);
        make_projection(2);
        draw_scene();
        break;
    case 3:// only orthographical projection(z-axis)
        glViewport(0, 0, width, height);
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
void my_keyboard(unsigned char key,int x_,int y_) {
    int i;
    float  x[3], y[3], z[3];
    if (key == 'Q' || key == 'q')
        exit(0);
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
    else if (key == 'd' && elbow_angle[LR] <120)
        elbow_angle[LR] += 1.0;
    else if (key == 'D' && elbow_angle[LR]>0)
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
    else if (key == 'z' || key == 'Z') {
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
        for (i = 0; i < 3; i++) eye[i] +=0.5 * u[0][i];
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
        if (fovy > 20) {
            fovy /= 1.1;
            ncw[nl] -= 2;
            ncw[nr] += 2;
            ncw[nb] -= 2;
            ncw[nt] += 2;
        }

    }
    else if (key == 'P') {// zoom out
        if (fovy * 1.1 < 90) {
            fovy *= 1.1;
            ncw[nl] += 2;
            ncw[nr] -= 2;
            ncw[nb] += 2;
            ncw[nt] -= 2;
        }
    }
    else if ((key - '0') >= 0 && (key - '0') <= 4) {
        style = key - '0';
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
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    /*
    glViewport(0, 0, width, height);
    // Set a parallel projection mode
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-40.0, 40.0, -40, 40, 0.0, 120.0);
    glEnable(GL_DEPTH_TEST); // Enable depth buffer for shading computing
    */
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
