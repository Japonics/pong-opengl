#include <string>
#include <windows.h>
#include <iostream>
#include <conio.h>
#include <sstream>
#include <math.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "GL/freeglut.h"

#define VK_W 0x57
#define VK_S 0x53

using namespace std;

// window size and update rate (60 fps)
int width = 1000;
int height = 500;
int interval = 1000 / 60;

// score
int score_left = 0;
int score_right = 0;

// rackets in general
int racket_width = 10;
int racket_height = 80;
int racket_speed = 3;

// left racket position
float racket_left_x = 10.0f;
float racket_left_y = 50.0f;

// right racket position
float racket_right_x = width - racket_width - 10;
float racket_right_y = 50;

float ball_pos_x = width / 2;
float ball_pos_y = height / 2;
float ball_dir_x = -1.0f;
float ball_dir_y = 0.0f;
int ball_size = 8;
int ball_speed = 3;
int touches = 0;

string int2str(int x);
void drawText(float x, float y, std::string text, int r, int g, int b);
void drawRect(float x, float y, float width, float height, int r, int g, int b);
void draw();
void keyboard();
void update(int value);
void enable2D(int width, int height);
void updateBall();
void vec2_norm(float& x, float &y);

void draw() {
    // clear (has to be done at the beginning)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // draw ball
    drawRect(ball_pos_x - ball_size / 2, ball_pos_y - ball_size / 2, ball_size, ball_size, 0, 0, 255);

    // draw player 1 points
    drawText(width / 2 - 50, 20, "Player 1:" + int2str(score_left), 0, 255, 0);
    drawRect(racket_left_x, racket_left_y, racket_width, racket_height, 0, 255, 0);

    // draw player 2 points
    drawText(width / 2 + 50, 20, "Player 2:" + int2str(score_right), 255, 0, 0);
    drawRect(racket_right_x, racket_right_y, racket_width, racket_height, 255, 0, 0);

    // swap buffers (has to be done at the end)
    glutSwapBuffers();
}

void keyboard() {
    // left racket
    if (GetAsyncKeyState(VK_W)) racket_left_y += racket_speed;
    if (GetAsyncKeyState(VK_S)) racket_left_y -= racket_speed;

    // right racket
    if (GetAsyncKeyState(VK_UP)) racket_right_y += racket_speed;
    if (GetAsyncKeyState(VK_DOWN)) racket_right_y -= racket_speed;
}

void update(int value) {
    // input handling
   keyboard();

 // update ball
   updateBall();
   // Call update() again in 'interval' milliseconds
   glutTimerFunc(interval, update, 0);

   // Redisplay frame
   glutPostRedisplay();
}

void enable2D(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, width, 0.0f, height, 0.0f, 1.0f);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
}

void drawText(float x, float y, std::string text, int r, int g, int b) {
    glColor3ub(r, g, b);
    glRasterPos2f(x, y);
    glutBitmapString(GLUT_BITMAP_8_BY_13, (const unsigned char*)text.c_str());
}

void drawRect(float x, float y, float width, float height, int r, int g, int b) {
    glBegin(GL_QUADS);
        glColor3ub(r, g, b);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
    glEnd();
}

void updateBall() {
    // fly a bit
    ball_pos_x += ball_dir_x * ball_speed;
    ball_pos_y += ball_dir_y * ball_speed;

    // hit by left racket?
    if (ball_pos_x < racket_left_x + racket_width &&
        ball_pos_x > racket_left_x &&
        ball_pos_y < racket_left_y + racket_height &&
        ball_pos_y > racket_left_y) {
        // set fly direction depending on where it hit the racket
        // (t is 0.5 if hit at top, 0 at center, -0.5 at bottom)
        float t = ((ball_pos_y - racket_left_y) / racket_height) - 0.5f;
        ball_dir_x = fabs(ball_dir_x); // force it to be positive
        ball_dir_y = t;
        touches++;

        if (touches == 5) {
            ++ball_speed;
            touches = 0;
        }
    }

    // hit by right racket?
    if (ball_pos_x > racket_right_x &&
        ball_pos_x < racket_right_x + racket_width &&
        ball_pos_y < racket_right_y + racket_height &&
        ball_pos_y > racket_right_y) {
        // set fly direction depending on where it hit the racket
        // (t is 0.5 if hit at top, 0 at center, -0.5 at bottom)
        float t = ((ball_pos_y - racket_right_y) / racket_height) - 0.5f;
        ball_dir_x = -fabs(ball_dir_x); // force it to be negative
        ball_dir_y = t;
        touches++;

        if (touches == 5) {
            ++ball_speed;
            touches = 0;
        }
    }

    // hit left wall?
    if (ball_pos_x < 0) {
        ++score_right;
        ball_speed = 4;
        touches = 0;
        ball_pos_x = width / 2;
        ball_pos_y = height / 2;
        ball_dir_x = fabs(ball_dir_x); // force it to be positive
        ball_dir_y = 0;
    }

    // hit right wall?
    if (ball_pos_x > width) {
        ++score_left;
        ball_speed = 4;
        touches = 0;
        ball_pos_x = width / 2;
        ball_pos_y = height / 2;
        ball_dir_x = -fabs(ball_dir_x); // force it to be negative
        ball_dir_y = 0;
    }

    // hit top wall?
    if (ball_pos_y > height) {
        ball_dir_y = -fabs(ball_dir_y); // force it to be negative
    }

    // hit bottom wall?
    if (ball_pos_y < 0) {
        ball_dir_y = fabs(ball_dir_y); // force it to be positive
    }

    if ((racket_left_y + racket_height) > height) racket_left_y = height - racket_height;
    if ((racket_right_y + racket_height) > height) racket_right_y = height - racket_height;
    if (racket_left_y < 0) racket_left_y = 0;
    if (racket_right_y < 0) racket_right_y = 0;

    // make sure that length of dir stays at 1
    vec2_norm(ball_dir_x, ball_dir_y);
}

int main(int argc, char** argv) {

    // initialize OpenGL (via glut)
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow("Gra");

    // Register callback functions
    glutDisplayFunc(draw);
    glutTimerFunc(interval, update, 0);

    // setup scene to 2d mode and set draw color to white
    enable2D(width, height);

    // start the whole thing
    glutMainLoop();
    return 0;

}

string int2str(int x) {
    // converts int to string
    std::stringstream ss;
    ss << x;
    return ss.str( );
}

void vec2_norm(float& x, float &y) {
    // sets a vectors length to 1 (which means that x + y == 1)
    float length = sqrt((x * x) + (y * y));
    if (length != 0.0f) {
        length = 1.0f / length;
        x *= length;
        y *= length;
    }
}
