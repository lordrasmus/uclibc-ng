#!/usr/bin/python

"""

   Badge fuer einen Lauf, der die Tests nie erreicht hat.

   read_test_log.py schreibt den Badge aus dem Gast-Log -- laeuft aber nur im
   qemu-Job, und der wird uebersprungen, sobald Build oder Rootfs scheitern.
   Ohne diesen Ersatz bleibt auf dem Server die letzte gruene SVG liegen und
   behauptet neben einer roten Build-Spalte weiter, die Tests seien in
   Ordnung. Also dasselbe Format, nur mit der Stufe als Text.

"""

import sys

from pybadges import badge

if len(sys.argv) != 4:
    print("usage: failed_badge.py <build_result> <images_result> <run_url>")
    sys.exit(1)

build_result, images_result, run_url = sys.argv[1:4]

if build_result != "success":
    stage = "build failed"
    detail = "the uClibc-ng build did not finish"
elif images_result != "success":
    stage = "rootfs build failed"
    detail = "the testsuite/busybox/rootfs build did not finish"
else:
    # Nur erreichbar, wenn der Job trotz gruener Vorstufen laeuft -- dann
    # lieber abbrechen als einen gruenen Badge mit rotem Text ueberschreiben.
    print("ERROR: neither build nor images failed, refusing to write a badge")
    sys.exit(1)

with open("badge.svg", "w") as f:
    f.write(badge(left_text='test results', right_text=stage, right_color='#800000'))

with open("log_text.txt", "w") as f:
    f.write("ERROR: the test suite did not run\n")
    f.write("       " + detail + "\n")
    f.write("       no test results for this commit\n")
    f.write("\n")
    f.write("build log: " + run_url + "\n")

print(stage + " -- " + detail)
