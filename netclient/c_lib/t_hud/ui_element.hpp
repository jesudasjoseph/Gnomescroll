#pragma once

#include <t_hud/constants.hpp>
#include <hud/text.hpp>

namespace t_hud
{


class UIElement
{
        public:

        bool visible;
        float xoff;
        float yoff;

        int container_id;

        HudText::Text* stack_numbers;

        void set_position(float x, float y)
        {
            xoff = x;
            yoff = y;
        }

        virtual void init() = 0;
        
        virtual void draw() = 0;
        virtual int get_slot_at(int px, int py) = 0;

        UIElement()
        : visible(false), xoff(0), yoff(0), container_id(-1),
        stack_numbers(NULL)
        {}
    
        virtual ~UIElement() {} //abc virtual deconstructor
};

} // t_hud
