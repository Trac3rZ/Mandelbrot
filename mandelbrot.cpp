#include <GL/glut.h>
#include <cmath>
#include <thread>
#include <iostream>
#include "mandelbrot.h"
#include "config.h"

using namespace std;

void (*displayFunction)() = multiThread ? displayMandelbrotMultiThread : displayMandelbrotSingleThread;
int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
  glutInitWindowSize(width, height);
  glutInitWindowPosition(250, 50);
  glutCreateWindow("Mandelbrot");
  glutMouseFunc(mouseClicks);
  glutKeyboardFunc(keyboardFunction);
  glutDisplayFunc(displayFunction);
  glutMainLoop();
  return 0;
}

void fillMandelbrot(int arr[width][height], int numThread) {
  std::thread threads[numThread];
  for (int pos = 0; pos < numThread; ++pos) {
    threads[pos] = std::thread(fillMandelbrotPartial, arr, numThread, pos);
  }
  for (int pos = 0; pos < numThread; ++pos) {
    threads[pos].join();
  }
}

void fillMandelbrotPartial(int arr[][height], int numSlice, int pos) {
  ldouble rScaled, iScaled;
  double rangeX = maxReal - minReal;
  double rangeY = maxImag - minImag;
  for (int r = pos * (width/numSlice); r < (pos + 1) * (width/numSlice); r++) {
      for (int i = 0; i < height; i++) {
	rScaled = r * rangeX / width + minReal;
	iScaled = i * rangeY / height + minImag;
        arr[r][i] = getIterMandelbrot(rScaled, iScaled, maxIter);
      }
  }
}

void displayMandelbrotMultiThread(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D(0, width, height, 0);
    glBegin(GL_POINTS);
    double red, blue, green;
    int iter;
    
    int iterMap[width][height];
    fillMandelbrot(iterMap, numThread);
    // begin main loop

    for (int r = 0; r < width; r++) {
      for (int i = 0; i < height; i++) {
	if ((iter = iterMap[r][i]) == maxIter) {
	  glColor3d(0, 0, 0);
	}
	else {
	  red = (int) ((maxIter - iter) * M_PI) % maxIter;
	  green = ((maxIter - iter) * (maxIter - iter)) % maxIter;
	  blue = (int) ((maxIter - iter) * sqrt(2)) % maxIter;
	  glColor3d(red/maxIter, green/maxIter, blue/maxIter);
	}
	glVertex2i(r, i);
      }
    }
    glEnd();
    glFlush();
}


void displayMandelbrotSingleThread(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D(0, width, height, 0);
    glBegin(GL_POINTS);

    ldouble rScaled;
    ldouble iScaled;
    ldouble rangeX = maxReal - minReal;
    ldouble rangeY = maxImag - minImag;
    double red, blue, green;
    
    // begin main loop

    for (int r = 0; r < width; r++) {
      for (int i = 0; i < height; i++) {
	rScaled = r * rangeX / width + minReal;
	iScaled = i * rangeY / height + minImag;
	int iter = getIterMandelbrot(rScaled, iScaled, maxIter);
	if (iter == maxIter) {
	  glColor3d(0, 0, 0);
	}
	else {
	  red = (int) ((maxIter - iter) * M_PI) % maxIter;
	  green = ((maxIter - iter) * (maxIter - iter)) % maxIter;
	  blue = (int) ((maxIter - iter) * sqrt(2)) % maxIter;
	  glColor3d(red/maxIter, green/maxIter, blue/maxIter);
	}
	glVertex2i(r, i);
      }
    }
    glEnd();
    glFlush();
}

void mouseClicks(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) { // left mouse clicked
    recalculateRange(x, y, true, true, 0.5);
  }
  
  else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) { // right mouse clicked
    recalculateRange(x, y, false, true, 2);
  }

  else if (button == 3) { // scroll up, zoom in
    recalculateRange(x, y, true, false, 0.85);
  }

  else if (button == 4) { // scroll down, zoom out
    recalculateRange(x, y, false, false, 1.15);
  }
  glutPostRedisplay();
}

void keyboardFunction(unsigned char key, int x, int y) {
  ldouble dX = (maxReal - minReal) * 0.1;
  ldouble dY = (maxImag - minImag) * 0.1;
  switch (key) {
  case 'w':
    minImag -= dY;
    maxImag -= dY;
    break;
  case 's':
    minImag += dY;
    maxImag += dY;
    break;
  case 'a':
    minReal -= dX;
    maxReal -= dX;
    break;
  case 'd':
    minReal += dX;
    maxReal += dX;
    break;
  case 'i': {
    double scale = 0.8; // zoom in small step
    ldouble marginX = ((1 - scale)/2) * (maxReal - minReal);
    ldouble marginY = ((1 - scale)/2) * (maxImag - minImag);
    minReal += marginX;
    maxReal -= marginX;
    minImag += marginY;
    maxImag -= marginY;
    break;
  }
  case 'k': {
    double scale = 1.2; // zoom out small step each size get 1.2 times larger
    ldouble marginX = ((scale - 1)/2) * (maxReal - minReal);
    ldouble marginY = ((scale - 1)/2) * (maxImag - minImag);
    minReal -= marginX;
    maxReal += marginX;
    minImag -= marginY;
    maxImag += marginY;
    break;
  }
  default:
    return;
  }
  glutPostRedisplay();
}

void recalculateRange(int x, int y, bool zoomedIn, bool atCursor, double scale) { // now only zoom in
  
  ldouble dW = (maxReal - minReal) * scale / 2;
  ldouble dH = (maxImag - minImag) * scale / 2;
  ldouble xScaled = ((double) x / width) * (maxReal - minReal) + minReal;
  ldouble yScaled = ((double) y / height) * (maxImag - minImag) + minImag;
  if (zoomedIn && atCursor) {
    minReal = xScaled - dW;
    maxReal = xScaled + dW;
    minImag = yScaled - dH;
    maxImag = yScaled + dH;
  }
  else if (!zoomedIn && atCursor) {
    minReal = 3 * minReal / 2 - maxReal / 2;
    maxReal = 3 * maxReal / 2 - minReal / 2;
    minImag = 3 * minImag / 2 - maxImag / 2;
    maxImag = 3 * maxImag / 2 - minImag / 2;
  }
  else if (zoomedIn && !atCursor) {
    ldouble marginX = ((1 - scale)/2) * (maxReal - minReal);
    ldouble marginY = ((1 - scale)/2) * (maxImag - minImag);
    minReal += marginX;
    maxReal -= marginX;
    minImag += marginY;
    maxImag -= marginY;
  }
  else {
    ldouble marginX = ((scale - 1)/2) * (maxReal - minReal);
    ldouble marginY = ((scale - 1)/2) * (maxImag - minImag);
    minReal -= marginX;
    maxReal += marginX;
    minImag -= marginY;
    maxImag += marginY;
  }
}

    /* xTemp = x * x - y * y + rScaled;
    y = 2*x*y + iScaled;
    x = xTemp; */

int getIterMandelbrot(ldouble rScaled, ldouble iScaled, int maxIter) {
  int iter = 0;
  ldouble x = 0;
  ldouble y = 0;
  ldouble xsq = 0;
  ldouble ysq = 0;
  while (iter < maxIter && xsq + ysq <= 4.0) {
    y = x * y;
    y += y;
    y += iScaled;
    x = xsq - ysq + rScaled;
    xsq = x * x;
    ysq = y * y;
    iter++;
  }
  return iter;
}
