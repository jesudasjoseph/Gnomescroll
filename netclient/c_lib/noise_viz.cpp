#include <c_lib/noise_viz.hpp>
//srand((unsigned)time(0));

static const int num_bins = 100;
static int bin2[num_bins];

const float bin_min = 0;
const float bin_inc = 0.01;

float bin_scale;

static int less_than_min = 0;
static int greater_than_max = 0;

float noise_viz_test[2000];
void noise_viz_test_setup() {
    
    int i;

    for(i=0; i< 2000; i++) {
        noise_viz_test[i] = (float)rand()/(float)RAND_MAX;
        //printf("%f \n", noise_viz_test[i]);
    }
/*
    for(i=0; i< 2000; i++) {
        printf("%i= %f \n", i, noise_viz_test[i]);
    }
*/
    noise_viz_setup_bin(noise_viz_test, 2000);
}
inline void bin_float(float x) {
    if(x < bin_min) {
        less_than_min++;
        return;
    }
    int index = (x-bin_min) / bin_inc;
    if(index > num_bins){
        greater_than_max++;
        return;    
    }
    bin2[index]++;
}

void noise_viz_setup_bin(float* arr, int n) {
    printf("===\n");
    printf("Starting Binning\n");
    int i;
    for(i=0; i<num_bins;i++) bin2[i] = 0;
    double average = 0; 
    for(i=0; i<n; i++) {
        average += arr[i];
        //printf("%f \n", arr[n]);
        bin_float(arr[i]);
    }
    //average = average / ((double)(n));

    printf("number of samples= %i\n", n);
    printf("average= %f, sum= %f \n", average/n, average);
    printf("less_than_min= %i \n",less_than_min);
    printf("greater_than_max= %i \n", greater_than_max);
    printf("finished binning\n");
    printf("===\n");
    bin_scale = ((float)(n))/1000;
}

#define _C 0.5
#ifdef DC_CLIENT
void draw_noise_viz_histrogram(float x, float y, float z) {
    //printf("po=%i\n", PO_L);
    x -= 10;
    y += 10;
    int i,j;

    //printf("draw\n");

    glBegin(GL_POINTS);
    glColor3ub((unsigned char) 0,(unsigned char)0,(unsigned char)255);
    for(i=0; i<num_bins; i++) {
        //printf("bin[%i]=%i\n", i,bin[i]);
        for(j=0; j<bin2[i]; j++) {
            glVertex3f(_C+x+2*j,_C+y+2*i,z);
        }
    }

    glColor3ub((unsigned char) 200,(unsigned char)0,(unsigned char)0);
    for(i=0; i<8; i++) {
        glVertex3f(_C+x-2,_C+y+2*10*i,z);
        glVertex3f(_C+x-3,_C+y+2*10*i,z);
        glVertex3f(_C+x-4,_C+y+2*10*i,z);
    }
    glEnd();

    //printf("time= %i\n", get_current_netpeer_time());
}
#endif