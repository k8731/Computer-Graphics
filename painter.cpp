/* 
    Author:00957144
    This program simulates the painter.
    Illustrates the the fundamental instructions such as handling mouse, keyboeard events,
    menu buttons, and drawing. 
 */
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h><gl/gl.h><gl/glu.h>WINGDIAPIAPIENTRY
#include <iostream>
#include <string>
#include <ctime>
using namespace std;

#define SIZEX 2000
#define SIZEY 2000
// others
#define MY_QUIT 1
#define MY_CLEAR 2
// file
#define MY_SAVE 1
#define MY_LOAD 2
// color
#define BLACK 1
#define RED 2
#define GREEN 3
#define BLUE 4
#define YELLOW 5
#define PURPLE 6
#define CYAN 7

// fundamental type
#define POINT 1
#define LINE 2
#define POLYGON 3
#define CIRCLE 4
#define CURVE 5
#define TRIANGLE 6
#define POLYLINES 7
#define CHAR 8
#define STRING 9

// advanced type
#define PEN 10
#define ERASER 11
#define BLUR 12
#define SPRAY1 13
#define SPRAY2 14
#define BUBBLE 15

#define BITMAP_TIMES_ROMAN_10 1
#define BITMAP_TIMES_ROMAN_24 2
#define BITMAP_8_BY_13 3
#define BITMAP_9_BY_15 4
#define BITMAP_HELVETICA_10 5
#define BITMAP_HELVETICA_12 6
#define BITMAP_HELVETICA_18 7

#define POINT_MODE 1
#define LINE_MODE 2
#define FILL 3
#define SILHOUETTE 4

#define BIGGER 1
#define SMALLER 2

#define SHOW_GRID 1
#define HIDE_GRID 0

typedef int menu_t;
menu_t top_m, color_m,  type_m,size_m,setting_m,other_m,advanced_m;
menu_t file_m, save_m, load_m;
menu_t style_m, font_m, fill_m;

/*
            +--- color_m
            |
            +--- type_m
            |
            +--- size_m
            |
            |              +-- font_m
    top_m --+--- style_m --+ 
            |              +-- fill_m   
            |
            +--- setting_m
            |              
            |              +-- save_m
            +--- file_m  --+
            |              +-- load_m
            |
            +--- other_m
                           
                           
            
*/

int height = 800, width = 1000;
unsigned char image[8][SIZEX * SIZEY][4]; // Image data in main memory

bool not_empty[8] = { 1,1,0,0,0,0,1,1 };
// image[0,5,6]:system temp image 
// image[1]:show image
// image[2~4]:user image
int pos_x = -1, pos_y = -1;
int px = -1, py = -1;
float myColor[3] = { 0, 0, 0 };
int obj_type = -1;
int font = 2;// initial
int polygon_mode = 3;
int first = 0;      // flag:initial points for lines and curve
int vertex[128][2]; // vertice array
int side = 0;       // num of sides of polygon
float pnt_size = 1.0;// point size
float line_width = 1.0;// line width
float cir_size = 10.0;// circle size

int offset = 20;// grid line offset
bool first_load = 1;
bool show_grid = 1;// show or hide grid
float grid_color = 0.8;

int file_id = 1;
string str;
// button state
bool press_font = 0;// close
bool press_save = 0;// close
bool press_load = 0;// close
void init_draw();
// choose polygon mode func
void choose_poly_mode() {
    switch (polygon_mode) {
    case POINT_MODE:
        glPolygonMode(GL_FRONT, GL_POINT);
        glPolygonMode(GL_BACK, GL_POINT);
        break;
    case LINE_MODE:
        glPolygonMode(GL_FRONT, GL_LINE);
        glPolygonMode(GL_BACK, GL_LINE);
        break;
    case FILL:
        glPolygonMode(GL_FRONT, GL_FILL);
        glPolygonMode(GL_BACK, GL_FILL);
    case SILHOUETTE:// polygon doesn't have this!
        glPolygonMode(GL_FRONT, GL_FILL);
        glPolygonMode(GL_BACK, GL_FILL);
    default:
        break;
    }
}

