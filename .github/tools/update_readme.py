#!/usr/bin/python

import os
import glob
import yaml

matching_files = glob.glob(".github/workflows/*.yml")

with open('README_HEAD.md', 'r') as file:
    readme = file.read()

lines = []

for f in matching_files:
    
    f_base = os.path.basename(f)
    if f_base.startswith("z_"): continue
    
    with open(f, 'r') as f2:
        data = yaml.safe_load(f2)
    
    if not "master" in data[True]["push"]["branches"]: continue
        
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
    else:
        line +="|:x:"
        
    line +="\n"
    
    lines.append( [ data["name"], line ] )
    

for l in sorted(lines, key=lambda x: x[0]) :
    readme += l[1]
    
    
with open('README.md', 'w') as file:
    file.write(readme)
