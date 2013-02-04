#include "body_mesh.hpp"

#include <SDL/texture_loader.hpp>

namespace t_mob
{

void BodyMesh::load(class ModelLoader* ml)
{
    static int loaded = 0;
    IF_ASSERT(loaded++) return;
    this->nm = ml->_nlm;
    this->nnl = new char*[nm];
    this->npl = new int[nm];

    this->node_mTransformation = new struct Mat4[this->nm];  //mTransformation;
    this->tnode_matrix =  new struct Mat4[this->nm];

    for (int i=0; i<this->nm; i++)
    {
        GS_ASSERT(ml->_nl[i].index == i);
        this->nnl[i] = ml->_nl[i].name;
        this->npl[i] = (ml->_nl[i].p == NULL ? 0 : ml->_nl[i].p->index);
        //if (ml->_nl[i].p != NULL)
            //printf("node %d parent %d p2= %d \n", i, this->npl[i], ml->_nl[i].p->index);
        this->node_mTransformation[i] = ml->_nl[i].mTransformation;
    }

    //load bone list
    this->blm = ml->bnlm;
    this->bnl = new char*[this->blm];
    this->bone_mOffsetMatrix = new struct Mat4[this->blm];
    this->tbone_matrix = new struct Mat4[this->blm];
    this->bpl = new int[this->blm];

    for (int i=0; i<this->blm; i++)
    {
        this->bnl[i] = ml->bnl[i].name;
        this->bone_mOffsetMatrix[i] = ml->bnl[i].mOffsetMatrix;
        this->bpl[i] = ml->bnl[i].parent_index;
        IF_ASSERT(this->bpl[i] == -1)
            printf("ERROR: bone parent for bone %d is -1 \n", i);
    }

    //load meshes
    GS_ASSERT(ml->_nl[0].p == 0);
    this->mlm = ml->_mlm;
    this->ml = new class BodyPartMesh[this->mlm];
    for (int i=0; i<ml->_mlm; i++)
        this->ml[i].load(&(ml->_ml[i]));
    init_texture();
}

void BodyMesh::draw_prep()
{
    IF_ASSERT(this->nm <= 0) return;
    MALLOX(int, _set, this->nm);
    memset(_set, 0, this->nm * sizeof(int));
    _set[0] = 1;
    this->tnode_matrix[0] = this->node_mTransformation[0];

    for (int i=1; i<this->nm; i++)
    {
        if (this->npl[i] == -1) continue;
        GS_ASSERT(_set[i] == 0);
        GS_ASSERT(_set[this->npl[i]] == 1);
        this->tnode_matrix[i] = mat4_mult(this->tnode_matrix[this->npl[i]],
                                          this->node_mTransformation[i]);
        _set[i] = 1;
    }

    for (int i=0; i<this->blm; i++)
    {
        if (this->bpl[i] == -1) continue;
        GS_ASSERT(_set[this->bpl[i]] == 1);
        this->tbone_matrix[i] = mat4_mult(this->tnode_matrix[this->bpl[i]],
                                          this->bone_mOffsetMatrix[i]);
    }
}

void BodyMesh::draw(float x, float y, float z)
{
    // TODO - Move this stuff onto the mesh
    for (int i=0; i<this->mlm; i++)
    {
        class BodyPartMesh* m = &this->ml[i];
        for (int j=0; j<m->bvlm; j++)
        {
            m->tbvl[j].ux = m->bvl[j].ux;
            m->tbvl[j].uy = m->bvl[j].uy;

            m->tbvl[j].v.x = 0.0f;
            m->tbvl[j].v.y = 0.0f;
            m->tbvl[j].v.z = 0.0f;
        }

        for (int j=0; j<m->vwlm; j++)
        {
            struct _VertexWeight w = m->vwl[j];
            int bone_index   = w.bone_index;
            int vertex_index = w.vertex_index;
            float weight     = w.weight;

            GS_ASSERT(vertex_index < m->bvlm);
            GS_ASSERT(bone_index < blm);
            GS_ASSERT(vertex_index < m->bvlm);

            Vec3 v = vec3_mat3_apply(m->bvl[vertex_index].v, this->tbone_matrix[bone_index]);

            struct Mat4 out = this->tbone_matrix[bone_index];
            GS_ASSERT(out._f[0*4+3] == 0.0f && out._f[1*4+3] == 0.0f &&
                      out._f[2*4+3] == 0.0f && out._f[3*4+3] == 1.0f)

            m->tbvl[vertex_index].v.x += weight*v.x;
            m->tbvl[vertex_index].v.y += weight*v.y;
            m->tbvl[vertex_index].v.z += weight*v.z;
        }
    }

    for (int i=0; i<this->mlm; i++)
    {
        class BodyPartMesh* m = &this->ml[i];
        GS_ASSERT(m->viam == m->tvln);
        for (int j=0; j<m->viam; j++)
        {
            int index = m->via[j];
            GS_ASSERT(index < m->viam && index >= 0);
            m->tvl[j] = m->tbvl[index];
        }
    }

    glColor4ub(255,255,255,255);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_TRIANGLES);
    for (int i=0; i<this->mlm; i++)
    {
        class BodyPartMesh* m = &this->ml[i];
        for (int j=0; j<m->tvln; j++)
        {
            struct _Vertex v = m->tvl[j];
            glTexCoord2f(v.ux, v.uy);
            glVertex3f(v.v.x + x, v.v.y + y, v.v.z + z);
        }
    }
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    check_gl_error();
}

void BodyMesh::init_texture()
{
    GS_ASSERT(this->s == NULL);
    this->s = create_surface_from_file("./media/mesh/body_template.png");
    IF_ASSERT(this->s == NULL) return;
    int ret = create_texture_from_surface(this->s, &this->texture, GL_LINEAR, GL_NEAREST);
    GS_ASSERT(ret == 0);
}

}   // t_mob
