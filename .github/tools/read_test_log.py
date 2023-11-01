#!/usr/bin/python

import sys
import time
import re

from pybadges import badge

from pprint import pprint


test_results = {"TOTAL": 0, "PASS": 0 , "FAIL":0 , "SKIP": 0 }
tests_failed = []
tests_skip = []

header=True

file_text = open("log_text.txt","w")

with open("log.txt","rb") as f:
    for line_bin in f.readlines():
        #print( line_bin )
        try:
            line = line_bin.decode()
        except:
            continue
            
            
        if "-------------------- tests_start ------------------------" in line:
            header=False
            file_text.write( line )
            continue
            
        if '-------------------- tests_end --------------------------' in line:
            break
        
        if header == True:
            file_text.write( line )
            continue
            
        
        file_text.write( line )
        
        line = line.replace("\r\n","")
            
        print( line )
        
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



"""

   create badge.svg

"""

file_text.close();

#pprint( test_results )
#pprint( tests_failed )
#pprint( tests_skip )

#test_results["FAIL"] = 0

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
