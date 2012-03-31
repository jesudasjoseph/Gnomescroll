#include "circuit_tree.hpp"
#pragma once

#ifdef DC_CLIENT
    #include <compat_gl.h>
#endif

#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>

/*
 *  Client only
 */
#ifdef DC_CLIENT

//GLint particle_sheet_id;

struct p_tree_node {
    int depth;
    int s[3];
    int e[3];
};

struct p_tree_node p_array[2048];
int pt_i = 0;

struct p_tree_node* new_branch(struct p_tree_node* n, int depth, int dx,int dy,int dz) {
    if(n==NULL) return NULL;
    if(pt_i >= 2048) return NULL;

    p_array[pt_i].depth = depth;
    p_array[pt_i].s[0] = n->e[0];
    p_array[pt_i].s[1] = n->e[1];
    p_array[pt_i].s[2] = n->e[2];
    p_array[pt_i].e[0] = n->e[0]+dx;
    p_array[pt_i].e[1] = n->e[1]+dy;
    p_array[pt_i].e[2] = n->e[2]+dz;
    pt_i++;
    return &p_array[pt_i-1];
}

void p_tree_recursive(struct p_tree_node* n) {
    if(n==NULL) return;
    if(n->depth > 4) return;

    int dx,dy,dz;

    struct p_tree_node* p;
    int a,b,l;

    int i, r;
    r = (rand() % 3)+2;
    r=2;
    //printf("Branch %i times\n", r);
    for(i=0;i<r;i++) {
        //printf("x\n");
        dx=0;
        dy=0;
        dz=0;

        a = rand() %2;
        a = (a==1 ? 1 : -1);

        l = 1 + (rand() %2);

        b = rand() %3;
        if(b == 0) {
            dx = l*a;
        }
        if(b==1) {
            dy = l*a;
        }
        if(b==2) {
            dz = l; //only grow up
        }

        /*
        dx = -3+(rand() % 6);
        dy = -3+(rand() % 6);
        dz = -2+(rand() % 6);
        */
        p = new_branch(n, n->depth+1, dx,dy,dz);
        p_tree_recursive(p);


    }
}

void p_tree_recursive2(struct p_tree_node* n) {
    if(n==NULL) return;
    if(n->depth > 10) return;

    int dx,dy,dz;

    struct p_tree_node* p;
    int a,b,l;

    int i, r;
    r = (rand() % 3)+2;
    r=2;
    //printf("Branch %i times\n", r);
    for(i=0;i<r;i++) {
        //printf("x\n");
        dx=0;
        dy=0;
        dz=0;

        a = rand() %2;
        a = (a==1 ? 1 : -1);

        l = 1 + (rand() %2);

        b = rand() %3;
        if(b == 0) {
            dx = l*a;
        }
        if(b==1) {
            dy = l*a;
        }
        if(b==2) {
            dz = l; //only grow up
        }

        /*
        dx = -3+(rand() % 6);
        dy = -3+(rand() % 6);
        dz = -2+(rand() % 6);
        */
        p = new_branch(n, n->depth+abs(dx)+abs(dy)+abs(dz), dx,dy,dz);
        p_tree_recursive(p);


    }
}

void circuit_tree_generate(int type, int seed) {
    //printf("Generated Circuit Tree\n");
    //printf("RANDMAX= %i \n", RAND_MAX);
    //srand(seed); //seed
    //rand()
    pt_i = 1;

    p_array[0].depth = 0;
    p_array[0].s[0] = 10;
    p_array[0].s[1] = 10;
    p_array[0].s[2] = 10;

    p_array[0].e[0] = 10;
    p_array[0].e[1] = 10;
    p_array[0].e[2] = 15;

    p_tree_recursive(&p_array[0]);
}

void circuit_tree_draw() {
    //printf("Drawing Circuit Tree: %i nodes\n", pt_i);
    int i;
    //float x0,y0,z0;
    //float x1,y1,z1;

    struct p_tree_node* p;
    glLineWidth(15.0);
    //glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBegin(GL_LINES);
    glColor4ub((unsigned char)150,(unsigned char)0,(unsigned char)150, (unsigned char)100);
    for(i=0;i<pt_i;i++) {
        p = &p_array[i];

        /*
        if(p->depth % 2 == 0) {
            glColor3ub((unsigned char)0,(unsigned char)0,(unsigned char)255);
        } else {
            glColor3ub((unsigned char)255,(unsigned char)0,(unsigned char)255);
        }
        */
        //glColor3ub((unsigned char)255,(unsigned char)0,(unsigned char)0);
        glVertex3f(p->s[0]+0.5,p->s[1]+0.5,p->s[2]+0.5);
        //glColor3ub((unsigned char)0,(unsigned char)255,(unsigned char)0);
        glVertex3f(p->e[0]+0.5,p->e[1]+0.5,p->e[2]+0.5);
        //glVertex3f(p->s[0],p->s[1],p->s[2]);
        //glVertex3f(p->e[0],p->e[1],p->e[2]);
    }
    glEnd();
    glLineWidth(1);
    glDisable(GL_BLEND);
    glColor3ub((unsigned char)255,(unsigned char)255,(unsigned char)255);
    //glDisable(GL_LINE_SMOOTH);
}

/*
void neutron_draw() {
    //printf("particle sheet id= %i \n", particle_texture_id );
    if(neutron_count == 0) { return; }
    glGetFloatv(GL_MODELVIEW_MATRIX, a);

    struct neutron* g = NULL;
    int i;

    float size = 0.3;
    float up[3] = {a[0]*size, a[4]*size, a[8]*size};
    float right[3] = {a[1]*size, a[5]*size, a[9]*size};
    int id = 5;

    float tx_min, tx_max, ty_min, ty_max;

    //should not change state unless there is something to draw
    glEnable(GL_TEXTURE_2D);
    glEnable (GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glBindTexture( GL_TEXTURE_2D, particle_texture_id );
    //printf("particle sheet= %i \n", particle_sheet_id);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);

    glBegin( GL_QUADS );
    float x,y,z;

    int _c = 0;
    for(i=0; i<1024; i++) {
    if(neutron_list[i] != NULL) {
        //printf("draw neutron: %i \n", i);
        _c++;
        g = neutron_list[i];
        //draw setup
        id = 48+3*g->type+ (g->energy-1);
        tx_min = (float)(id%16)* (1.0/16.0);
        tx_max = tx_min + (1.0/16.0);
        ty_min = (float)(id/16)* (1.0/16.0);
        ty_max = ty_min + (1.0/16.0);

        x=g->x; y=g->y; z=g->z;

        glTexCoord2f(tx_min,ty_max );
        glVertex3f(x+(-right[0]-up[0]), y+(-right[1]-up[1]), z+(-right[2]-up[2]));  // Bottom left

        glTexCoord2f(tx_min,ty_min );
        glVertex3f(x+(up[0]-right[0]), y+(up[1]-right[1]), z+(up[2]-right[2]));  // Top left

        glTexCoord2f(tx_max,ty_min);
        glVertex3f(x+(up[0]+right[0]), y+(up[1]+right[1]), z+(up[2]+right[2]));  // Top right

        glTexCoord2f(tx_max,ty_max );
        glVertex3f(x+(right[0]-up[0]), y+(right[1]-up[1]), z+(right[2]-up[2]));  // Bottom right
        }
    }
    //printf("drew %i gernades\n", _c);
    glEnd();

    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);
    glDisable (GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}
*/
#endif