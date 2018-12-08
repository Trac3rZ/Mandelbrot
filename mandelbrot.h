#include "config.h"

typedef long double ldouble;


static ldouble minReal = -2.5;
static ldouble maxReal = 0.7;
static ldouble minImag = -1.2;
static ldouble maxImag = 1.2;



int getIterMandelbrot(ldouble, ldouble, int);
void recalculateRange(int, int, bool, bool, double);
void displayMandelbrotSingleThread(void);
void displayMandelbrotMultiThread(void);
void mouseClicks(int, int, int, int);
void keyboardFunction(unsigned char, int, int);
int getIterMandelbrot(ldouble, ldouble, int);
void fillMandelbrot(int a[width][height], int);
void fillMandelbrotPartial(int arr[][height], int, int);