// Procedure to draw a polygon
void draw_polygon(double r, double g, double b) {
    choose_poly_mode();
    glColor3f(r,g,b);
    glLineWidth(line_width);
    glPointSize(pnt_size);
    glBegin(GL_POLYGON);
    for (int i = 0; i < side; i++)
        glVertex2f(vertex[i][0], height - vertex[i][1]);
    glEnd();
    glFinish();
    side = 0;
}
// Procedure to draw a triangle
void draw_triangle() {
    choose_poly_mode();
    glColor3f(myColor[0], myColor[1], myColor[2]);
    glLineWidth(line_width);
    glPointSize(pnt_size);
    glBegin(GL_TRIANGLES);
    //glColor3f(1, 0, 0);
    glVertex2f(vertex[0][0], height - vertex[0][1]);
    //glColor3f(0, 1, 0);
    glVertex2f(vertex[1][0], height - vertex[1][1]);
    //glColor3f(0, 0, 1);
    glVertex2f(vertex[2][0], height - vertex[2][1]);
    glEnd();
    side = 0;
}
// Procedure to draw a circle
void draw_circle(int x,int y) {
    if (y <= 120)return;
    glColor3f(myColor[0], myColor[1], myColor[2]);
    glLineWidth(1);
    glPointSize(1);
    GLUquadricObj* mycircle = NULL;// declare
    if (mycircle == NULL) {// create
        mycircle = gluNewQuadric();
        switch (polygon_mode) {
        case POINT_MODE:
            gluQuadricDrawStyle(mycircle, GLU_POINT);
            break;
        case FILL:
            gluQuadricDrawStyle(mycircle, GLU_FILL);
            break;
        case LINE:
            gluQuadricDrawStyle(mycircle, GLU_LINE);
            break;
        case SILHOUETTE:
            gluQuadricDrawStyle(mycircle, GLU_SILHOUETTE);
            break;
        default:
            break;
        }
    }
    glPushMatrix();
    glTranslatef(x, height - y, 0.0);
    gluDisk(mycircle,
        0.0,    // inner radius=0.0
        cir_size,   // outer radius
        16,     // slice
        3);    // ring
    //glRasterPos2i(pos_x, height - pos_y);
    glPopMatrix();
}
// Procedure to show a string
void show_string(int x, int y,double r,double g,double b) {
    glColor3f(r, g, b);
    glRasterPos2i(x, y); //initial position
    for (int i = 0; i < (int)str.size(); i++) {
        switch (font) {
        case BITMAP_TIMES_ROMAN_10:
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, str[i]);
            break;
        case BITMAP_TIMES_ROMAN_24:
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
            break;
        case BITMAP_8_BY_13:
            glutBitmapCharacter(GLUT_BITMAP_8_BY_13, str[i]);
            break;
        case BITMAP_9_BY_15:
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[i]);
            break;
        case BITMAP_HELVETICA_10:
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, str[i]);
            break;
        case BITMAP_HELVETICA_12:
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[i]);
            break;
        case BITMAP_HELVETICA_18:
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
            break;
        default:
            break;
        }

    }
    glFinish();
}
// Callback function for color menu
void color_func(int value) {
    switch (value) {
    case BLACK:
        myColor[0] = myColor[1] = myColor[2] = 0.0;
        break;

    case RED:
        myColor[0] = 1.0;
        myColor[1] = myColor[2] = 0.0;
        break;

    case GREEN:
        myColor[0] = myColor[2] = 0.0;
        myColor[1] = 1.0;
        break;

    case BLUE:
        myColor[0] = myColor[1] = 0.0;
        myColor[2] = 1.0;
        break;
    case YELLOW:
        myColor[0] = myColor[1] = 1.0;
        myColor[2] = 0.0;
        break;
    case PURPLE:
        myColor[0] = myColor[2] = 1.0;
        myColor[1] = 0.0;
        break;
    case CYAN:
        myColor[1] = myColor[2] = 1.0;
        myColor[0] = 0.0;
        break;
    default:
        break;
    }
    glColor3f(myColor[0], myColor[1], myColor[2]);
}
// Callback function for size menu
void size_func(int value) {
    switch (value) {
    case BIGGER:
        if (obj_type == CIRCLE) {
            cir_size += 1.0;
        }
        else if (obj_type == LINE) {
            if (line_width < 10.0)line_width += 1.0;
        }
        else {
            if (pnt_size < 10.0)
                pnt_size += 1.0;
        }
        break;
    case SMALLER:
        if (obj_type == CIRCLE) {
            if (cir_size > 1.0) {
                cir_size -= 1.0;
            }
        }
        else if (obj_type == LINE) {
            if (line_width >1.0)line_width -= 1.0;
        }
        else {
            if (pnt_size > 1.0)
                pnt_size = pnt_size - 1.0;
        }
        break;
    default:
        break;
    }
    init_draw();
}
// Callback function for special key event.
void special_key(int key, int x, int y){
    // change size
    if (key == GLUT_KEY_DOWN) {
        size_func(SMALLER);
        return;
    }
    if (key == GLUT_KEY_UP) {
        size_func(BIGGER);
        return;
    }
}
// Callback function for keyboard event.
void keyboard(unsigned char key, int x, int y) {
    // input a char or a string
    if (obj_type == CHAR) {
        glColor3f(myColor[0], myColor[1], myColor[2]);
        glRasterPos2i(x, height - y);
        switch (font) {
        case BITMAP_TIMES_ROMAN_10:
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, key);
            break;
        case BITMAP_TIMES_ROMAN_24:
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, key);
            break;
        case BITMAP_8_BY_13:
            glutBitmapCharacter(GLUT_BITMAP_8_BY_13, key);
            break;
        case BITMAP_9_BY_15:
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, key);
            break;
        case BITMAP_HELVETICA_10:
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, key);
            break;
        case BITMAP_HELVETICA_12:
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, key);
            break;
        case BITMAP_HELVETICA_18:
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, key);
            break;
        default:
            break;
        }
        glFinish();
        return;
    }
    if (obj_type == STRING) {
        str = str + char(key);
        show_string(px, height - py,myColor[0],myColor[1],myColor[2]);
        return;
    }
    // special instruction
    if (obj_type != CHAR && obj_type != STRING) {
        if (key == 'D' || key == 'd') {// press d or D to draw a circle
            //pos_x = x, pos_y = y;
            draw_circle(x,y);
            glFinish();
            return;
        }
        if (key == 'Q' || key == 'q')// press q or Q to quit
            exit(0);
    }

}

// Callback function for draw type menu
void draw_type(int value) {
    obj_type = value;
    if (value == LINE || value == CURVE)
        first = 0;
    else if (value == POLYGON || value == TRIANGLE || value == POLYLINES)
        side = 0;
}
// Callback function for polygon fill mode menu
void fillpoly_func(int value) {
    polygon_mode = value;
}
// Procedure to draw a rectangle(for initial scene)
void draw_rectangle(double r, double g, double b, int xmin, int ymin, int xmax, int ymax) {
    side = 0;
    vertex[side][0] = xmin, vertex[side][1] = ymax, side++;
    vertex[side][0] = xmax, vertex[side][1] = ymax, side++;
    vertex[side][0] = xmax, vertex[side][1] = ymin, side++;
    vertex[side][0] = xmin, vertex[side][1] = ymin, side++;
    vertex[side][0] = xmin, vertex[side][1] = ymin;
    draw_polygon(r, g, b);
}


