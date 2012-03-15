''' User '''
name = 'HaltingState'
alt_name = '[%s]' % (name,)

''' Network '''
#server = '127.0.0.1'
server = '0.0.0.0'  # use internal default
port = 4096

''' Game Preferences '''
auto_assign_team = True

#for Youtube recording
#1920x1080, 1600x900, 1366x768, 1280x720, 1024x600 are all 16:9

''' Window / Camera '''
width = 1280
height = 720
fullscreen = False
fov = 85.

''' Controls '''
sensitivity = 1000
camera_speed = 0.8
invert_mouse = False
agent_motion = 'normal'

''' HUD/Info panels '''
hud = True  # show hud
diagnostic_hud = False # show diagnostic HUD elements (graphs, ping, fps)

#inventory_hud_x_offset = 250
#inventory_hud_y_offset = 500

#cube_selector_x_offset = 888.0
#cube_selector_y_offset = 130.0

fps_perf_graph_x_offset = 50.0
fps_perf_graph_y_offset = 700.0

network_latency_graph_x_offset = 780.0
network_latency_graph_y_offset = 400.0

map_vbo_indicator_x_offset = 50.0
map_vbo_indicator_y_offset = 50.0

fps = True  # display FPS
ping = True # calculate and display ping
ping_update_interval = 500  # ms delay between ping updates

''' Sound '''
sound = True # global sound.
sfx = 100
music = 100
