#ifndef CONFIG_H
#define CONFIG_H

#define width 1280
#define height 960

// Bigger iter give more accurate image but slower render
int maxIter = 700;

// For computer with multi core, can use this to increase performance
bool multiThread = true;
int numThread = 32;


#endif
