#define cimg_display 0
#define cimg_OS 1
#include <math.h>
#include <iostream>
#include <fstream>
using namespace std;

#include "CImg.h"
using namespace cimg_library;

ifstream::pos_type size;
char * memblock;

//rotate/flip a quadrant appropriately
void rot(int n, int *x, int *y, int rx, int ry) {
    if (ry == 0) {
        if (rx == 1) {
            *x = n-1 - *x;
            *y = n-1 - *y;
        }
 
        //Swap x and y
        int t  = *x;
        *x = *y;
        *y = t;
    }
}

//convert (x,y) to d
int xy2d (int n, int x, int y) {
    int rx, ry, s, d=0;
    for (s=n/2; s>0; s/=2) {
        rx = (x & s) > 0;
        ry = (y & s) > 0;
        d += s * s * ((3 * rx) ^ ry);
        rot(s, &x, &y, rx, ry);
    }
    return d;
}
 
//convert d to (x,y)
void d2xy(int n, int d, int *x, int *y) {
    int rx, ry, s, t=d;
    *x = *y = 0;
    for (s=1; s<n; s*=2) {
        rx = 1 & (t/2);
        ry = 1 & (t ^ rx);
        rot(s, x, y, rx, ry);
        *x += s * rx;
        *y += s * ry;
        t /= 4;
    }
}
 
const unsigned char A[] = { 0xD9, 0x16, 0x0C };
const unsigned char G[] = { 0xAF, 0xCE, 0x39 };
const unsigned char C[] = { 0xF6, 0x8C, 0x13 };
const unsigned char T[] = { 0x16, 0x76, 0xAE };
const unsigned char N[] = { 0x10, 0x10, 0x10 };
const int d = 8192;

unsigned char get_c(unsigned char n) {
    return n=='A' ? 0x30 :
           n=='G' ? 0x70 :
           n=='C' ? 0xB0 :
           n=='T' ? 0xF0 :
                    0x10 ;
}

int main(int argc, char* argv[]) {

  int s = d;
  int c = d*d/2;

  CImg<unsigned char> img(s,s,1,3,0);
  img.fill(0);

  ifstream fileL("genome_L.txt", ios::in|ios::binary|ios::ate);
  if (fileL.is_open()) {

    size = fileL.tellg();

    memblock = new char [size];
    fileL.seekg (0, ios::beg);
    fileL.read (memblock, size);
    fileL.close();
    int i=0;
    while(i*3+2<size && i+2<c) {
       int x, y;
       d2xy(s, c-i, &x, &y);
       unsigned char c[] = {get_c(memblock[i*3]), get_c(memblock[i*3+1]), get_c(memblock[i*3+2])};
       img.draw_point(x, y, c);
       i++;
    }   
    delete[] memblock;
  }

  ifstream fileR("genome_R.txt", ios::in|ios::binary|ios::ate);
  if (fileR.is_open()) {

    size = fileR.tellg();

    memblock = new char [size];
    fileR.seekg (0, ios::beg);
    fileR.read (memblock, size);
    fileR.close();
    int i=0;
    while(i*3+2<size && i+2<c) {
       int x, y;
       d2xy(s, c+i+1, &x, &y);
       unsigned char c[] = {get_c(memblock[i*3]), get_c(memblock[i*3+1]), get_c(memblock[i*3+2])};
       img.draw_point(x, y, c);
       i++;
    }   
    delete[] memblock;
  }
  
  img.save("genome.png");
  
  return 0;
}
