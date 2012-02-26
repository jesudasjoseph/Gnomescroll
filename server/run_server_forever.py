#!/usr/bin/python

import subprocess
import os
import platform
import sys
import time

if platform.system() != "Linux":
    print "This script is only compatible with linux"
    sys.exit()

subprocess.Popen('ulimit -c unlimited', shell=True)
    
print "Starting eternal server"
MAX_DUMPS = 100
ct = 0
while True:
    print '%d-th server run' % ct
    logname = 'log%d' % (time.time(),)
    subprocess.call(['./m643_run_log.sh %s' % (logname)], shell=True)
    print "Server died"
    ct += 1
    if ct >= MAX_DUMPS:
        print "Reached max core dumps."
        break
    if os.path.exists('./core'):
        print "Core dumped; saving"
        os.rename("./core", "%s/coredumps/core%d" % (os.path.expanduser('~'), time.time(),))
    if os.path.exist('./%s', % (logname,)):
        print "Saving %s" % (logname,)
        os.rename('./%s' % (logname,), '%s/coredumps/%s', (os.path.expanduser('~'), logname,))

subprocess.Popen('ulimit -c 0', shell=True)

print "Exiting"
