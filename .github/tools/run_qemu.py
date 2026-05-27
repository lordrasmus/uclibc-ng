#!/usr/bin/python

import os
import sys
import json
import threading
import subprocess
import select
import time
import signal

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

qemu_proc = None

def run_command(command):
    global qemu_proc
    command ="./qemu-inst/bin/" + command
    print("thread starting")
    sys.stdout.flush()
    # start_new_session: eigene Prozessgruppe, damit am Ende NUR dieses Job-Qemu
    # gekillt wird. Auf der shared self-hosted VM wuerde "killall -9 qemu-system-X"
    # sonst die gleichnamigen Qemus paralleler Runner mit-killen.
    qemu_proc = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, start_new_session=True)
    stdout, stderr = qemu_proc.communicate()
    print("qemu (stdout):", stdout.decode())
    print("qemu (stderr):", stderr.decode())
    print("Befehl beendet mit Exit-Code", qemu_proc.returncode)
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
            # Ist Qemu schon beendet, kommen keine Daten mehr -> nicht endlos weiterdrehen
            # (z.B. wenn Qemu gecrasht ist oder -- vor diesem Fix -- von einem anderen
            # Job per killall mit-gekillt wurde).
            if qemu_proc is not None and qemu_proc.poll() is not None:
                print("Qemu-Prozess ist beendet, breche Lese-Schleife ab.")
                break
            #break
    except Exception as e:
        print("Fehler beim Lesen aus der Named Pipe:", str(e))
        break
        
    if "-------------------- tests_end --------------------------" in test_log:
        break;
        
    
    if "Kernel panic - not syncing: Attempted to kill init" in test_log:
        break;

#os.close(pipe_out)
 
print("\nshutdown Qemu")
os.write( pipe_in, "reboot\n".encode())
#pipe_in.write("reboot")


command_thread.join(10)
if command_thread.is_alive():
    # Nur das eigene Job-Qemu killen (Prozessgruppe), NICHT global per Namen --
    # sonst stirbt auf der shared VM das gleichnamige Qemu paralleler Runner.
    if qemu_proc is not None and qemu_proc.poll() is None:
        print("kill qemu pid {0} (Prozessgruppe)".format(qemu_proc.pid))
        try:
            os.killpg(os.getpgid(qemu_proc.pid), signal.SIGKILL)
        except ProcessLookupError:
            pass

os.remove("guest_pipe.in")
os.remove("guest_pipe.out")
