#!/usr/bin/python

import sys
import time
import re

from pybadges import badge

from pprint import pprint


test_results = {"TOTAL": 0, "PASS": 0 , "FAIL":0 , "SKIP": 0 }
# Zeilen, die ein bestandener Test selbst ausgibt, weil er etwas nicht
# pruefen konnte -- Ausnahmen ohne fenv, Symbole in einem statischen
# Bau, Funktionen die die Config nicht baut. Nicht in TOTAL: das sind
# keine uebersprungenen Tests, sondern uebersprungene Pruefungen
# innerhalb bestandener Tests.
unchecked = []
tests_failed = []
tests_skip = []

# Lief die Suite ueberhaupt? Ohne diese Pruefung ist ein Lauf, der nie bis zu
# den Tests kommt (z.B. Kernel panic, weil das rootfs nicht ausfuehrbar ist),
# nicht von einem erfolgreichen zu unterscheiden: es gibt dann einfach keine
# PASS/FAIL-Zeilen und der Job wird gruen.
tests_started = False
tests_ended = False
kernel_panic = False
initramfs_failed = False
last_started = None   # letzter Test, dessen RUN-Marke im Log steht

header=True

file_text = open("log_text.txt","w")

with open("log.txt","rb") as f:
    for line_bin in f.readlines():
        #print( line_bin )
        try:
            line = line_bin.decode()
        except:
            continue
            
            
        if "Kernel panic" in line:
            kernel_panic = True

        if "Initramfs unpacking failed" in line:
            initramfs_failed = True

        if "-------------------- tests_start ------------------------" in line:
            tests_started = True
            header=False
            file_text.write( line )
            continue

        if '-------------------- tests_end --------------------------' in line:
            tests_ended = True
            break
        
        if header == True:
            file_text.write( line )
            continue
            
        
        file_text.write( line )
        
        line = line.replace("\r\n","")
            
        print( line )
        
        if line.startswith('RUN '):
            last_started = line.split('RUN ', 1)[1].strip()
            continue

        if 'PASS ' in line:
            test_results["TOTAL"] += 1
            test_results["PASS"] += 1
                    

        if 'FAIL ' in line:
            test_results["TOTAL"] += 1
            test_results["FAIL"] += 1
            sp = line.split("FAIL ")
            sp2 = sp[1].split(" ", 1)
            tests_failed.append( [ sp2[0], sp2[1] ] );
            

        if 'SKIP ' in line:
            test_results["TOTAL"] += 1
            test_results["SKIP"] += 1
            tests_skip.append( line.split("SKIP ")[1] );

        # von den Runnern nach einem PASS durchgereicht, eingerueckt
        st = line.strip()
        if st.startswith("SKIP:") or "expectations skipped" in st:
            unchecked.append( [ last_started or "", st ] )



"""

   create badge.svg

"""

file_text.close();

#pprint( test_results )
#pprint( tests_failed )
#pprint( tests_skip )

#test_results["FAIL"] = 0

"""

   Ein Lauf ist gut, wenn die Suite durchgelaufen ist UND kein Test
   fehlgeschlagen ist.  Diese Entscheidung faellt hier, vor dem Badge, damit
   Badge, Summary und Exit-Code dieselbe Antwort geben: ein Lauf, der die
   Tests nie erreicht hat, lieferte frueher 0 Fehler von 0 Tests und wurde
   damit gruen, waehrend der Job rot war.

"""

if kernel_panic:
    reason = 'kernel panic'
elif initramfs_failed:
    # rootfs passt nicht in den Gastspeicher: der Init kommt noch hoch, die
    # spaeteren Dateien (u.a. der Testrunner) fehlen -- -m im qemu_cmd erhoehen
    reason = 'initramfs unpack failed'
elif not tests_started:
    reason = 'guest never reached the tests'
elif not tests_ended:
    reason = 'run cut short'
elif test_results["TOTAL"] == 0:
    reason = 'no test results'
else:
    reason = None

suite_ran = reason is None

if not suite_ran:
    right_color='#800000'
    if test_results["TOTAL"] > 0:
        text = '{0} after {1} tests'.format( reason, test_results["TOTAL"] )
    else:
        text = reason
else:
    right_color='green'
    if test_results["FAIL"] > 0:
        right_color='#800000'
    text = 'total: {0} pass: {1}  failed: {2}  skip: {3}'.format( test_results["TOTAL"], test_results["PASS"], test_results["FAIL"], test_results["SKIP"] )

s = badge(left_text='test results', right_text=text, right_color=right_color )

with open("badge.svg","w") as f:
    f.write( s )



"""

   create Test Summary

"""

summary_text = "## Test Summary :bulb:\n\n"
summary_text += "|  |  |\n"
summary_text += "| ---- | ---- |\n"
summary_text += "|Total: {0} |:hash:|\n".format( test_results["TOTAL"] )
summary_text += "|Pass: {0}  |:white_check_mark:|\n".format( test_results["PASS"] )
summary_text += "|Fail: {0}  |:x:|\n".format( test_results["FAIL"] )
summary_text += "|skip: {0}  |:warning:|\n".format( test_results["SKIP"] )
if unchecked:
    summary_text += "|unchecked: {0}  |:grey_question:|\n".format( len(unchecked) )


if unchecked:
    summary_text += "\n"
    summary_text += "#### Not checked :grey_question:\n\n"
    summary_text += "| Test | Reason |\n"
    summary_text += "| ---- | ---- |\n"
    for u in unchecked:
        summary_text += "| {0} | {1} |\n".format( u[0], u[1] )

summary_text += "\n"
summary_text += "#### Test Failed :x:\n\n"
summary_text += "| Test  | Message  |\n"
summary_text += "| ---- | ---- |\n"


for fa in tests_failed:
    summary_text += "|" + fa[0] + "|" + fa[1] + "|\n"


summary_text += "\n"
summary_text += "#### Test Skiped :warning:\n\n"
summary_text += "| Test  |\n"
summary_text += "| ----  |\n"

for fa in tests_skip:
    summary_text += "|" + fa + "|\n"

with open("test_summary.md","w") as f:
    f.write( summary_text )


"""

   did the suite run at all?

   Rot wird ein Lauf in zwei Faellen: die Suite ist nicht gelaufen (Panic,
   fehlender tests_end-Marker, keine Ergebnisse), oder mindestens ein Test
   ist fehlgeschlagen. Badge, log_text.txt und test_summary.md sind zu dem
   Zeitpunkt schon geschrieben, und der Upload-Schritt laeuft mit always()
   -- die Anzeige bleibt also aktuell, auch wenn der Job rot ist.

"""

if not suite_ran:
    print("")
    print("ERROR: the test suite did not run")
    if kernel_panic:
        print("       the kernel panicked")
    if initramfs_failed:
        print("       initramfs unpacking failed - the rootfs does not fit into")
        print("       the guest memory, raise -m in the workflow's qemu_cmd")
    if not tests_started:
        print("       tests_start marker missing - userspace never got that far")
    elif not tests_ended:
        print("       tests_end marker missing - the run was cut short")
    if test_results["TOTAL"] == 0:
        print("       no test results in log.txt")
    if last_started:
        print("       last test started: {0} ({1} results before it)".format(
              last_started, test_results["TOTAL"] ))
    sys.exit(1)

if test_results["FAIL"] > 0:
    print("")
    print("ERROR: {0} of {1} tests failed".format( test_results["FAIL"], test_results["TOTAL"] ))
    for fa in tests_failed:
        print("       " + fa[0] + " : " + fa[1])
    sys.exit(1)
