/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "_interface.hpp"

#include <entity/constants.hpp>
#include <hud/container/constants.hpp>
#include <hud/container/inventory.hpp>
#include <hud/container/toolbelt.hpp>
#include <hud/container/synthesizer.hpp>
#include <hud/container/storage_block.hpp>
#include <hud/container/crusher.hpp>
#include <hud/container/cache.hpp>

namespace HudContainer
{

static bool agent_inventory_enabled = false;
static bool container_block_enabled = false;
static ItemContainerID container_block_enabled_id = NULL_CONTAINER;
float mouse_x = -1;
float mouse_y = -1;
bool lm_down = false;

// private containers
class AgentInventoryUI* inventory = NULL;
class AgentToolbeltUI* toolbelt = NULL;
class AgentSynthesizerUI* synthesizer = NULL;
class EnergyTanksUI* energy_tanks = NULL;
class CacheUI* cache = NULL;
class EquipmentUI* equipment = NULL;

// public containers
class CraftingUI* crafting_bench = NULL;
class StorageBlockUI* storage_block = NULL;
class SmelterUI* smelter = NULL;
class CrusherUI* crusher = NULL;

// array holding all those containers for convenient lookup
UIElement** ui_elements = NULL;

UIElement* get_ui_element(ItemContainerType type)
{
    IF_ASSERT(!isValid(type)) return NULL;
    return ui_elements[type];
}

void set_container_id(ItemContainerType container_type, ItemContainerID container_id)
{
    if (container_type == ItemContainer::name::hand) return;
    UIElement* container_ui = get_ui_element(container_type);
    IF_ASSERT(container_ui == NULL) return;
    container_ui->set_container_id(container_id);
    container_ui->set_container_type(container_type);
    container_ui->name.set_text(ItemContainer::get_container_pretty_name(container_type));
}

void close_container(ItemContainerID container_id)
{
    // unset ids for free-world container UIs
    bool closed = false;
    for (size_t i=0; i<MAX_CONTAINER_TYPES; i++)
    {
        if (ui_elements[i] == NULL) continue;
        if (ItemContainer::container_type_is_attached_to_agent((ItemContainerType)i)) continue;
        if (ui_elements[i]->container_id != NULL_CONTAINER) closed = true;
        ui_elements[i]->container_id = NULL_CONTAINER;
    }
    if (container_block_enabled && closed)
        disable_container_block_hud();
}

// Input Handling

void enable_agent_inventory_hud()
{
    GS_ASSERT(!container_block_enabled);
    agent_inventory_enabled = true;
}

void disable_agent_inventory_hud()
{
    mouse_x = -1;
    mouse_y = -1;
    agent_inventory_enabled = false;
}

void enable_container_block_hud(ItemContainerID container_id)
{
    GS_ASSERT(!agent_inventory_enabled);
    GS_ASSERT(container_id != NULL_CONTAINER);
    GS_ASSERT(container_block_enabled_id == NULL_CONTAINER);
    container_block_enabled = true;
    container_block_enabled_id = container_id;
}

void disable_container_block_hud()
{
    mouse_x = -1;
    mouse_y = -1;
    container_block_enabled = false;
    container_block_enabled_id = NULL_CONTAINER;
}

static UIElement* get_container_and_slot(int x, int y, int* slot)
{
    UIElement* closest_container = NULL;
    int closest_slot = NULL_SLOT;
    // get topmost container click
    // WARNING: doesnt support overlapping containers yet.
    for (size_t i=0; i<MAX_CONTAINER_TYPES; i++)
    {
        UIElement* container = ui_elements[i];
        if (container == NULL) continue;
        if (container->container_id == NULL_CONTAINER) continue;
        if (!container->point_inside(x,y)) continue;
        closest_container = container;
        closest_slot = container->get_slot_at(x,y);
    }

    *slot = closest_slot;
    return closest_container;
}


static ContainerInputEvent get_container_hud_ui_event(int x, int y)
{
    int slot = NULL_SLOT;
    UIElement* container = get_container_and_slot(x,y, &slot);
    ItemContainerID container_id = NULL_CONTAINER;
    if (container != NULL) container_id = container->container_id;

    ContainerInputEvent event;
    event.container_id = container_id;
    event.slot = slot;
    event.alt_action = (container != NULL &&
                       ((container->type == UI_ELEMENT_SYNTHESIZER_CONTAINER &&
                       ((AgentSynthesizerUI*)container)->in_shopping_region(x,y)) ||
                       (container->type == UI_ELEMENT_CRAFTING_CONTAINER &&
                       ((CraftingUI*)container)->in_craft_output_region(x,y)) ||
                       (container->type == UI_ELEMENT_CRUSHER &&
                       ((CrusherUI*)container)->in_button_region(x,y))));
    return event;
}

// returns item type at screen position x,y
static ItemType get_item_type_at(int x, int y)
{
    int slot;
    UIElement* ui = get_container_and_slot(x,y, &slot);
    if (ui == NULL) return NULL_ITEM_TYPE;
    if (ui->container_id == NULL_CONTAINER) return NULL_ITEM_TYPE;
    if (slot == NULL_SLOT) return NULL_ITEM_TYPE;

    using ItemContainer::ItemContainerUIInterface;
    ItemContainerUIInterface* container = ItemContainer::get_container_ui(ui->container_id);
    if (container == NULL) return NULL_ITEM_TYPE;

    if (ui->type == UI_ELEMENT_SYNTHESIZER_CONTAINER)
    {
        using ItemContainer::ItemContainerSynthesizerUI;
        if (((AgentSynthesizerUI*)ui)->in_shopping_region(x,y))
        {
            int xslot = slot % ((AgentSynthesizerUI*)ui)->shopping_xdim;
            int yslot = slot / ((AgentSynthesizerUI*)ui)->shopping_xdim;
            return Item::get_synthesizer_item(xslot, yslot);
        }
        else if (((AgentSynthesizerUI*)ui)->in_coins_region(x,y))
            return ((ItemContainerSynthesizerUI*)container)->get_coin_type();
        return NULL_ITEM_TYPE;
    }

    if (ui->type == UI_ELEMENT_CRAFTING_CONTAINER)
    {
        if (((CraftingUI*)ui)->in_craft_output_region(x,y))
            return Item::get_selected_craft_recipe_type(container->id, slot);
    }

    return container->get_slot_type(slot);
}

static const ContainerInputEvent NULL_EVENT = {
    NULL_CONTAINER,         // null container id
    NULL_SLOT,              // null slot
    false,                  // alt action
};

ContainerInputEvent left_mouse_down(int x, int y)
{
    set_mouse_position(x, y);
    lm_down = true;
    return NULL_EVENT;
}

ContainerInputEvent left_mouse_up(int x, int y)
{
    set_mouse_position(x, y);
    lm_down = false;
    return get_container_hud_ui_event(x,y);
}

ContainerInputEvent right_mouse_down(int x, int y)
{
    set_mouse_position(x, y);
    return NULL_EVENT;
}

ContainerInputEvent right_mouse_up(int x, int y)
{
    set_mouse_position(x, y);
    return get_container_hud_ui_event(x,y);
}

ContainerInputEvent mouse_motion(int x, int y)
{
    set_mouse_position(x, y);
    return NULL_EVENT;
}

void set_mouse_position(int x, int y)
{
    mouse_x = x;
    mouse_y = y;
}

ContainerInputEvent scroll_up()
{
    if (toolbelt == NULL) return NULL_EVENT;
    toolbelt->selected_slot -= 1;
    toolbelt->selected_slot %= toolbelt->xdim;
    if (toolbelt->selected_slot < 0) toolbelt->selected_slot += toolbelt->xdim;

    ContainerInputEvent event;
    event.container_id = toolbelt->container_id;
    event.slot = toolbelt->selected_slot;
    event.alt_action = false;
    return event;
}

ContainerInputEvent scroll_down()
{
    if (toolbelt == NULL) return NULL_EVENT;
    toolbelt->selected_slot += 1;
    toolbelt->selected_slot %= toolbelt->xdim;

    ContainerInputEvent event;
    event.container_id = toolbelt->container_id;
    event.slot = toolbelt->selected_slot;
    event.alt_action = false;
    return event;
}

ContainerInputEvent select_slot(int numkey)
{
    if (toolbelt == NULL) return NULL_EVENT;

    if (numkey == 0) numkey = 10;
    int slot = numkey-1;
    if (slot < 0 || slot >= toolbelt->xdim) return NULL_EVENT;
    toolbelt->selected_slot = slot;

    ContainerInputEvent event;
    event.container_id = toolbelt->container_id;
    event.slot = slot;
    event.alt_action = false;
    return event;
}

// Drawing

static HudText::Text* grabbed_icon_stack_text = NULL;
static HudText::Text* tooltip_text = NULL;

static void draw_grabbed_icon()
{
    IF_ASSERT(TextureSheetLoader::item_texture_sheet_loader->texture == 0) return;
    using ItemContainer::player_hand_ui;
    if (player_hand_ui == NULL) return;
    struct ItemContainer::SlotMetadata hand_metadata = player_hand_ui->get_item_metadata();
    if (hand_metadata.type == NULL_ITEM_TYPE) return;

    const float w = float(ITEM_ICON_RENDER_SIZE);

    // center icon on mouse position
    float x = mouse_x - (w / 2);
    float y = _yresf - (mouse_y + (w / 2));

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // render durability
    const unsigned char alpha = 128;
    draw_durability_meter(x, y, w, alpha, hand_metadata);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    glColor4ub(255, 255, 255, 255);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, TextureSheetLoader::item_texture_sheet_loader->texture);

