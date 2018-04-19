#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include "kvec.h"

typedef unsigned char byte;

typedef struct mask_t {
   uint32_t netip[4];
   uint32_t prefix;
} mask_t;

// masks_vector;
typedef kvec_t(struct mask_t) masks_v;

int parse_mask(byte* line, mask_t* mask) {
    if(strlen(line)<9) return -1;
    int i = 0;
    // parse ip
    while (*line!=0 && *line!='/') {
        if (isdigit((byte)*line)) {
            mask->netip[i] *= 10;
            mask->netip[i] += *line - '0';
        } else {
            i++;
        }
        line++;
    }
    line++;

    // parse mask
    while (*line!=0 && *line!=' ' && *line!='\n') {
        if (isdigit((byte)*line)) {
            mask->prefix *= 10;
            mask->prefix += *line - '0';
        }
        line++;
    }
    return 0;
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

masks_v masks;

int masked(uint32_t i0, uint32_t i1) {

    for(size_t i=0; i<masks.n; i++) {
        uint32_t mask = (-1) << (32 - masks.a[i].prefix);
        uint32_t* netip = masks.a[i].netip;
        uint32_t netstart = netip[0]<<24 | netip[1]<<16 | netip[2]<<8 | netip[3] & mask;
        uint32_t netend = netstart | ~mask;
        uint32_t ip = i0<<24 | i1<<16;
        if ((ip >= netstart) && (ip <= netend)) return 1;
    }
    return 0;
}

int main(){

    kv_init(masks);

    char *line = NULL;
    size_t size;

    FILE* in = fopen("blocked.txt", "r");
    while(getline(&line, &size, in) > 0) {
        mask_t mask = {{0},0};
        if(!parse_mask(line, &mask)) {
            fprintf(stderr, "%s\t%d.%d.%d.%d/%d\n", line,
                mask.netip[0], mask.netip[1], mask.netip[2], mask.netip[3], mask.prefix);
            kv_push(mask_t, masks, mask); 
        };
    };
    fclose(in);
    
    int x,y;
    size_t image_size = 2<<16;
    byte* image = calloc(1,image_size);
    byte i0=0, i1=0;
    do {
        do{ 
            if(masked(i0, i1)) {
                d2xy(256, i0*256+i1, &x, &y);
                image[x+y*256] = 255;
            }
        } while(++i0 != 0);
    } while(++i1 != 0);

    kv_destroy(masks);
    free(line);

    FILE *const out = fdopen(dup(fileno(stdout)), "wb");
    fwrite(image, image_size, 1, out);
    fclose(out);
    free(image);
        
}