// Callback function for save menu
void save_func(int value) {
    not_empty[value] = 1;
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image[value]);
}
// Callback function for load menu
void load_func(int value) {
    if (!not_empty[value])
        return;
    glRasterPos2i(0, 0);
    glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, image[value]);
}
// draw the x,y index
void draw_index(double c) {
    for (int y = 0; y <= height; y += 100) {
        str = to_string(height - 100 - y);
        font = BITMAP_8_BY_13;
        if (str != "0")show_string(0, y, c, c, c);
        str = "";
    }
    for (int x = 0; x <= width; x += 100) {
        str = to_string(x);
        font = BITMAP_8_BY_13;
        show_string(x, height - 110, c, c, c);
        str = "";
    }
    glFinish();
}
void draw_bar(int x, int len, int y, string s) {// y=42
    draw_rectangle(0.90, 0.90, 0.90, x - 2, y - 2, x + len + 2, y + 16);
    draw_rectangle(0.95, 0.95, 0.95, x, y, x + len, y + 14);
    str = s;
    show_string(x, height - (y + 12), 0, 0, 0);
}
// draw the initial scene
void init_draw() {
    // save setting
    int pre_type = obj_type;
    int pre_mode = polygon_mode;
    int pre_font = font;

    font = BITMAP_8_BY_13;
    // draw main bar
    polygon_mode = FILL;
    draw_rectangle(0.7, 0.7, 0.7, 10, 10, width - 10, 100);
    draw_rectangle(1.0, 1.0, 1.0, 15, 15, width - 15, 95);
    // draw color bars
    draw_rectangle(0.8, 0.8, 0.8, 18, 18, 60, 36);
    str = "Color";
    show_string(18, height - 32, 0, 0, 0);
    draw_rectangle(0, 0, 0, 65, 20, 95, 34);// black color bar
    draw_rectangle(1, 0, 0, 100, 20, 130, 34);// red color bar
    draw_rectangle(0, 1, 0, 135, 20, 165, 34);// green color bar
    draw_rectangle(0, 0, 1, 170, 20, 200, 34);// blue color bar
    draw_rectangle(1, 1, 0, 205, 20, 235, 34);// yellow color bar
    draw_rectangle(1, 0, 1, 240, 20, 270, 34);// purple color bar
    draw_rectangle(0, 1, 1, 275, 20, 305, 34);// cyan color bar
    // draw type bars
    draw_rectangle(0.8, 0.8, 0.8, 18, 43, 60, 61);
    str = "Type";
    show_string(22, height - 57, 0, 0, 0);
    draw_bar(68, 40, 45, "Point");
    draw_bar(113, 32, 45, "Line");
    draw_bar(150, 56, 45, "Polygon");
    draw_bar(211, 47, 45, "Circle");
    draw_bar(263, 40, 45, "Curve");
    draw_bar(68, 65, 70, "Triangle");
    draw_bar(138, 72, 70, "Polylines");
    draw_bar(215, 34, 70, "Char");
    draw_bar(254, 49, 70, "String");
    // draw mode bars
    draw_rectangle(0.8, 0.8, 0.8, 320, 18, 355, 36);
    str = "Mode";
    show_string(322, height - 32, 0, 0, 0);
    draw_bar(360, 40, 20, "Point");
    draw_bar(405, 32, 20, "Line");
    draw_bar(442, 32, 20, "Fill");
    draw_bar(479, 80, 20, "Silhouette");


    // draw file bars
    draw_rectangle(0.8, 0.8, 0.8, 320, 43, 355, 61);
    str = "File";
    show_string(322, height - 57, 0, 0, 0);
    draw_bar(360, 32, 45, "Save");
    draw_bar(397, 32, 45, "Load");
    // draw font bars
    draw_rectangle(0.8, 0.8, 0.8, 570, 18, 605, 36);
    str = "Font";
    show_string(572, height - 32, 0, 0, 0);
    draw_bar(610, 30, 20, "");// button
    // draw grid bars
    draw_rectangle(0.8, 0.8, 0.8, 320, 68, 355, 86);
    str = "Grid";
    show_string(322, height - 82, 0, 0, 0);
    draw_bar(360, 32, 70, "Show");
    draw_bar(397, 32, 70, "Hide");
    // draw point size bar
    draw_rectangle(0.8, 0.8, 0.8, 660, 18, 754, 36);
    str = "Point  Size";
    show_string(664, height - 32, 0, 0, 0);
    draw_bar(760, 48, 20, "Bigger");
    draw_bar(813, 56, 20, "Smaller");
    draw_bar(874, 18, 20, to_string((int)pnt_size));
    // draw line width bar
    draw_rectangle(0.8, 0.8, 0.8, 660, 43, 754, 61);
    str = "Line  width";
    show_string(664, height - 57, 0, 0, 0);
    draw_bar(760, 48, 45, "Bigger");
    draw_bar(813, 56, 45, "Smaller");
    draw_bar(874, 18, 45, to_string((int)line_width));
    // draw circle size bar
    draw_rectangle(0.8, 0.8, 0.8, 660, 68, 754, 86);
    str = "Circle size";
    show_string(664, height - 82, 0, 0, 0);
    draw_bar(760, 48, 70, "Bigger");
    draw_bar(813, 56, 70, "Smaller");
    draw_bar(874, 18, 70, to_string((int)cir_size));
    // draw other bar
    draw_rectangle(0.8, 0.8, 0.8, 900, 18, 954, 36);
    str = "Others";
    show_string(904, height - 32, 0, 0, 0);
    draw_bar(904, 40, 45, "Clear");
    draw_bar(904, 40, 70, "Quit");
    // draw advanced type bar
    draw_rectangle(0.8, 0.8, 0.8, 438, 43, 508, 61);
    str = "Advanced";
    show_string(440, height - 57, 0, 0, 0);
    draw_bar(515, 24, 45, "Pen");
    draw_bar(544, 50, 45, "Eraser");
    draw_bar(599, 32, 45, "Blur");
    draw_bar(440, 50, 70, "Spray1");
    draw_bar(495, 50, 70, "Spray2");
    draw_bar(550, 50, 70, "Bubble");

    // draw line
    draw_rectangle(0.7, 0.7, 0.7, 15, 39, 650, 41);
    draw_rectangle(0.7, 0.7, 0.7, 434, 42, 436, 100);
    draw_rectangle(0.7, 0.7, 0.7, 310, 10, 312, 100);
    draw_rectangle(0.7, 0.7, 0.7, 650, 10, 652, 100);
    draw_rectangle(0.7, 0.7, 0.7, 564, 10, 566, 40);
    draw_rectangle(0.7, 0.7, 0.7, 310, 64, 436, 66);
    draw_rectangle(0.7, 0.7, 0.7, 896, 10, 898, 100);
    // return setting
    polygon_mode = pre_mode;
    obj_type = pre_type;
    font = pre_font;

    save_func(1);
    glFinish();
}
// draw the grid lines
void draw_grid() {
    // image[0]:原本沒有格線的圖
    file_id = 0;
    save_func(file_id);
    glColor3f(grid_color, grid_color, grid_color);
    for (int y = 0; y < SIZEY; y += offset) {
        if ((y / offset) % 5 == 0)glLineWidth(2.0);
        else glLineWidth(1.0);
        glBegin(GL_LINES);
        glVertex2f(0, height - y - 100);
        glVertex2f(width, height - y - 100);
        glEnd();
    }
    for (int x = 0; x < SIZEX; x += offset) {
        if ((x / offset) % 5 == 0)glLineWidth(2.0);
        else glLineWidth(1.0);
        glBegin(GL_LINES);
        glVertex2f(x, 0);
        glVertex2f(x, height - 100);
        glEnd();
    }
    // image[1]:上了格線後的圖
    file_id = 1;
    save_func(1);
    int gc = 204;
    if (first_load) {
        file_id = 1;
        draw_index(grid_color);
        glFinish();
        return;
    }
    for (int y = 0; y < SIZEY; y++) {
        for (int x = 0; x < SIZEX; x++) {
            int cur = y * width + x;
            if (y < 100)image[1][cur][0] = image[1][cur][1] = image[1][cur][2] = 255;

            if (image[7][cur][0] == gc) {
                // 若原本在現在要加格線的地方就有自己的顏色-> 保持image[1]的顏色
                if (!(image[0][cur][0] == 255 && image[0][cur][1] == 255 && image[0][cur][2] == 255)) {
                    image[1][cur][0] = image[0][cur][0];
                    image[1][cur][1] = image[0][cur][1];
                    image[1][cur][2] = image[0][cur][2];
                }
                else {// 否則 畫上格線
                    image[1][cur][0] = image[1][cur][1] = image[1][cur][2] = gc;
                }
            }
            else {
                image[1][cur][0] = image[0][cur][0];
                image[1][cur][1] = image[0][cur][1];
                image[1][cur][2] = image[0][cur][2];
            }
        }
    }
    file_id = 1;
    load_func(1);
    draw_index(grid_color);
    if(!press_font)init_draw();
    glFinish();
}
// hide the grid lines
void hide_grid() {
    file_id = 1;
    save_func(file_id);
    int gc = 204;
    int fid = file_id;
    for (int x = 0; x < SIZEX; x++) {
        for (int y = 0; y < SIZEY; y++) {
            int cur = y * width + x;
            if (image[fid][cur][0] == gc && image[fid][cur][1] == gc && image[fid][cur][2] == gc)
                image[fid][cur][0] = image[fid][cur][1] = image[fid][cur][2] = 255;
        }
    }
    load_func(file_id);
    // hide index
    glColor3f(1, 1, 1);
    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < SIZEY; y++) {
            glPointSize(pnt_size); //  Define point size
            glBegin(GL_POINTS);    //  Draw a point
            glVertex2f(x, height - y);
            glEnd();
        }
    }
    if(!press_font)init_draw();
    save_func(1);
    glFinish();
}
//  Callback function for setting menu
void setting_func(int value) {
    switch (value) {
    case SHOW_GRID:
        if (show_grid)return;
        show_grid = 1;
        draw_grid();
        break;
    case HIDE_GRID:
        if (!show_grid)return;
        show_grid = 0;
        hide_grid();
        break;
    default:
        break;
    }
}

