import os
import time

pid=os.fork()
if pid:
    # parent
    while True:
        print("I'm the parent")
        time.sleep(0.5)    
else:
    # child
    while True:
        print("I'm just a child")
        time.sleep(0.5)