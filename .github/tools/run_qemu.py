#!/usr/bin/python

import os
import sys
import json
import threading
import subprocess
import select
import time

from pprint import pprint

print("\nQemu Runner starting\n")

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

log_file = open("log.txt","wb")

#os.system("ls")

#pipe_in = open("guest_pipe.in", "w")
pipe_out = os.open("guest_pipe.out", os.O_RDONLY | os.O_NONBLOCK)

if not os.path.exists( "qemu-inst.tar.xz" ):
    os.system('wget -nv  "https://github.com/lordrasmus/uclibc-ng-qemu-imgs/blob/main/qemu-inst.tar.xz?raw=true" -O qemu-inst.tar.xz')
    os.system("tar -xaf qemu-inst.tar.xz")


ret = subprocess.getstatusoutput("./qemu-inst/bin/" + cmd.split(" ")[0] + " --version")

print( "Qemu Command : " + cmd )
print( "Qemu Version : " + ret[1] )
sys.stdout.flush()

log_file.write( ( "Qemu Command : " + cmd + "\n").encode() )
log_file.write( ( "Qemu Version : " + ret[1] + "\n").encode())
log_file.flush()

def run_command(command):
    command ="./qemu-inst/bin/" + command
    print("thread starting")
    sys.stdout.flush()
    process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()
    print("qemu (stdout):", stdout.decode())
    print("qemu (stderr):", stderr.decode())
    print("Befehl beendet mit Exit-Code", process.returncode)
    sys.stdout.flush()


command_thread = threading.Thread(target=run_command, args=(cmd,))
command_thread.start()

time.sleep(3) 

pipe_in =  os.open("guest_pipe.in",  os.O_WRONLY )

test_log = ""

# mit 30  gabs timeouts beim powerpc
# mit 120 gabs timeouts beim riscv64
read_pipe_timeout = 240

while True:
    try:
        rlist, _, _ = select.select([pipe_out], [], [], read_pipe_timeout ) 
        #print( rlist )
        if rlist:
            # Wenn Daten verfügbar sind, lesen Sie sie aus der Pipe
            pipe_data = os.read(pipe_out, 4096)  # Sie können die Puffergröße anpassen
            
            
            log_file.write( pipe_data )
            log_file.flush()
            
            try:
                out_txt = pipe_data.decode()
            except:
                out_txt = ""
            
            test_log += out_txt
            sys.stdout.write( out_txt )
            sys.stdout.flush()
        else:
            print("Timeout: Keine Daten verfügbar. timeout : {0}".format( read_pipe_timeout ) )
            #break
    except Exception as e:
        print("Fehler beim Lesen aus der Named Pipe:", str(e))
        break
        
    if "-------------------- tests_end --------------------------" in test_log:
        break;

#os.close(pipe_out)
 
print("\nshutdown Qemu")
os.write( pipe_in, "reboot\n".encode())
#pipe_in.write("reboot")


command_thread.join(10)
if command_thread.is_alive():
    kill_cmd = "killall -9 " + data["CONFIG_QEMU_CMD"].split(" ")[0]
    print("kill_cmd: " + kill_cmd );
    os.system( kill_cmd )

os.remove("guest_pipe.in")
os.remove("guest_pipe.out")
