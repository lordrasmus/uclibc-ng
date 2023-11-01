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
    
    if ( not "master" in data["on"]["push"]["branches"] ) and ( not "master2" in data["on"]["push"]["branches"] ): 
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
        tmp = data["name"]+ "_test_result.svg"
        line +="|[![" + data["name"] + "test](https://gist.githubusercontent.com/lordrasmus/867aa95ade60fa5b1ad098fa6c6a1968/raw/" + tmp + ")]"
        tmp = data["name"]+ "_test_result.txt"
        line += "(https://gist.githubusercontent.com/lordrasmus/867aa95ade60fa5b1ad098fa6c6a1968/raw/" + tmp + ")"
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
