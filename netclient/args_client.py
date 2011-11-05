#!/usr/bin/python

'''
Usage:

To integrate in existing code:

from this_module import get_args
args = get_args()

'''

'''
Available options:

version         Prints client version to STDOUT
quiet           Suppress STDOUT (not implemented)

name            Player name
alt-name        Player name when 'name' in use

server          Server IP Address
tcp-port        Server TCP Port
udp-port        Server UDP Port

auto-assign     Automatically assign to team

fullscreen      Run game in fullscreen
width           Window width
height          Window height

sensitivity     Global mouse sensitivity
mouse-sensitivity   Mouse sensitivity for player agent (Overrides sensitivity)
camera-sensitivity  Mouse sensitivity for camera (Override sensitivity)
camera-speed    Camera speed

no-hud          Don't display HUD elements on start (Enable in game with /)
display-fps     Show FPS (frames per second) in HUD
display-ping    Show round trip ping time in milliseconds in HUD
ping-update-interval    How often the server is pinged

no-sound        Disable all sound
sfx             Sound effects volume (Values 0-100)
music           Music volume (Values 0-100)

no-player       Free-camera only. Will not create player or join game
disable-draw-agents Don't draw agent character models

print-args      Print all settings to STDOUT
no-load         Don't start the client. Abort after argument processing.

'''

import sys
import argparse
import default_settings as settings

DC_CLIENT_VERSION = '3.7'

# Booleans cannot be processed in this way by the argparser.
# They will be processed after argparse.parse()
DEFAULTS = {

    # User
    'name'      :   settings.name,
    'alt_name'  :   settings.alt_name,

    # Common CLI Options
    'version'   : '%%(prog)s %s' % (DC_CLIENT_VERSION,),

    # Network
    'server'    :   settings.server,
    'tcp'       :   settings.tcp_port,
    'udp'       :   settings.udp_port,

    # Window
    'width'     :   settings.width,
    'height'    :   settings.height,
    
    # Controls
    'sensitivity'        : settings.sensitivity,
    'mouse_sensitivity'  : settings.mouse_sensitivity,
    'camera_sensitivity' : settings.camera_sensitivity,
    'camera_speed'  :   settings.camera_speed,

    # HUD / Info Panels
    'ping_update_interval' : settings.ping_update_interval,

    # Sound
    'sfx'       :   settings.sfx,
    'music'     :   settings.music,
}

def load_defaults():
    global DEFAULTS

    if hasattr(settings, 'sensitivity'):
        DEFAULTS['sensitivity'] = settings.sensitivity

    for prop in ['mouse_sensitivity', 'camera_sensitivity']:
        if hasattr(settings, prop):
            DEFAULTS[prop] = getattr(settings, prop)
        else:
            DEFAULTS[prop] = DEFAULTS['sensitivity']
        
load_defaults()

def parse(cl_args=None):
    parser = argparse.ArgumentParser(description="Client", prog="Gnomescroll")

    ''' CLI Options '''
    parser.add_argument('-V', '--version', action='version', version=DEFAULTS['version'])
    parser.add_argument('-q', '--quiet', action='store_true')   # Not implemented!

    ''' User '''
    parser.add_argument('-n', '--name', default=DEFAULTS['name'])
    parser.add_argument('-an', '--alt-name', default=DEFAULTS['alt_name'], dest='alt_name')

    ''' Network '''
    parser.add_argument('-s', '--server', default=DEFAULTS['server'])
    parser.add_argument('-tcp', '--tcp-port', default=DEFAULTS['tcp'], type=int)
    parser.add_argument('-udp', '--udp-port', default=DEFAULTS['udp'], type=int)

    ''' Game Preferences '''
    parser.add_argument('-aa', '--auto-assign', action='store_true', dest='auto_assign_team')

    ''' Window '''
    parser.add_argument('-fs', '--fullscreen', action='store_true')
    parser.add_argument('-x', '--width', default=DEFAULTS['width'], type=int)
    parser.add_argument('-y', '--height', default=DEFAULTS['height'], type=int)

    ''' Controls '''
    parser.add_argument('-sen', '--sensitivity', default=argparse.SUPPRESS, type=int)
    parser.add_argument('-csen', '--camera-sensitivity', default=argparse.SUPPRESS, type=int)
    parser.add_argument('-msen', '--mouse-sensitivity', default=argparse.SUPPRESS, type=int)
    parser.add_argument('-cs', '--camera-speed', default=DEFAULTS['camera_speed'], type=float)

    ''' HUD/Info panels '''
    parser.add_argument('-nh', '--no-hud', action='store_true', dest='hud')
    parser.add_argument('-fps', '--display-fps', action='store_true', dest='fps')  # display frames per second in hudp
    parser.add_argument('-ping', '--display-ping', action='store_true', dest='ping')
    parser.add_argument('-pud', '--ping-update-interval', default=DEFAULTS['ping_update_interval'], type=int)

    ''' Sound '''
    parser.add_argument('-ns', '--no-sound', action='store_true', dest='sound')
    parser.add_argument('--sfx', default=DEFAULTS['sfx'], type=int)
    parser.add_argument('--music', default=DEFAULTS['music'], type=int)

    ''' Rendering '''
    parser.add_argument('-np', '--no-player', action='store_true') # no player, just camera viewer
    parser.add_argument('-dad', '--disable-draw-agents', action='store_true', dest='draw_agents')   # Don't draw agents

    ''' Physics '''
    #parser.add_argument('-g', '--gravity', default=DEFAULTS['gravity'], type=float)

    ''' Debug '''
    parser.add_argument('-pa', '--print-args', action='store_true') # Print args & settings
    parser.add_argument('-nl', '--no-load', action='store_true')    # Don't start the game. Process the args and abort.

    ''' Alternate Client Modes '''
    #parser.add_argument('-c', '--cli', action='store_true') # command line mode (meaningless / not implemented)
    #parser.add_argument('-a', '--admin', action='store_true') # admin mode      (meaningless / not implemented)

    if cl_args is not None:
        args = parser.parse_args(cl_args)
    else:
        args = parser.parse_args()
        
    setattr(args, 'version', DC_CLIENT_VERSION)
    
    return args