void font_choose_open() {
    press_font = 1;// set to open
    save_func(0);
    int pre_font = font;
    font = BITMAP_8_BY_13;
    draw_rectangle(0.8, 0.8, 0.8, 640, 16, 820, 160);
    draw_bar(645, 170, 20, "BITMAP_TIMES_ROMAN_10");
    draw_bar(645, 170, 40, "BITMAP_TIMES_ROMAN_24");
    draw_bar(645, 170, 60, "BITMAP_8_BY_13");
    draw_bar(645, 170, 80, "BITMAP_9_BY_15");
    draw_bar(645, 170, 100, "BITMAP_HELVETICA_10");
    draw_bar(645, 170, 120, "BITMAP_HELVETICA_12");
    draw_bar(645, 170, 140, "BITMAP_HELVETICA_18");
    font = pre_font;
    save_func(1);
    glFinish();
}
void font_choose_close() {
    press_font = 0;// set to close
    load_func(0);
    glFinish();
}
// Callback function for font menu
void font_func(int value) {
    font = value;
}

void save_choose_open() {
    press_save = 1;
    save_func(0);
    int pre_font = font;
    font = BITMAP_8_BY_13;
    draw_rectangle(0.8, 0.8, 0.8, 380, 46, 400, 110);
    draw_bar(385, 10, 50, "1");
    draw_bar(385, 10, 70, "2");
    draw_bar(385, 10, 90, "3");
    font = pre_font;
    save_func(1);
    glFinish();
}
void save_choose_close() {
    press_save = 0;
    load_func(0);
    glFinish();
}
void load_choose_open() {
    press_load = 1;
    save_func(6);
    int pre_font = font;
    font = BITMAP_8_BY_13;
    draw_rectangle(0.8, 0.8, 0.8, 417, 46, 437, 110);
    draw_bar(422, 10, 50, "1");
    draw_bar(422, 10, 70, "2");
    draw_bar(422, 10, 90, "3");
    font = pre_font;
    //save_func(1);
    glFinish();
}
void load_choose_close(bool c) {
    press_load = 0;
    if (c) {
        load_func(6);
    }
    glFinish();
}


