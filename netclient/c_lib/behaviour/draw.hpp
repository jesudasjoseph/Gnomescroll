#pragma once

#if DC_CLIENT
#include <c_lib/camera/camera.hpp>
#endif

template <class Super, class Object>
class DrawBillboardSprite: public Super
{
    protected:
    inline void draw(ObjectState* state, Object* object)
    {
        #if DC_CLIENT
        Vec3 v = state->vp->p;
        v.z += state->texture_scale;
        
        if (current_camera == NULL
        || !current_camera->in_view(v.x, v.y, v.z))
            return;

        Vec3 up = vec3_init(
            model_view_matrix[0]*state->texture_scale,
            model_view_matrix[4]*state->texture_scale,
            model_view_matrix[8]*state->texture_scale
        );
        Vec3 right = vec3_init(
            model_view_matrix[1]*state->texture_scale,
            model_view_matrix[5]*state->texture_scale,
            model_view_matrix[9]*state->texture_scale
        );

        float tx_min, tx_max, ty_min, ty_max;
        tx_min = (float)(state->texture_index%16)* (1.0/16.0);
        tx_max = tx_min + (1.0/16.0);
        ty_min = (float)(state->texture_index/16)* (1.0/16.0);
        ty_max = ty_min + (1.0/16.0);

        Vec3 p = vec3_sub(v, vec3_add(right, up));
        glTexCoord2f(tx_min,ty_max);
        glVertex3f(p.x, p.y, p.z);

        p = vec3_add(v, vec3_sub(up, right));
        glTexCoord2f(tx_max,ty_max);
        glVertex3f(p.x, p.y, p.z);

        p = vec3_add(v, vec3_add(up, right));
        glTexCoord2f(tx_max,ty_min);
        glVertex3f(p.x, p.y, p.z);

        p = vec3_add(v, vec3_sub(right, up));
        glTexCoord2f(tx_min,ty_min);
        glVertex3f(p.x, p.y, p.z);
        #endif
        Super::draw(state, object);
    }
};