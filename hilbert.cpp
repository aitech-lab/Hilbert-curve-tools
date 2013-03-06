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
 
const unsigned char empty[] = {   0,   0,   0 };
const unsigned char color[] = { 255, 255, 255 };

int main(int argc, char* argv[]) {

  if(argc <3) {
    cout << "usage: hilbert file.bin file.png [b|g|r]" << endl << "\tb - binary" << endl << "\tg - grayscale"<< endl << "\tr - rgb"<< endl;
    return 0;
  }
  
  char method = (argc<4) ? (char)'b' : *argv[3];

  ifstream file (argv[1], ios::in|ios::binary|ios::ate);
  if (file.is_open()) {

    size = file.tellg();
    memblock = new char [size];
    file.seekg (0, ios::beg);
    file.read (memblock, size);
    file.close();

    cout << "[LOADING COMPLETE]" << endl;

    int s;
    switch (method) {
      case 'r':
        s = ceil(sqrt(size/3.0));
        break;
      case 'g':
        s = ceil(sqrt(size*1.0));
        break;
      case 'b':
      default:
        s = ceil(sqrt(size*8.0));
        break;
    }

    CImg<unsigned char> img(s,s,1,3,0);
    img.fill(0);

    switch (method) {
      case 'r':        
      for(int i=0; i<floor(size/3.0); i++) {
          unsigned char gr[] = { memblock[i*3], memblock[i*3+1], memblock[i*3+2] };
          int x, y;
          d2xy(s, i, &x, &y);
          img.draw_point(x, y, gr);
        }
        break;
      case 'g':
        for(int i=0; i<size; i++) {
          unsigned char gr[] = { memblock[i], memblock[i], memblock[i] };
          int x, y;
          d2xy(s, i, &x, &y);
          img.draw_point(x, y, gr);
        }    
        break;
      case 'b':
      default:
        int id = 0;
        for(int i=0; i<size; i++) {
           for(int j=0;j<8; j++) {
              int x, y;
              d2xy(s, id, &x, &y);
              if(memblock[i] & (1 << j)) {    
                img.draw_point(x,y,color);
              }
              id++;
           }
        }    
        break;
    }

    img.save(argv[2]);

    delete[] memblock;
  } else cout << "Unable to open file" << endl;
  return 0;
}