void init_grid() {
    glColor3f(grid_color, grid_color, grid_color);
    for (int y = 0; y < SIZEY; y += offset) {
        if ((y / offset) % 5 == 0)glLineWidth(2.0);
        else glLineWidth(1.0);
        glBegin(GL_LINES);
        glVertex2f(0, height - y - 100);
        glVertex2f(width, height - y - 100);
        glEnd();
    }
    for (int x = 0; x < SIZEX; x += offset) {
        if ((x / offset) % 5 == 0)glLineWidth(2.0);
        else glLineWidth(1.0);
        glBegin(GL_LINES);
        glVertex2f(x, 0);
        glVertex2f(x, height - 100);
        glEnd();
    }
    save_func(7);
}
void other_func(int value) {
    if (value == MY_QUIT)
        exit(0);
    else if (value == MY_CLEAR) {
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        save_func(1);
        if (show_grid)draw_grid();
        init_draw();
        glFinish();
    }
    glFlush();
}
void spray1(int x_, int y_) {
    int cs = cir_size;
    int pre_mode = polygon_mode;
    polygon_mode = FILL;
    for (int i = 0; i < 10; i++) {
        double rx = ((x_ + 5) - (x_ - 5)) * rand() / (RAND_MAX + 1.0) + (x_ - 5);
        double ry = ((y_ + 5) - (y_ - 5)) * rand() / (RAND_MAX + 1.0) + (y_ - 5);
        double ret = fabs(rx - x_) + fabs(ry - y_);
        double rcs = (cs * 0.7 / ret - cs * 0.01) * rand() / (RAND_MAX + 1.0) + 0.01 * cs;
        if (rcs > 0.5 * cs)continue;
        cir_size = rcs;
        draw_circle(rx, ry);
    }
    cir_size = cs;
    polygon_mode = pre_mode;
}
void spray2(int x_, int y_) {
    int cs = cir_size;
    cir_size = 0.1 * cs;
    int pre_mode = polygon_mode;
    polygon_mode = FILL;
    for (int i = 0; i < 10; i++) {
        double rx = ((x_ + 5) - (x_ - 5)) * rand() / (RAND_MAX + 1.0) + (x_ - 5);
        double ry = ((y_ + 5) - (y_ - 5)) * rand() / (RAND_MAX + 1.0) + (y_ - 5);
        double ret = fabs(rx - x_) + fabs(ry - y_);
        draw_circle(rx, ry);
    }
    cir_size = cs;
    polygon_mode = pre_mode;
}
void blur(int x_, int y_) {
    if (show_grid)setting_func(HIDE_GRID);
    y_ = height - y_;
    int len = 3;
    save_func(0);
    for (int x = max(0, x_ - (int)cir_size); x <= min(x_ + (int)cir_size, width); x++) {
        for (int y = max(0, y_ - (int)cir_size); y <= min(y_ + (int)cir_size, height); y++) {
            if (y <= 100)continue;
            int cnt = 0;
            int r = 0, g = 0, b = 0;
            for (int i = max(0, x - len); i <= min(x + len, width); i++) {
                for (int j = max(0, y - len); j <= y + len; j++) {
                    cnt++;
                    int cur = j * width + i;
                    r += image[0][cur][0];
                    g += image[0][cur][1];
                    b += image[0][cur][2];
                }
            }
            if (cnt == 0)return;
            r /= cnt, g /= cnt, b /= cnt;
            if (r > 255)r = 255;
            if (g > 255)g = 255;
            if (b > 255)b = 255;
            image[1][y * width + x][0] = r;
            image[1][y * width + x][1] = g;
            image[1][y * width + x][2] = b;
        }
    }
    load_func(1);
}
void bubble(int x_, int y_) {
    
    double p = rand() / (RAND_MAX + 1.0);
    if (p <= 0.7)return;
    int cs = cir_size;
    int pre_mode = polygon_mode;
    polygon_mode = SILHOUETTE;
    double rx = ((x_ + 5) - (x_ - 5)) * rand() / (RAND_MAX + 1.0) + (x_ - 5);
    double ry = ((y_ + 5) - (y_ - 5)) * rand() / (RAND_MAX + 1.0) + (y_ - 5);
    double rcs = (cs - cs * 0.3) * rand() / (RAND_MAX + 1.0) + 0.3 * cs;
    double rr = myColor[0];
    double rg = myColor[1];
    double rb = myColor[2];
    double rdrgb = (0.4) * rand() / (RAND_MAX + 1.0);
    if (myColor[0] != 0)myColor[0] -= rdrgb;
    else myColor[0] += rdrgb;
    rdrgb = (0.4) * rand() / (RAND_MAX + 1.0);
    if (myColor[1] != 0)myColor[1] -= rdrgb;
    else myColor[1] += rdrgb;
    rdrgb = (0.4) * rand() / (RAND_MAX + 1.0);
    if (myColor[2] != 0)myColor[2] -= rdrgb;
    else myColor[2] += rdrgb;
    if (myColor[0] >= 1)myColor[0] = 1;
    if (myColor[0] < 0)myColor[0] = 0;
    if (myColor[1] >= 1)myColor[0] = 1;
    if (myColor[1] < 0)myColor[0] = 0;
    if (myColor[2] >= 1)myColor[0] = 1;
    if (myColor[2] < 0)myColor[0] = 0;
    cir_size = rcs;
    draw_circle(rx, ry);

    myColor[0] = rr, myColor[1] = rg, myColor[2] = rb;
    cir_size = cs;
    polygon_mode = pre_mode;
}
// Callback function handling mouse-press events
void mouse_func(int button, int state, int x, int y) {
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) {
        if (obj_type == CURVE) {
            first = 0;
        }
        return;
    }
    if (press_font) {// choose font or not
        if (y > 16 && y < 160 && x > 640 && x < 820) {
            if (y < 40)font_func(BITMAP_TIMES_ROMAN_10);
            else if (y < 60)font_func(BITMAP_TIMES_ROMAN_24);
            else if (y < 80)font_func(BITMAP_8_BY_13);
            else if (y < 100)font_func(BITMAP_9_BY_15);
            else if (y < 120)font_func(BITMAP_HELVETICA_10);
            else if (y < 140)font_func(BITMAP_HELVETICA_12);
            else font_func(BITMAP_HELVETICA_18);
        }
        font_choose_close();
        return;
    }
    if (press_save) {// choose save or not
        if (y > 46 && y < 110 && x>380 && x < 400) {
            save_choose_close();
            hide_grid();
            if (y < 70) save_func(2);
            else if (y < 90) save_func(3);
            else save_func(4);
            draw_grid();
        }
        else
            save_choose_close();
        return;
    }
    if (press_load) {
        if (y > 46 && y < 110 && x>417 && x < 437) {
            int temp = 0;
            if (y < 70) temp=2;
            else if (y < 90) temp=3;
            else temp=4;
            if (!not_empty[temp]) {
                load_choose_close(1);
            }
            else {
                load_func(temp);
                load_choose_close(0);
            }
            
            show_grid = 0;
        }
        else load_choose_close(1);
        return;
    }
    // left button press down
    if (y < 100) {// tools
        if (y > 20 && y < 34 && x < 305 && x > 65) {// color bar
            if (x < 95)color_func(BLACK);
            else if (x < 130)color_func(RED);
            else if (x < 165)color_func(GREEN);
            else if (x < 200)color_func(BLUE);
            else if (x < 235)color_func(YELLOW);
            else if (x < 270)color_func(PURPLE);
            else color_func(CYAN);
        }
        else if (x < 307 && x > 65) {// type
            if (y < 70) {// up bar
                if (x < 108)draw_type(POINT);
                else if (x < 145)draw_type(LINE);
                else if (x < 206)draw_type(POLYGON);
                else if (x < 258)draw_type(CIRCLE);
                else draw_type(CURVE);
            }
            else {// down bar
                if (x < 133)draw_type(TRIANGLE);
                else if (x < 210)draw_type(POLYLINES);
                else if (x < 249)draw_type(CHAR);
                else draw_type(STRING);
            }
        }
        else if (y > 20 && y < 34 && x > 360 && x < 559) {// mode
            if (x < 400)fillpoly_func(POINT_MODE);
            else if (x < 438)fillpoly_func(LINE_MODE);
            else if (x < 474)fillpoly_func(FILL);
            else fillpoly_func(SILHOUETTE);
        }
        else if (y > 20 && y < 34 && x > 610 && x < 640) {// font button
            if (!press_font)font_choose_open();
            else font_choose_close();
        }
        else if (y > 45 && y < 59 && x > 360 && x < 392) {// save button
            if (!press_save)save_choose_open();
            else save_choose_close();
        }
        else if (y > 45 && y < 59 && x > 397 && x < 429) {// load button
            if (!press_load)load_choose_open();
            //else load_choose_close(1);
        }
        else if (y > 70 && y < 84 && x > 360 && x < 392)
            setting_func(SHOW_GRID);
        else if (y > 70 && y < 84 && x > 397 && x < 429)
            setting_func(HIDE_GRID);
        else if (x > 760 && x < 869) {// size bar
            int pre_type = obj_type;
            if (y > 20 && y < 34) {
                obj_type = POINT;
                if (x < 808)size_func(BIGGER);
                else size_func(SMALLER);
            }
            else if (y < 59) {
                obj_type = LINE;
                if (x < 808)size_func(BIGGER);
                else size_func(SMALLER);
            }
            else {
                obj_type = CIRCLE;
                if (x < 808)size_func(BIGGER);
                else size_func(SMALLER);
            }
            obj_type = pre_type;
        }
        else if (x > 904 && x < 944 && y>45) {// others bar
            if (y > 45 && y < 59) other_func(MY_CLEAR);
            else other_func(MY_QUIT);
        }
        else if (x > 515 && x < 631 && y>45 && y < 59) {// advanced bar
            if (x < 539)draw_type(PEN);
            else if (x < 594)draw_type(ERASER);
            else draw_type(BLUR);
        }
        else if (x > 440 && x < 600 && y>70) {
            if (x < 490)draw_type(SPRAY1);
            else if (x < 545)draw_type(SPRAY2);
            else draw_type(BUBBLE);
        }
    }
    else {// draw
        if (y <= 100)return;
        glColor3f(myColor[0], myColor[1], myColor[2]);
        switch (obj_type) {
        case POINT:
            glPointSize(pnt_size); //  Define point size
            glBegin(GL_POINTS);    //  Draw a point
            glVertex2f(x, height - y);
            glEnd();
            break;
        case LINE:
            if (first == 0) {// first point
                first = 1;
                px = x;
                py = y;
                glPointSize(pnt_size);
                glBegin(GL_POINTS); //  Draw the first point(pos_x,pos_y)
                glVertex2f(x, height - y);
                glEnd();
            }
            else {// second point
                first = 0;
                glLineWidth(line_width); // Define line width
                glBegin(GL_LINES);     // Draw the line[2 points:(pos_x,pos_y),(x,y)]
                glVertex2f(px, height - py);
                glVertex2f(x, height - y);
                glEnd();
                glPointSize(pnt_size);
                glBegin(GL_POINTS); //  Draw the first point(pos_x,pos_y)
                glVertex2f(x, height - y);
                glEnd();
            }
            break;
        case POLYGON: // Define vertices of poly
            if (side == 0) {
                vertex[side][0] = x;
                vertex[side][1] = y;
                glPointSize(pnt_size);
                glBegin(GL_POINTS); //  Draw the first point(pos_x,pos_y)
                glVertex2f(x, height - y);
                glEnd();
                side++;
            }
            else {
                // 連點兩下就可以畫好poly囉(和前一個點曼哈頓距離<2)
                if (fabs(vertex[side - 1][0] - x) + fabs(vertex[side - 1][1] - y) < 2)
                    draw_polygon(myColor[0], myColor[1], myColor[2]);
                else {
                    glLineWidth(line_width);
                    glBegin(GL_LINES);// draw a line
                    glVertex2f(vertex[side - 1][0], height - vertex[side - 1][1]);// pre point
                    glVertex2f(x, height - y);// cur point
                    glEnd();
                    vertex[side][0] = x;
                    vertex[side][1] = y;
                    side++;
                }
                glPointSize(pnt_size);
                glBegin(GL_POINTS); //  Draw the first point(pos_x,pos_y)
                glVertex2f(x, height - y);
                glEnd();
            }
            break;
        case TRIANGLE:
            if (side == 0) {
                vertex[side][0] = x;
                vertex[side][1] = y;
                glPointSize(pnt_size);
                glBegin(GL_POINTS); //  Draw the first point(pos_x,pos_y)
                glVertex2f(x, height - y);
                glEnd();
                side++;
            }
            else {
                if (side == 3) // draw a RGB colors triangle
                    draw_triangle();
                else {
                    glLineWidth(line_width);
                    glBegin(GL_LINES);// draw a line
                    glVertex2f(vertex[side - 1][0], height - vertex[side - 1][1]);// pre point
                    glVertex2f(x, height - y);// cur point
                    glEnd();
                    vertex[side][0] = x;
                    vertex[side][1] = y;
                    side++;
                    glPointSize(pnt_size);
                    glBegin(GL_POINTS); //  Draw the first point(pos_x,pos_y)
                    glVertex2f(x, height - y);
                    glEnd();
                }
            }
            break;
        case POLYLINES:
            if (side == 0) {
                vertex[side][0] = x;
                vertex[side][1] = y;
                glPointSize(pnt_size);
                glBegin(GL_POINTS); //  Draw the first point(pos_x,pos_y)
                glVertex2f(x, height - y);
                glEnd();
                side++;
            }
            else {
                // 連點兩下就可以畫好polylines(和前一個點曼哈頓距離<2)
                if (fabs(vertex[side - 1][0] - x) + fabs(vertex[side - 1][1] - y) < 2)
                    side = 0;
                else {
                    glLineWidth(line_width);
                    glBegin(GL_LINES);// draw a line
                    glVertex2f(vertex[side - 1][0], height - vertex[side - 1][1]);// pre point
                    glVertex2f(x, height - y);// cur point
                    glEnd();
                    vertex[side][0] = x;
                    vertex[side][1] = y;
                    side++;
                    glPointSize(pnt_size);
                    glBegin(GL_POINTS); //  Draw the first point(pos_x,pos_y)
                    glVertex2f(x, height - y);
                    glEnd();
                }
            }

            break;
        case CIRCLE:
            //pos_x = x;
            //pos_y = y;
            draw_circle(x,y);
            break;
        case STRING:
            px = x;
            py = y;
            str = "";
            break;
        default:
            break;
        }
    }
    glFinish();
}

