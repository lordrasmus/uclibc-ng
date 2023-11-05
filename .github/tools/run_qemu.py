#!/usr/bin/python

import os
import sys
import json
import threading
import subprocess
import select
import time


with open('infos.json', 'r') as json_file:
    data = json.load(json_file)



cmd = data["CONFIG_QEMU_CMD"]
cmd += " -serial pipe:guest_pipe"
#cmd += " -monitor 'telnet:127.0.0.1:55555,server,nowait'"
cmd += " -no-reboot"


if os.path.exists("log.txt"):
    os.remove("log.txt")

if os.path.exists("guest_pipe.in"):
    os.remove("guest_pipe.in")

if os.path.exists("guest_pipe.out"):
    os.remove("guest_pipe.out")

os.mkfifo("guest_pipe.in")
os.mkfifo("guest_pipe.out")

time.sleep(1) 

log_file = open("log.txt","w")

os.system("ls")

#pipe_in = open("guest_pipe.in", "w")
pipe_out = os.open("guest_pipe.out", os.O_RDONLY | os.O_NONBLOCK)


def run_command(command):
    print("starting : " + command)
    process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()
    #print("Ausgabe (stdout):", stdout.decode())
    #print("Fehler (stderr):", stderr.decode())
    #print("Befehl beendet mit Exit-Code", process.returncode)


command_thread = threading.Thread(target=run_command, args=(cmd,))
command_thread.start()

time.sleep(3) 

pipe_in =  os.open("guest_pipe.in",  os.O_WRONLY )

test_log = ""

while True:
    try:
        rlist, _, _ = select.select([pipe_out], [], [], 30)
        #print( rlist )
        if rlist:
            # Wenn Daten verfügbar sind, lesen Sie sie aus der Pipe
            data = os.read(pipe_out, 4096)  # Sie können die Puffergröße anpassen
            
            out_txt = data.decode()
            log_file.write( out_txt )
            log_file.flush()
            
            test_log += out_txt
            sys.stdout.write( out_txt )
            sys.stdout.flush()
        else:
            print("Timeout: Keine Daten verfügbar.")
            #break
    except Exception as e:
        print("Fehler beim Lesen aus der Named Pipe:", str(e))
        
    if "-------------------- tests_end --------------------------" in test_log:
        break;

#os.close(pipe_out)
 
print("shutdown")
os.write( pipe_in, "reboot\n".encode())
#pipe_in.write("reboot")


command_thread.join()

os.remove("guest_pipe.in")
os.remove("guest_pipe.out")
