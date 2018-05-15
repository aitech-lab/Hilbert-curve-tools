#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define WIDTH 80

void rot(int n, int *x, int *y, int rx, int ry);
int  xy2d (int n, int x, int y);
void d2xy(int n, int d, int *x, int *y);

int main(int argc, char** argv) {
    int p = 6;
    int s=1<<p;
    while(1) {
        printf("---\n");
        FILE* in = popen("tmux capture-pane -pJ", "r");

        unsigned char buf[s*s+1];
        for(int i=0; i<s*s; i++) buf[i] = '.';
        buf[s*s] = 0;
        
        unsigned char c;
        int i=0;
        while(c = fgetc(in)) {
            if(feof(in)) break;
            int x, y;
            d2xy(s, i, &x, &y);
            if(c=='\n') c='~';
            buf[x+y*s]=c;
            i++;
        }

        for(int y=0; y<32; y++){
           for(int x=0; x<s; x++) {
              printf("%c", buf[x+y*s]);
           }
           printf("\n");
        }

        pclose(in);
        sleep(1);
    }
}

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