/*
    reshape callback function for window.
    a window pops up,
    the window’s size(shape) is changed.
*/
void my_reshape(int new_w, int new_h) {
    height = new_h, width = new_w;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();// Record the dimension of the window
    gluOrtho2D(0.0, (double)width, 0.0, (double)height);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_MODELVIEW);

    glutPostRedisplay(); // Trigger Display event for redisplay window

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);
    first_load = 1;
}

// motion callback function. The mouse is pressed and moved.
void motion_func(int x, int y) {
    if (y <= 100)return;
    if (obj_type == CURVE) {
        if (first == 0) {
            first = 1;
            pos_x = x;
            pos_y = y;
        }
        else {
            glColor3f(myColor[0], myColor[1], myColor[2]);
            glLineWidth(line_width);
            glBegin(GL_LINES);
            glVertex3f(pos_x, height - pos_y, 0.0);
            glVertex3f(x, height - y, 0.0);
            glEnd();
            pos_x = x;
            pos_y = y;
        }
        glFinish();
    }
    else if (obj_type == PEN) {
        glColor3f(myColor[0], myColor[1], myColor[2]);
        draw_circle(x, y);
        pos_x = x;
        pos_y = y;
        save_func(1);
        glFinish();
    }
    else if (obj_type == ERASER) {
        int pre_mode = polygon_mode;
        double r = myColor[0], g = myColor[1], b = myColor[2];
        myColor[0] = myColor[1] = myColor[2] = 1;
        polygon_mode = FILL;
        glColor3f(1, 1, 1);
        draw_circle(x, y);
        pos_x = x;
        pos_y = y;
        save_func(1);
        glFinish();
        myColor[0] = r, myColor[1] = g, myColor[2] = b;
        setting_func(HIDE_GRID);
        polygon_mode = pre_mode;
    }
    else if (obj_type == BLUR) {
        blur(x, y);
        glFinish();
    }
    else if (obj_type == SPRAY1) {
        spray1(x, y);
        glFinish();
    }
    else if (obj_type == SPRAY2) {
        spray2(x, y);
        glFinish();
    }
    else if (obj_type == BUBBLE) {
        bubble(x, y);
        glFinish();
    }
}
// Callback function for passive motion
void passive_motion_func(int x,int y) {
    save_func(5);
    pos_x = x, pos_y = y;

    int pre_size = cir_size;
    if (obj_type == PEN)draw_circle(pos_x, pos_y);
    else {
        cir_size = 5;
        draw_circle(pos_x, pos_y);
        cir_size = pre_size;
    }
    
    glFinish();
    load_func(5);
    
}
// Procedure to initialize data alighment and other stuff and set image memory
void init_func() {
    glReadBuffer(GL_FRONT);// select the front buf
    glDrawBuffer(GL_FRONT);// select the front buf
    glPixelStorei(GL_PACK_ALIGNMENT, 1);// alighment=1
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);// alighment=1
    memset(image, 255, sizeof(image));
}
// Callback function for top menu. Do nothing.
void top_menu_func(int value) {
}