def merge_with_settings(args):
    # collect settings keys (ignore builtins like __doc__)
    s_props = [p for p in dir(settings) if not (p.startswith('__') and p.endswith('__'))]

    # only copy values that are not defined in the args object
    # the args object already collects defaults from settings
    # in the process step, for shared arg names
    for p in s_props:
        if not hasattr(args, p):
            setattr(args, p, getattr(settings, p))

def postprocess_args(args):
    # Check booleans and override
    ''' Game Preferences '''
    if not args.auto_assign_team:
        args.auto_assign_team = settings.auto_assign_team

    ''' Window '''
    if not args.fullscreen:
        args.fullscreen = settings.fullscreen

    ''' Controls '''
    # if -sen --sensitivity is provided on the command line, override all sensitivity,
    # except other cli sensitivities
    # e.g. ./run -sen 500           --- mouse and camera sen 500
    # e.g. ./run -sen 500 -csen 200 --- mouse 500, camera 200
    # e.g. ./run -msen 200          --- mouse 200, camera is either settings.camera_sensitivity, settings.sensitivity, or DEFAULTS['sensitivity'] (in decreasing priority order)
    sen_cli_defined = hasattr(args, 'sensitivity')
    for prop in ['mouse_sensitivity', 'camera_sensitivity']:
        if not hasattr(args, prop):
            if sen_cli_defined:
                sen_attr = args.sensitivity
            else:
                sen_attr = DEFAULTS[prop]
            setattr(args, prop, sen_attr)
    if not sen_cli_defined:
        args.sensitivity = DEFAULTS['sensitivity']

    ''' HUD/Info panels '''
    if not args.fps and settings.fps:
        args.fps = settings.fps
    if not args.ping and settings.ping:
        args.ping = settings.ping
    if not args.hud: # --no-hud is the option
        args.hud = settings.hud
    else:
        args.hud = not args.hud

    ''' Sound '''
    if not args.sound:  # --no-sound is the option
        args.sound = settings.sound
    else:
        args.sound = not args.sound
        
    args.sfx = max(min(args.sfx, 100), 0)
    args.sfx /= 100.
    args.music = max(min(args.music, 100), 0)
    args.music /= 100.
    if args.sfx <= 0 and args.music <= 0:
        args.sound = False

    ''' Camera, Rendering '''
    args.draw_agents = not args.draw_agents # cli argument is "--disable-agents-draw", so flip it


def get_args():
    try:
        args = parse()
    except:
        # this allows us to do: python gameloop.py 222.33.44.55  or 222.333.44.55:6666 (i.e. specifying only the ip address)
        server = sys.argv[1]
        try:
            if not len(server.split('.')) == 4:
                raise Exception
        except:
            sys.exit()

        if ':' in server:
            server, port = server.split(':')
            cl_args = '--server %s --port %s' % (server, port,)
        else:
            cl_args = '--server %s' % (server,)

        args = parse(cl_args.split())

    postprocess_args(args)
    merge_with_settings(args)
    
    if args.print_args:
        print_args(args)

    if args.no_load:
        sys.exit()
    
    return args

def print_args(args):
    keypairs = []
    print 'Options:'
    for key in dir(args):
        if key.startswith('__') and key.endswith('__'): continue # __special__
        val = getattr(args, key)
        if type(val).__name__ in ['instancemethod', 'module']:
            continue
        keypairs.append((key, getattr(args, key)))
    keypairs.sort()
    for keypair in keypairs:
        print '%s :: %s' % keypair

if __name__ == '__main__':
    args = get_args()
    print_args(args)
