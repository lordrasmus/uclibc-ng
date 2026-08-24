#!/usr/bin/python

"""

   Badge fuer einen Lauf, der keine Testergebnisse hat.

   read_test_log.py schreibt den Badge aus dem Gast-Log -- das setzt voraus,
   dass es ein Gast-Log gibt. Scheitert eine Stufe davor, bleibt auf dem
   Server die letzte gruene SVG liegen und behauptet neben einer roten
   Build-Spalte weiter, die Tests seien in Ordnung. Also dasselbe Format,
   nur mit der Stufe als Text.

"""

import sys

from pybadges import badge

STAGES = {
    "build":  ("build failed",        "the uClibc-ng build did not finish"),
    "rootfs": ("rootfs build failed", "the testsuite/busybox/rootfs build did not finish"),
    "qemu":   ("test run failed",     "the run died before the log was analysed"),
}

if len(sys.argv) != 3 or sys.argv[1] not in STAGES:
    print("usage: failed_badge.py <" + "|".join(STAGES) + "> <run_url>")
    sys.exit(1)

stage, detail = STAGES[sys.argv[1]]
run_url = sys.argv[2]

with open("badge.svg", "w") as f:
    f.write(badge(left_text='test results', right_text=stage, right_color='#800000'))

with open("log_text.txt", "w") as f:
    f.write("ERROR: the test suite did not run\n")
    f.write("       " + detail + "\n")
    f.write("       no test results for this commit\n")
    f.write("\n")
    f.write("build log: " + run_url + "\n")

print(stage + " -- " + detail)