// Callback function for file menu
void file_func(int value) {
    
}

// Callback function for style menu
void style_func(int value) {

}


// Callback function for display, redisplay, expose events
void display_func(void) {
    // define window background color
    //glClearColor(1, 1, 1, 1);
    if (first_load) {
        draw_grid();
        if(!press_font)init_draw();
        first_load = 0;
        init_grid();
        return;
    }
    load_func(1);// copy the saved color buffer contents
    glFlush();
}
// Callback function for idle event
void idle_func(void) {
    save_func(5);
    int pre_size = cir_size;
    if(obj_type==PEN)draw_circle(pos_x, pos_y);
    else {
        cir_size = 5;
        draw_circle(pos_x, pos_y);
        cir_size = pre_size;
    }
    glFinish();
    load_func(5);

    // save the contents of color buffer
    save_func(1);
}
// Callback function for advanced type
void advanced_func(int value) {
    first = 0;
    obj_type = value;
}
// Main procedure sets up the window environment.
void main(int argc, char** argv) {
    // make a session with the window system.
    glutInit(&argc, argv);

    // define geometrical parameters for the window
    glutInitWindowPosition(0,0);  // initial position
    glutInitWindowSize(width, height); // window size

    // define display mode
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA); // set display mode

    init_func();

    // Create parent window
    glutCreateWindow("Painter");

    // set background color
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // Register callback functions
    glutDisplayFunc(display_func); // display event
    glutReshapeFunc(my_reshape);   // reshape event
    glutKeyboardFunc(keyboard); // keyboard event
    glutSpecialFunc(special_key);
    glutMouseFunc(mouse_func);   // Mouse Button event
    glutMotionFunc(motion_func); // Mouse motion event
    glutPassiveMotionFunc(passive_motion_func);
    glutIdleFunc(idle_func);

    // sub-menus
    color_m = glutCreateMenu(color_func); /* Create color-menu */
    glutAddMenuEntry("black", BLACK);
    glutAddMenuEntry("red", RED);
    glutAddMenuEntry("green", GREEN);
    glutAddMenuEntry("blue", BLUE);
    glutAddMenuEntry("yellow", YELLOW);
    glutAddMenuEntry("purple", PURPLE);
    glutAddMenuEntry("cyan", CYAN);

    save_m = glutCreateMenu(save_func);
    glutAddMenuEntry("1", 2);
    glutAddMenuEntry("2", 3);
    glutAddMenuEntry("3", 4);

    load_m = glutCreateMenu(load_func);
    glutAddMenuEntry("1", 2);
    glutAddMenuEntry("2", 3);
    glutAddMenuEntry("3", 4);

    file_m = glutCreateMenu(file_func); /* Create another menu, file-menu */
    glutAddSubMenu("save", save_m);
    glutAddSubMenu("load", load_m);
   
    other_m = glutCreateMenu(other_func);
    glutAddMenuEntry("clear", MY_CLEAR);
    glutAddMenuEntry("quit", MY_QUIT);

    type_m = glutCreateMenu(draw_type); /* Create draw-type menu */
    glutAddMenuEntry("Point", POINT);
    glutAddMenuEntry("Line", LINE);
    glutAddMenuEntry("Polygon", POLYGON);
    glutAddMenuEntry("Triangle", TRIANGLE);
    glutAddMenuEntry("Curve", CURVE);
    glutAddMenuEntry("Circle", CIRCLE);
    glutAddMenuEntry("Polylines", POLYLINES);
    glutAddMenuEntry("Char", CHAR);
    glutAddMenuEntry("String", STRING);

    advanced_m = glutCreateMenu(advanced_func);
    glutAddMenuEntry("Pen", PEN);
    glutAddMenuEntry("Eraser", ERASER);
    glutAddMenuEntry("Blur", BLUR);
    glutAddMenuEntry("Spray1", SPRAY1);
    glutAddMenuEntry("Spray2", SPRAY2);
    glutAddMenuEntry("Bubble", BUBBLE);

    size_m = glutCreateMenu(size_func);
    glutAddMenuEntry("Bigger", BIGGER);
    glutAddMenuEntry("Smaller", SMALLER);

    font_m = glutCreateMenu(font_func); // create font menu
    glutAddMenuEntry("BITMAP_TIMES_ROMAN_10", BITMAP_TIMES_ROMAN_10);
    glutAddMenuEntry("BITMAP_TIMES_ROMAN_24", BITMAP_TIMES_ROMAN_24);
    glutAddMenuEntry("BITMAP_8_BY_13", BITMAP_8_BY_13);
    glutAddMenuEntry("BITMAP_9_BY_15", BITMAP_9_BY_15);
    glutAddMenuEntry("BITMAP_HELVETICA_10", BITMAP_HELVETICA_10);
    glutAddMenuEntry("BITMAP_HELVETICA_12", BITMAP_HELVETICA_12);
    glutAddMenuEntry("BITMAP_HELVETICA_18", BITMAP_HELVETICA_18);

    setting_m = glutCreateMenu(setting_func);
    glutAddMenuEntry("Show Grid", SHOW_GRID);
    glutAddMenuEntry("Hide Grid", HIDE_GRID);

    fill_m = glutCreateMenu(fillpoly_func);
    glutAddMenuEntry("Point", POINT_MODE);
    glutAddMenuEntry("Line", LINE_MODE);
    glutAddMenuEntry("Fill", FILL);
    glutAddMenuEntry("Silhouette", SILHOUETTE);

    // style_m: font_m + fillpoly_m
    style_m = glutCreateMenu(style_func);
    glutAddSubMenu("fill mode", fill_m);
    glutAddSubMenu("font", font_m);

    //  top-menu
    top_m = glutCreateMenu(top_menu_func); // Create top menu
    glutAddSubMenu("Colors", color_m);     // add sub-menus
    glutAddSubMenu("Type", type_m);
    glutAddSubMenu("Advanced Type", advanced_m);
    glutAddSubMenu("Size", size_m);
    glutAddSubMenu("Style", style_m);
    glutAddSubMenu("Setting", setting_m);
    glutAddSubMenu("File", file_m);
    glutAddSubMenu("Others", other_m);
    glutAttachMenu(GLUT_RIGHT_BUTTON); // associate top-menu with right but

    // Enter the infinite loop
    glutMainLoop();
}
