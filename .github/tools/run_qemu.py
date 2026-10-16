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

# Ein FIFO blockiert beim Oeffnen zum Schreiben, bis die Gegenseite ihn liest,
# und die Gegenseite ist qemu. Stirbt qemu beim Start -- etwa weil die Maschine
# in der gerade gebundelten qemu-Version nicht mehr bootet --, oeffnet es den
# Pipe nie und dieser Aufruf wartet fuer immer. Die Wache unten in der
# Leseschleife hilft dann nicht, weil die Schleife gar nicht erst anlaeuft: der
# Job steht bis zum Job-Timeout und belegt die ganze Zeit einen Runner.
# Deshalb nicht-blockierend oeffnen (ENXIO, solange kein Leser da ist) und
# zwischen den Versuchen nachsehen, ob qemu ueberhaupt noch lebt.
pipe_in = None
for _ in range( 60 ):
    try:
        pipe_in = os.open("guest_pipe.in", os.O_WRONLY | os.O_NONBLOCK )
        break
    except OSError:
        pass
    if qemu_proc is not None and qemu_proc.poll() is not None:
        break
    time.sleep( 1 )

if pipe_in is None:
    rc = qemu_proc.returncode if qemu_proc is not None else "?"
    msg = "Qemu hat guest_pipe.in nie geoeffnet (Exit-Code {0}) -- Abbruch.".format( rc )
    print( msg )
    log_file.write( ( msg + "\n" ).encode() )
    log_file.flush()
    log_file.close()
    command_thread.join(10)
    os.remove("guest_pipe.in")
    os.remove("guest_pipe.out")
    sys.exit(1)

test_log = ""

# mit 30  gabs timeouts beim powerpc
# mit 120 gabs timeouts beim riscv64
read_pipe_timeout = 240
max_consecutive_timeouts = 3   # 3 x 240s = 12 min komplett stumm -> als haengend werten
consecutive_timeouts = 0

# Die Stille-Erkennung oben faengt nur den stummen Gast. Laeuft die Suite
# dagegen weiter und kommt nur nie bei tests_end an -- ein Test, der haengt
# und dabei Ausgabe macht --, setzt jede Zeile consecutive_timeouts zurueck
# und die Schleife dreht endlos. Der Job laeuft dann in GitHubs 6-Stunden-
# Grenze, ohne Badge und ohne Gast-Log, und belegt so lange einen der 60
# Runner. Langsamstes Ziel gemessen: 16 min, hier also reichlich Reserve.
# Ausnahme arcv2: der ARC-Fork von qemu laesst die Gastuhr ~11x langsamer
# laufen, mit nptl braucht die Suite dort ~70 min. Ein Ziel kann die Grenze
# deshalb ueber qemu_max_minutes im Workflow setzen.
max_total_seconds = int( data.get( "CONFIG_QEMU_MAX_MINUTES" ) or 35 ) * 60
deadline = time.monotonic() + max_total_seconds

while True:
    try:
        rlist, _, _ = select.select([pipe_out], [], [], read_pipe_timeout ) 
        #print( rlist )
        if rlist:
            # Wenn Daten verfügbar sind, lesen Sie sie aus der Pipe
            pipe_data = os.read(pipe_out, 4096)  # Sie können die Puffergröße anpassen
            
            
            log_file.write( pipe_data )
            log_file.flush()
            
            # errors="replace": nie einen Chunk verwerfen -- sonst koennte die
            # tests_end-Marke (oder Ausgabe ueberhaupt) durch nicht-UTF8-Bytes
            # verloren gehen, sodass die Schleife trotz fertigem Test endlos
            # weiterlaeuft und der GitHub-Log leer bleibt.
            out_txt = pipe_data.decode(errors="replace")
            consecutive_timeouts = 0

            test_log += out_txt
            sys.stdout.write( out_txt )
            sys.stdout.flush()
        else:
            consecutive_timeouts += 1
            print("Timeout: Keine Daten verfügbar. timeout : {0} (#{1})".format( read_pipe_timeout, consecutive_timeouts ) )
            # Ist Qemu schon beendet, kommen keine Daten mehr -> nicht endlos weiterdrehen.
            if qemu_proc is not None and qemu_proc.poll() is not None:
                print("Qemu-Prozess ist beendet, breche Lese-Schleife ab.")
                break
            # Sicherheitsnetz: Qemu lebt zwar, ist aber dauerhaft stumm -> als haengend werten.
            if consecutive_timeouts >= max_consecutive_timeouts:
                print("Zu viele Timeouts in Folge, werte Qemu als haengend und breche ab.")
                break
    except Exception as e:
        print("Fehler beim Lesen aus der Named Pipe:", str(e))
        break
        
    if "-------------------- tests_end --------------------------" in test_log:
        break;

    if time.monotonic() > deadline:
        msg = "Gesamtlaufzeit ueber {0} min, werte den Lauf als haengend und breche ab.".format( max_total_seconds // 60 )
        print( msg )
        log_file.write( ( "\n" + msg + "\n" ).encode() )
        log_file.flush()
        break

    # Nach einer Panic kommt nichts mehr -- sonst wartet die Schleife die
    # kompletten 3 x 240s Stille ab, bevor sie aufgibt. Der Fehler selbst wird
    # in read_test_log.py gemeldet (fehlende tests_end-Marke).
    if "Kernel panic" in test_log:
        print("Kernel panic erkannt, breche Lese-Schleife ab.")
        break
        
    
    if "Kernel panic - not syncing: Attempted to kill init" in test_log:
        break;

#os.close(pipe_out)
 
print("\nshutdown Qemu")
# Die Schleife bricht auch ab, weil qemu schon weg ist (Panic, Absturz). Dann
# hat der Pipe keinen Leser mehr und der Schreibversuch wirft EPIPE -- bisher
# endete der Lauf an der Stelle mit einem Python-Traceback statt mit der
# Auswertung, die der naechste Step noch machen soll.
try:
    os.write( pipe_in, "reboot\n".encode())
except OSError as e:
    print("Qemu nimmt kein 'reboot' mehr entgegen:", str(e))


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
