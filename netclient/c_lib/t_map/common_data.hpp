#pragma once

namespace t_map
{
    struct ColorElement
    {
        union
        {
            struct
            {
                unsigned char r,g,b,a;
            };
            unsigned int color;
        };
    };

    struct AOElement
    {
        union
        {
            struct
            {
                unsigned char ao[4]; //32
            };
            unsigned int AO;
        };
    };

    /*
        Replace x,y,z and tx,ty,tz by char
        to reduce size from x 36 to 16 bytes per vertex for normal blocks
    */
    struct Vertex {
        float x,y,z;
        float tx,ty,tz; //12

        union
        {
            struct
            {
                unsigned char r,g,b,a;
            };
            unsigned int color;
        };

        union
        {
            char normal[4]; //16
            unsigned int n;
        };

        union
        {
            unsigned char ao[4]; //32
            unsigned int AO;
        };
    };
}