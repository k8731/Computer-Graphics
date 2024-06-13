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
#include <stb_image.h>
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

// light source:sun
float  light_sun_position[] = { 35, 30, 0,0 };// directional light
float  light_sun_direction[] = { 0.0, -1.0, 0.0, 0.0 };
float  light_sun_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
float  light_sun_specular[] = { 0.1, 0.1, 0.1, 1.0 };

// light source angle
float  sun_angle = 0.0;
// light on control
bool light_effect = 1;

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
    node(double sz, double x, double y, double z, double r_, double g_, double b_, string s, float ambient_[], float specular_[], float diffuse_[], float shiness_) {
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
vector<node>rocks;
node robot;
// initial variables
int step = 0, cur_step = 0;
int LR = 0;
int take_mine = -1, show_mine = -1;
bool first = 1;
bool walk_start = 0;
bool walk_run = 0;// walk:0 run:1

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
// for texture mapping
int image_width, image_height, nrChannels;
unsigned char* image_data;
unsigned int TextureID[10];
unsigned int Texture_gif[36];// for UFO gif
// pre declare
void display();
void stand_posture();
double dist(node& a, node& b);
void bind_texture(int index) {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_GEN_S); 
    glEnable(GL_TEXTURE_GEN_T);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    //glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    //glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glBindTexture(GL_TEXTURE_2D, TextureID[index]);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
}
void close_bind() {
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
}
void draw_sphere(float mat_ambient[], float mat_specular[], float mat_diffuse[], GLfloat mat_shininess, double radius, int slice, int stack, int texture) {// draw a unit cube
    // Define some material properties
    bind_texture(texture);
    /*
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    if (mat_specular[0] + mat_specular[1] + mat_specular[2] > 0)glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    if (mat_shininess > 0)glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
    */
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexCoord2f(0, 0);
    gluSphere(sphere, radius, slice, stack);
    close_bind();
}
void draw_cube(float mat_ambient[], float mat_specular[], float mat_diffuse[], GLfloat mat_shininess,int texture) {// draw a unit cube
    // Define some material properties
    bind_texture(texture);
    /*
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
    */
    for (int i = 0; i < 6; i++) {
        glNormal3fv(normal[i]);
        glBegin(GL_POLYGON); // Draw the face
        glTexCoord2f(0.25, 0.25);
        glVertex3fv(points[face[i][0]]);
        glTexCoord2f(0.25, 0.75);
        glVertex3fv(points[face[i][1]]);
        glTexCoord2f(0.75, 0.75);
        glVertex3fv(points[face[i][2]]);
        glTexCoord2f(0.75, 0.25);
        glVertex3fv(points[face[i][3]]);
        glEnd();
    }
    close_bind();
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
    draw_sphere(rock_ambient, rock_specular, rock_diffuse, rock_shininess, x.size, 12, 12,1);
    glPopMatrix();
}
void draw_floor() {// draw floor
    // [0,0,0] ~ [50,0,50]
    // Define floor properties
    /***********************************/
    bind_texture(0);
    /****************************************/
    /*
    glMaterialfv(GL_FRONT, GL_AMBIENT, floor_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, floor_diffuse);
    */
    glPushMatrix();
    glTranslatef(25, -1, 25);
    glScalef(50, 2, 50);
    glColor3fv(floor_diffuse);
    for (int i = 0; i < 6; i++) {
        glNormal3fv(normal[i]);
        glBegin(GL_POLYGON);
        glTexCoord2f(0.25, 0.25);
        glVertex3fv(points[face[i][0]]);
        glTexCoord2f(0.25, 0.75);
        glVertex3fv(points[face[i][1]]);
        glTexCoord2f(0.75, 0.75);
        glVertex3fv(points[face[i][2]]);
        glTexCoord2f(0.75, 0.25);
        glVertex3fv(points[face[i][3]]);
        glEnd();
    }
    glPopMatrix();
    close_bind();
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
    draw_sphere(joint_ambient, joint_specular, joint_diffuse, joint_shininess, 0.3, 12, 12,3);
    glPopMatrix();// end left joint1

    // draw left up arm
    glPushMatrix();
    glTranslatef(0, 0.7, 0.0);// move to left arm middle
    glScalef(0.4, 0.8, 0.4);
    glColor3f(arm_color[0], arm_color[1], arm_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess,2);
    glPopMatrix();// end left up arm

    // draw left joint2
    glTranslatef(0.0, 1.3, 0.0);// move to left elbow coord
    glRotatef(elbow_angle[0], 1, 0, 0);
    glPushMatrix();
    glColor3f(joint_color[0], joint_color[1], joint_color[2]);
    draw_sphere(joint_ambient, joint_specular, joint_diffuse, joint_shininess, 0.2, 12, 12,3);
    glPopMatrix();// end left joint2

    // draw left down arm
    glPushMatrix();
    glTranslatef(0.0, 0.45, 0.0);
    glScalef(0.3, 0.5, 0.3);
    glColor3f(arm_color[0], arm_color[1], arm_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess,2);
    glPopMatrix();// end left down arm

    // draw left joint3
    glTranslatef(0.0, 0.85, 0.0);// move to left wrist coord sys
    glRotatef(wrist_angle[0], 1, 0, 0);
    glPushMatrix();
    glColor3f(joint_color[0], joint_color[1], joint_color[2]);
    draw_sphere(joint_ambient, joint_specular, joint_diffuse, joint_shininess, 0.15, 12, 12,3);
    glPopMatrix();// end left joint3


    // draw left finger
    glPushMatrix();
    glRotatef(-finger_angle[0], 0, 0, 1);
    glTranslatef(0.06, 0.2, 0);
    glScalef(0.12, 0.3, 0.12);
    glColor3f(finger_color[0], finger_color[1], finger_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess,2);
    glPopMatrix();// end left finger
    // draw right finger
    glPushMatrix();
    glRotatef(finger_angle[0], 0, 0, 1);
    glTranslatef(-0.06, 0.2, 0);
    glScalef(0.12, 0.3, 0.12);
    glColor3f(finger_color[0], finger_color[1], finger_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess,2);
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
    draw_sphere(joint_ambient, joint_specular, joint_diffuse, joint_shininess, 0.3, 12, 12,3);
    glPopMatrix();// end right joint1

    // draw right up arm
    glPushMatrix();
    glTranslatef(0, 0.7, 0.0);// move to right arm middle
    glScalef(0.4, 0.8, 0.4);
    glColor3f(arm_color[0], arm_color[1], arm_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess,2);
    glPopMatrix();// end right up arm

    // draw right joint2
    glTranslatef(0.0, 1.3, 0.0);// move to right elbow coord
    glRotatef(elbow_angle[1], 1, 0, 0);
    glPushMatrix();
    glColor3f(joint_color[0], joint_color[1], joint_color[2]);
    draw_sphere(joint_ambient, joint_specular, joint_diffuse, joint_shininess, 0.2, 12, 12,3);
    glPopMatrix();// end right joint2

    // draw right down arm
    glPushMatrix();
    glTranslatef(0.0, 0.45, 0.0);
    glScalef(0.3, 0.5, 0.3);
    glColor3f(arm_color[0], arm_color[1], arm_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess,2);
    glPopMatrix();// end right down arm

    // draw right joint3
    glTranslatef(0.0, 0.85, 0.0);// move to right wrist coord sys
    glRotatef(wrist_angle[1], 1, 0, 0);
    glPushMatrix();
    glColor3f(joint_color[0], joint_color[1], joint_color[2]);
    draw_sphere(joint_ambient, joint_specular, joint_diffuse, joint_shininess, 0.15, 12, 12,3);
    glPopMatrix();// end right joint3

    // draw left finger
    glPushMatrix();
    glRotatef(-finger_angle[1], 0, 0, 1);
    glTranslatef(0.06, 0.2, 0);
    glScalef(0.12, 0.3, 0.12);
    glColor3f(finger_color[0], finger_color[1], finger_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess,2);
    glPopMatrix();// end left finger
    // draw right finger
    glPushMatrix();
    glRotatef(finger_angle[1], 0, 0, 1);
    glTranslatef(-0.06, 0.2, 0);
    glScalef(0.12, 0.3, 0.12);
    glColor3f(finger_color[0], finger_color[1], finger_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess,2);
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
    draw_sphere(joint_ambient, joint_specular, joint_diffuse, joint_shininess, 0.3, 12, 12,3);
    glPopMatrix();// end left joint1

    // draw left up leg
    glPushMatrix();
    glTranslatef(0, 0.7, 0.0);// move to left leg middle
    glScalef(0.5, 0.8, 0.5);
    glColor3f(arm_color[0], arm_color[1], arm_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess,2);
    glPopMatrix();// end left up leg

    // draw left joint2
    glTranslatef(0.0, 1.3, 0.0);// move to left knee coord
    glRotatef(knee_angle[0], 1, 0, 0);
    glPushMatrix();
    glColor3f(joint_color[0], joint_color[1], joint_color[2]);
    draw_sphere(joint_ambient, joint_specular, joint_diffuse, joint_shininess, 0.2, 12, 12,3);
    glPopMatrix();// end left joint2

    // draw left down leg
    glPushMatrix();
    glTranslatef(0.0, 0.45, 0.0);
    glScalef(0.4, 0.5, 0.4);
    glColor3f(arm_color[0], arm_color[1], arm_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess,2);
    glPopMatrix();// end left down leg

    // draw left joint3
    glTranslatef(0.0, 0.85, 0.0);// move to left ankle coord sys
    glRotatef(ankle_angle[0], 1, 0, 0);
    glPushMatrix();
    glColor3f(joint_color[0], joint_color[1], joint_color[2]);
    draw_sphere(joint_ambient, joint_specular, joint_diffuse, joint_shininess, 0.2, 12, 12,3);
    glPopMatrix();// end left joint3

    // draw left foot
    glPushMatrix();
    glTranslatef(0.0, 0.1, 0.35);
    glScalef(0.4, 0.2, 0.4);
    glColor3f(arm_color[0], arm_color[1], arm_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess,2);
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
    draw_sphere(joint_ambient, joint_specular, joint_diffuse, joint_shininess, 0.3, 12, 12,3);
    glPopMatrix();// end right joint1

    // draw right up leg
    glPushMatrix();
    glTranslatef(0, 0.7, 0.0);// move to right leg middle
    glScalef(0.5, 0.8, 0.5);
    glColor3f(arm_color[0], arm_color[1], arm_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess,2);
    glPopMatrix();// end right up leg

    // draw right joint2
    glTranslatef(0.0, 1.3, 0.0);// move to right knee coord
    glRotatef(knee_angle[1], 1, 0, 0);
    glPushMatrix();
    glColor3f(joint_color[0], joint_color[1], joint_color[2]);
    draw_sphere(joint_ambient, joint_specular, joint_diffuse, joint_shininess, 0.2, 12, 12,3);
    glPopMatrix();// end right joint2

    // draw right down leg
    glPushMatrix();
    glTranslatef(0.0, 0.45, 0.0);
    glScalef(0.4, 0.5, 0.4);
    glColor3f(arm_color[0], arm_color[1], arm_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess,2);
    glPopMatrix();// end right down leg

    // draw right joint3
    glTranslatef(0.0, 0.85, 0.0);// move to right ankle coord sys
    glRotatef(ankle_angle[1], 1, 0, 0);
    glPushMatrix();
    glColor3f(joint_color[0], joint_color[1], joint_color[2]);
    draw_sphere(joint_ambient, joint_specular, joint_diffuse, joint_shininess, 0.2, 12, 12,3);
    glPopMatrix();// end right joint3

    // draw right foot
    glPushMatrix();
    glTranslatef(0.0, 0.1, 0.35);
    glScalef(0.4, 0.2, 0.4);
    glColor3f(arm_color[0], arm_color[1], arm_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess,2);
    glPopMatrix();// end right foot

    glPopMatrix();// end right leg
}
void draw_left_eye() {
    bind_texture(4);
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
    bind_texture(4);
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
        first = 0;
    }
    // draw all rocks
    for (int i = 0; i < rocks.size(); i++)draw_a_rock(rocks[i]);
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
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess,2);
    glPopMatrix();

    // draw neck and head
    glPushMatrix();
    // draw neck (neck coord sys)
    glTranslatef(0, 3.0 + 0.3 + 0.3, 0); // move to neck coord sys
    glPushMatrix();
    glScalef(0.5, 0.6, 0.5);
    glColor3f(joint_color[0], joint_color[1], joint_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess,3);
    glPopMatrix();// end neck

    // draw head (head coord sys)
    glTranslatef(0.0, 0.3 + 0.5, 0.0); // move to head coord sys
    glRotatef(head_angle, 0, 1, 0);
    glPushMatrix();
    glScalef(2.5, 0.8, 2);
    glColor3f(body_color[0], body_color[1], body_color[2]);
    draw_cube(robot_ambient, robot_specular, robot_diffuse, robot_shininess,2);
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
void sun_light_setting() {
    if (sun_angle <= 140) {
        light_sun_diffuse[2] = 1.0 - sun_angle / 140.0;
        if (sun_angle < 60) {// decrease(1.0->0.4)
            light_sun_diffuse[0] = 1.0 - sun_angle / 60.0 * 0.6;
            light_sun_diffuse[1] = 1.0 - sun_angle / 60.0 * 0.6;
        }
        else if (sun_angle <= 100) {
            light_sun_diffuse[0] = 0.4 + (sun_angle - 60) / 40 * 0.4;// increase(0.4->0.8)
            light_sun_diffuse[1] = 0.4 + (sun_angle - 60) / 40 * 0.2;// increase(0.4->0.6)
        }
        else {
            light_sun_diffuse[0] = 0.8 - (sun_angle - 100) / 40 * 0.8;// decrease(0.8->0.0)
            light_sun_diffuse[1] = 0.6 - (sun_angle - 100) / 40 * 0.6;// decrease(0.6->0.0)
        }
    }
    else if (sun_angle >= 220) {
        double ta = 360 - sun_angle;
        light_sun_diffuse[0] = 1.0 - ta / 140.0;
        light_sun_diffuse[1] = 1.0 - ta / 140.0;
        light_sun_diffuse[2] = 1.0 - ta / 140.0;
    }
    else {
        light_sun_diffuse[0] = 0;
        light_sun_diffuse[1] = 0;
        light_sun_diffuse[2] = 0;
    }
}
void draw_light() {
    // light source 2:sun 
    glPushMatrix();
    glTranslatef(25, 0, 25);
    glPushMatrix();
    /****/
    sun_angle = 0;
    glRotatef(sun_angle, 0.0, 0.0, 1.0);
    glTranslatef(0, 35, 0);
    sun_light_setting();

    // Draw light source
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glColor3f(1.0, 1.0, 0.0);
    glutSolidSphere(3, 12, 12);
    glPopMatrix();
    if (light_effect) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT1);
    }
    else {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT1);
    }
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_sun_diffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, light_sun_position);
    glPopMatrix();
    glPopMatrix();
}
float  mtx[16];
float  a[3], b[3];
void compute_ab_axes() {
    float  w0, w2;
    double len;
    /*----Get w0 and w2 from the modelview matrix mtx[] ---*/
    w0 = mtx[2]; w2 = mtx[10];
    len = sqrt(w0 * w0 + w2 * w2);
    /*---- Define the a and b axes for billboards ----*/
    b[0] = 0.0; b[1] = 1.0; b[2] = 0.0;
    a[0] = w2 / len; a[1] = 0.0; a[2] = -w0 / len;
}
void draw_billboard(float x, float z, float w, float h) {
    float  v0[3], v1[3], v2[3], v3[3];
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    /*----Compute the 4 vertices of the billboard ----*/
    v0[0] = x - (w / 2) * a[0]; v0[1] = 0.0; v0[2] = z - (w / 2) * a[2];
    v1[0] = x + (w / 2) * a[0]; v1[1] = 0.0; v1[2] = z + (w / 2) * a[2];
    v2[0] = x + (w / 2) * a[0]; v2[1] = h; v2[2] = z + (w / 2) * a[2];
    v3[0] = x - (w / 2) * a[0]; v3[1] = h; v3[2] = z - (w / 2) * a[2];
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3fv(v3);
    glTexCoord2f(1.0, 0.0); glVertex3fv(v2);
    glTexCoord2f(1.0, 1.0); glVertex3fv(v1);
    glTexCoord2f(0.0, 1.0); glVertex3fv(v0);
    glEnd();
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
}
bool UFO_coming = 0;
float UFO_time = 0;
float monster_index[10];
void build_monster_index() {
    for (int i = 0; i < 10; i++) {
        float x = (rand() % 20)/10;
        monster_index[i] = 18+x;
    }
}
void draw_monster() {
    glGetFloatv(GL_MODELVIEW_MATRIX, mtx);
    compute_ab_axes();
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, TextureID[5]);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();

    int num = min(10,(int)(UFO_time - 1000) / 250);
    for (int i = 0; i < min(num,60); i++)
        draw_billboard(10+i*2, monster_index[i], 5, 5.0);
    glDisable(GL_TEXTURE_2D);
}
void draw_UFO() {
    int index = min((int)UFO_time / 125,35);
    glGetFloatv(GL_MODELVIEW_MATRIX, mtx);
    compute_ab_axes();
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, Texture_gif[index]);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    if (UFO_time <= 1000)draw_billboard(10, UFO_time / 50.0, 10, 20);
    else if (UFO_time <= 3500)draw_billboard(10, 20, 8, 16);
    else draw_billboard(10, 20 - (UFO_time - 3500) / 50.0, 10, 20);
    glDisable(GL_TEXTURE_2D);
}
void draw_sky_dome() {
    bind_texture(7);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexCoord2f(0, 0);
    glPushMatrix();
    glTranslatef(25, 0, 25);
    gluSphere(sphere, 40, 100, 100);
    glPopMatrix();
    close_bind();
}
int fog_time = 0;
float fog_color[] = { 219 / 255.0, 157 / 255.0, 0, 0.05 };
int fog_mode = 0;
bool fog_show = 1;
void setting_fog() {
    
    if (fog_time % 150 == 0) {
        int x = 215 + rand() % 10;
        fog_color[0] = x / 255.0;
        x = 155 + rand() % 5;
        fog_color[1] = x / 255.0;
        glFogf(GL_FOG_DENSITY, 0.5);
        glFogf(GL_FOG_START, 1);
        x = 55 + rand() % 10;
        glFogf(GL_FOG_END, x);
        glFogfv(GL_FOG_COLOR, fog_color);
    }
    glDisable(GL_FOG);
    if(fog_show)glEnable(GL_FOG);
    if(fog_mode==0)glFogi(GL_FOG_MODE, GL_LINEAR);
    else if(fog_mode == 1)glFogi(GL_FOG_MODE, GL_EXP);
    else glFogi(GL_FOG_MODE, GL_EXP2);
    
}
// procedure to draw scene and robot, setting variables, setting light sources
void draw_scene() {
    sun_angle += 0.05;
    if (sun_angle >= 360)sun_angle -= 360;
    draw_floor();
    draw_rocks();
    draw_robot();
    if (UFO_coming) {
        if (UFO_time == 10000) {
            //UFO_coming = 0;
        }
        else {
            UFO_time++;
            if(UFO_time<=4500)draw_UFO();
            if(UFO_time>=1000)draw_monster();
        }
    }
    draw_sky_dome();
    fog_time++;
    fog_time %= 10000;
    setting_fog();
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
// Procedure to draw axis
void draw_view() {
    glMatrixMode(GL_MODELVIEW);

    // Draw Eye position
    glPushMatrix();
    glTranslatef(eye[0], eye[1], eye[2]);

    glRotatef(eyeAngx, 1, 0, 0);
    glRotatef(eyeAngy, 0, 1, 0);
    glRotatef(eyeAngz, 0, 0, 1);

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
    else if (key == 'j') {
        if (light_effect)return;
        light_effect = 1;
        glEnable(GL_LIGHT1);// sun
        glEnable(GL_LIGHTING);
    }
    else if (key == 'J') {
        if (!light_effect)return;
        light_effect = 0;
        glDisable(GL_LIGHT1);// sun
        glDisable(GL_LIGHTING);
    }
    else if (key == 'k') UFO_coming = 1;
    if (key == 'b') {
        glEnable(GL_FOG);
        fog_show = 1;
    }
    else if (key == 'B') {
        glDisable(GL_FOG);
        fog_show = 0;
    }
    else if (key == '7')fog_mode = 0;
    else if (key == '8')fog_mode = 1;
    else if (key == '9')fog_mode = 2;
    display();
}
void idle_func() {
    display();
}
void setting_texture(int index) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glBindTexture(GL_TEXTURE_2D, TextureID[index]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    if (nrChannels == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
}
void setting_gif_texture() {
    for (int i = 0; i < 36; i++) {
        string s = "image/UFO/UFO ("+to_string(i+1)+").png";
        char* filename = const_cast<char*>(s.c_str());
        image_data = stbi_load(filename, &image_width, &image_height, &nrChannels, 0);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glBindTexture(GL_TEXTURE_2D, Texture_gif[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        if (nrChannels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    }
}
void myinit() {
    glClearColor(0.0, 0.0, 0.0, 1.0); // set the background color BLACK 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the Depth & Color Buffers
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_NORMALIZE);   // Enable mornalization
    glShadeModel(GL_SMOOTH);
    
    glEnable(GL_LIGHT1);      // Turn on light1 
    glEnable(GL_LIGHTING);    // Enable lighting effects

    // Define light1
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_sun_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_sun_specular);

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
    /*---- Compute cos(5.0) and sin(5.0) ----*/
    cv = cos(5.0 * pi / 180.0);
    sv = sin(5.0 * pi / 180.0);
    // Copy eye position
    eye[0] = Eye[0];
    eye[1] = Eye[1];
    eye[2] = Eye[2];

    // texture mapping
    build_monster_index();
    glGenTextures(10, TextureID);
    glGenTextures(36, Texture_gif);
    // texture0: soil
    image_data = stbi_load("image/soil.jpg", &image_width, &image_height, &nrChannels, 0);
    setting_texture(0);
    // texture1: rock
    image_data = stbi_load("image/rock.jpg", &image_width, &image_height, &nrChannels, 0);
    setting_texture(1);
    // texture2: blue metal
    image_data = stbi_load("image/metal_blue.jpg", &image_width, &image_height, &nrChannels, 0);
    setting_texture(2);
    // texture3: white metal
    image_data = stbi_load("image/metal_white.jpg", &image_width, &image_height, &nrChannels, 0);
    setting_texture(3);
    // texture4: black
    image_data = stbi_load("image/black.png", &image_width, &image_height, &nrChannels, 0);
    setting_texture(4);
    // texture5: monster
    image_data = stbi_load("image/monster.png", &image_width, &image_height, &nrChannels, 0);
    setting_texture(5);
    // texture6: UFO
    image_data = stbi_load("image/UFO.png", &image_width, &image_height, &nrChannels, 0);
    setting_texture(6);
    // texture7: space
    image_data = stbi_load("image/space.png", &image_width, &image_height, &nrChannels, 0);
    setting_texture(7);

    setting_gif_texture();

    // setting fog
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_DENSITY, 0.5);
    glFogf(GL_FOG_START, 1);
    glFogf(GL_FOG_END, 60.0);
    glFogfv(GL_FOG_COLOR, fog_color);
}
void main(int argc, char** argv) {
    srand(time(NULL));
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