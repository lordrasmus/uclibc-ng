#!/usr/bin/python3

import os
import glob
from ruamel.yaml import YAML


yaml=YAML(typ='safe')   # default, if not specfied, is 'rt' (round-trip)
yaml.default_flow_style = False


matching_files = glob.glob(".github/workflows/*.yml")

with open('README_HEAD.md', 'r') as file:
    readme = file.read()

lines = []

for f in matching_files:
    
    yaml_needs_fix = False
    
    f_base = os.path.basename(f)
    if f_base.startswith("z_"): continue
    
    with open(f, 'r') as f2:
        data = yaml.load(f2)
    
    # Ein Ziel gehoert in die Tabelle, wenn sein push-Trigger den Standardbranch
    # trifft.  Frueher stand dort "master", inzwischen "**" -- die Abfrage auf
    # die beiden Namen allein hielt deshalb jedes einzelne Ziel fuer
    # abgeschaltet und das Skript loeschte die komplette Tabelle.
    branches = data["on"]["push"]["branches"]
    if not any(b in branches for b in ("master", "master2", "**", "*")):
        print("disabled : " + f )
        continue
        
    #print(f_base)
    #print(data.keys())
    #print(data["jobs"])
    #if "call_images" in data["jobs"]:
    #    print(data["jobs"]["call_images"])
    
    
    
    #|aarch64-4.19.56      |[![aarch64-4.19.56]()](https://github.com/lordrasmus/uclibc-ng/actions/workflows/make-aarch64-4.19.56.yml)
    
    line = "|" + data["name"]
    line += "|[![" + data["name"] + "]"
    line += "(https://img.shields.io/github/actions/workflow/status/lordrasmus/uclibc-ng/" + f_base + "?style=flat)]"
    line += "(https://github.com/lordrasmus/uclibc-ng/actions/workflows/" + f_base + ")"
    
    if "call_images" in data["jobs"]:
        line +="|:white_check_mark:"
        
        #print( data["jobs"]["call_images"] )
        if not "secrets" in data["jobs"]["call_images"]:
            print( "secrets missing in " + f)
            data["jobs"]["call_images"]["secrets"] = "inherit"
            yaml_needs_fix = True
        
        else:
            if not data["jobs"]["call_images"]["secrets"] == "inherit":
                print( "secrets wrong in " + f)
                data["jobs"]["call_images"]["secrets"] = "inherit"
                yaml_needs_fix = True
        
    else:
        line +="|:x:"
    
    if "call_qemu" in data["jobs"]:
        # ?v=N busts GitHub's Camo image cache. The test SVGs now ship a short
        # Cache-Control, but Camo had cached the old no-Cache-Control version
        # under its 1-year default; a fresh URL forces a re-fetch (and the new
        # one then honours the short TTL). Bump N if the badges ever get stuck.
        tmp = data["name"]+ "_test_result.svg?v=2"
        #line +="|[![" + data["name"] + "test](https://gist.githubusercontent.com/lordrasmus/867aa95ade60fa5b1ad098fa6c6a1968/raw/" + tmp + ")]"
        line +="|[![" + data["name"] + "test](https://uclibc-ng.tangotanzen.de/tests/" + tmp + ")]"
        tmp = data["name"]+ "_test_result.html"
        #line += "(https://gist.githubusercontent.com/lordrasmus/867aa95ade60fa5b1ad098fa6c6a1968/raw/" + tmp + ")"
        line += "(https://uclibc-ng.tangotanzen.de/tests/" + tmp + ")"
    else:
        line +="|:x:"
        
    line +="\n"
    
    
    lines.append( [ data["name"], line ] )

if yaml_needs_fix:
    exit( 1   )

for l in sorted(lines, key=lambda x: x[0]) :
    readme += l[1]
    
    
with open('README.md', 'w') as file:
    file.write(readme)