    glBegin(GL_QUADS);

    int tex_id = Item::get_sprite_index_for_type(hand_metadata.type);

    const float iw = 16.0f; // icon_width
    const int iiw = 16; // integer icon width

    const float tx_min = (1.0f/iw)*(tex_id % iiw);
    const float ty_min = (1.0f/iw)*(tex_id / iiw);
    const float tx_max = tx_min + 1.0f/iw;
    const float ty_max = ty_min + 1.0f/iw;

    glTexCoord2f(tx_min, ty_min);
    glVertex2f(x,y+w);

    glTexCoord2f(tx_max, ty_min);
    glVertex2f(x+w, y+w);

    glTexCoord2f(tx_max, ty_max);
    glVertex2f(x+w, y);

    glTexCoord2f(tx_min, ty_max);
    glVertex2f(x, y);

    glEnd();
    glDisable(GL_TEXTURE_2D);

    // Draw stack numbers
    IF_ASSERT(grabbed_icon_stack_text == NULL) return;
    HudFont::start_font_draw(GL_ONE_MINUS_DST_COLOR);
    const int font_size = 12;
    HudFont::set_properties(font_size);
    HudFont::set_texture();
    x = x + (w/2) + font_size - 7;
    y = y + (w/2) - font_size - 2;
    draw_slot_numbers(grabbed_icon_stack_text, x, y, hand_metadata.stack_size, hand_metadata.charges);
    HudFont::reset_default();
    HudFont::end_font_draw();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

static void draw_tooltip()
{
    using ItemContainer::player_hand_ui;
    // dont draw tooltips if we're holding something
    if (player_hand_ui == NULL || player_hand_ui->get_item_type() != NULL_ITEM_TYPE) return;

    // get item type hovered
    ItemType item_type = get_item_type_at(mouse_x, mouse_y);
    if (item_type == NULL_ITEM_TYPE) return;

    // get name
    const char* name = Item::get_item_pretty_name(item_type);
    IF_ASSERT(name == NULL) return;
    IF_ASSERT(name[0] == '\0') return;

    HudFont::start_font_draw(GL_ONE_MINUS_DST_COLOR);
    const int font_size = 12;
    HudFont::set_properties(font_size);
    HudFont::set_texture();

    // align
    tooltip_text->set_position(mouse_x + 15, _yresf-mouse_y);

    tooltip_text->set_text(name);
    tooltip_text->draw();

    HudFont::reset_default();
    HudFont::end_font_draw();
}

void draw()
{
    toolbelt->draw();
    energy_tanks->inv_open = (agent_inventory_enabled || container_block_enabled);
    energy_tanks->draw();

    if (!agent_inventory_enabled && !container_block_enabled) return;

    energy_tanks->draw_name();
    //toolbelt->draw_name();
    inventory->draw();
    synthesizer->draw();
    equipment->draw();
    if (cache->container_id != NULL_CONTAINER)
        cache->draw();

    if (container_block_enabled)
    {
        GS_ASSERT(container_block_enabled_id != NULL_CONTAINER);
        ItemContainer::ItemContainerInterface* container = ItemContainer::get_container(container_block_enabled_id);
        //GS_ASSERT(container != NULL);
        if (container != NULL)
        {
            GS_ASSERT(container->type != NULL_CONTAINER_TYPE);
            if (isValid(container->type))
            {
                GS_ASSERT(ui_elements[container->type] != NULL);
                if (ui_elements[container->type] != NULL)
                {
                    ui_elements[container->type]->set_container_type(container->type);
                    ui_elements[container->type]->draw();
                }
            }
        }
    }

    draw_grabbed_icon();
    draw_tooltip();
}

/* Main init/teardown */

void init()
{
    synthesizer = new AgentSynthesizerUI;
    synthesizer->type = UI_ELEMENT_SYNTHESIZER_CONTAINER;
    synthesizer->init();
    synthesizer->xoff = (_xresf - synthesizer->width())/2;
    synthesizer->yoff = 120.0f + (_yresf + synthesizer->height())/2;

    inventory = new AgentInventoryUI;
    inventory->type = UI_ELEMENT_AGENT_INVENTORY;
    inventory->init();
    inventory->xoff = (_xresf - inventory->width())/2 + 1;  // +1 because the width is odd with odd valued inc1 and even valued xdim
    inventory->yoff = _yresf - (synthesizer->yoff - synthesizer->height() - 18);

    toolbelt = new AgentToolbeltUI;
    toolbelt->type = UI_ELEMENT_AGENT_TOOLBELT;
    toolbelt->init();
    toolbelt->xoff = (_xresf - toolbelt->width())/2;
    toolbelt->yoff = _yresf - (toolbelt->height());

    energy_tanks = new EnergyTanksUI;
    energy_tanks->type = UI_ELEMENT_ENERGY_TANKS;
    energy_tanks->init();
    energy_tanks->xoff = ((_xresf - toolbelt->width())/2);
    energy_tanks->yoff = 0;

    crafting_bench = new CraftingUI;
    crafting_bench->type = UI_ELEMENT_CRAFTING_CONTAINER;
    crafting_bench->init();
    crafting_bench->xoff = inventory->xoff + inventory->width()+ 20;
    crafting_bench->yoff = _yresf - inventory->yoff - 2;

    storage_block = new StorageBlockUI;
    storage_block->type = UI_ELEMENT_STORAGE_BLOCK;
    storage_block->set_container_type(ItemContainer::name::storage_block_small);
    storage_block->init();
    storage_block->xoff = inventory->xoff + inventory->width()+ 20;
    storage_block->yoff = _yresf - inventory->yoff - 2;

    smelter = new SmelterUI;
    smelter->type = UI_ELEMENT_SMELTER;
    smelter->set_container_type(ItemContainer::name::smelter_basic);
    smelter->init();
    smelter->xoff = inventory->xoff + inventory->width()+ 20;
    smelter->yoff = _yresf - inventory->yoff - 2;

    crusher = new CrusherUI;
    crusher->type = UI_ELEMENT_CRUSHER;
    crusher->set_container_type(ItemContainer::name::crusher);
    crusher->init();
    crusher->xoff = inventory->xoff + inventory->width()+ 20;
    crusher->yoff = _yresf - inventory->yoff - 2;

    cache = new CacheUI;
    cache->type = UI_ELEMENT_CACHE;
    cache->set_container_type(ItemContainer::name::premium_cache);
    cache->init();
    cache->xoff = inventory->xoff - (cache->width() + 22);
    cache->yoff = inventory->yoff;

    equipment = new EquipmentUI;
    equipment->type = UI_ELEMENT_EQUIPMENT;
    equipment->set_container_type(ItemContainer::name::equipment);
    equipment->init();
    // coordinates are point to top left on screen, where y=0 is along the bottom
    equipment->xoff = inventory->xoff - (equipment->width+ 20);
    equipment->yoff = synthesizer->yoff;

    grabbed_icon_stack_text = new HudText::Text;
    grabbed_icon_stack_text->set_format("%d");
    grabbed_icon_stack_text->set_format_extra_length(11 + 1 - 2);
    grabbed_icon_stack_text->set_color(Color(255,255,255,255));
    grabbed_icon_stack_text->set_depth(-0.1f);

    tooltip_text = new HudText::Text;
    tooltip_text->set_color(Color(255,255,255,255));
    tooltip_text->set_depth(-0.1f);

    // pack ui elements into the array
    GS_ASSERT(ui_elements == NULL);
    ui_elements = (UIElement**)calloc(MAX_CONTAINER_TYPES, sizeof(UIElement*));

    ui_elements[ItemContainer::name::inventory] = inventory;
    ui_elements[ItemContainer::name::toolbelt] = toolbelt;
    ui_elements[ItemContainer::name::energy_tanks] = energy_tanks;
    ui_elements[ItemContainer::name::premium_cache] = cache;
    ui_elements[ItemContainer::name::synthesizer] = synthesizer;
    ui_elements[ItemContainer::name::crafting_bench_basic] = crafting_bench;
    ui_elements[ItemContainer::name::storage_block_small] = storage_block;
    ui_elements[ItemContainer::name::cryofreezer_small] = storage_block;    // both use storage block instance
    ui_elements[ItemContainer::name::smelter_basic] = smelter;
    ui_elements[ItemContainer::name::equipment] = equipment;
    ui_elements[ItemContainer::name::crusher] = crusher;
}

void teardown()
{
    delete inventory;
    delete toolbelt;
    delete synthesizer;
    delete crafting_bench;
    delete storage_block;
    delete smelter;
    delete energy_tanks;
    delete crusher;
    delete cache;
    delete equipment;
    delete grabbed_icon_stack_text;
    delete tooltip_text;
    free(ui_elements);
}

void draw_durability_meter(float x, float y, int slot_size, unsigned char alpha,
                           const struct ItemContainer::SlotMetadata& metadata)
{
    int durability = metadata.durability;
    if (durability == NULL_DURABILITY) return;
    int max_durability = Item::get_max_durability(metadata.type);
    float ratio = float(durability)/float(max_durability);
    ratio = GS_MIN(ratio, 1.0f);
    ratio = GS_MAX(ratio, 0.0f);
    int mh = slot_size / 8; // meter height
    glColor4ub(255, 0, 0, alpha);               // red
    Hud::meter_graphic.draw(x, y, slot_size, mh, 1.0f); // full slot width background
    Hud::set_color_from_ratio(ratio, alpha);
    Hud::meter_graphic.draw(x, y, slot_size, mh, ratio);
}

void draw_init()
{
    init_texture();
}

void draw_teardown()
{
    teardown_texture();
}

void draw_tracking_pixel(float x, float y)
{
    // Draw dot in upper left corner
    GL_ASSERT(GL_TEXTURE_2D, false);

    glColor4ub(255, 0, 0, 255);

    float p = 1.0f;
    glBegin(GL_QUADS);

    glVertex2f(x-p, y+p);
    glVertex2f(x+p, y+p);
    glVertex2f(x+p, y-p);
    glVertex2f(x-p, y-p);

    glEnd();
}

}   // HudContainer
