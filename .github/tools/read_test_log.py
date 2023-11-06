#!/usr/bin/python

import sys
import time
import re

from pybadges import badge

from junit_xml import TestSuite, TestCase

test_cases = []

test_results = {"TOTAL": 0, "PASS": 0 , "FAIL":0 , "SKIP": 0 }

with open("log.txt","r") as f:
    for line in f.readlines():
        #print( l )
        
        if 'PASS ' in line:
            test_results["TOTAL"] += 1
            test_results["PASS"] += 1
            r = re.match("PASS (.*)", line)
            if r:
                test_name = r.group(1).split()[0]
                test = TestCase(test_name, '', time.time())
                test_cases.append(test)

        if 'FAIL ' in line:
            test_results["TOTAL"] += 1
            test_results["FAIL"] += 1
            r = re.match("FAIL (.*)", line)
            if r:
                failure_line = r.group(1).split(' ', 1)
                test_name = failure_line[0]
                if len(failure_line) > 1:
                    failure_msg = failure_line[1]
                test = TestCase(test_name, '', time.time())
                test.add_failure_info(message="FAIL {}".format(failure_msg))
                test_cases.append(test)

        if 'SKIP' in line:
            test_results["TOTAL"] += 1
            test_results["SKIP"] += 1
            r = re.match("SKIP (.*)", line)
            if r:
                test_name = r.group(1).split()[0]
                test = TestCase(test_name, '', time.time())
                test.add_skipped_info(message="SKIP")
                test_cases.append(test)

        if 'Total passed:' in line:
            print("uClibc-ng testsuite run is over, writing test results and exiting.")
            break


ts = TestSuite("uClibc-ng testsuite", test_cases)
with open("test_result.xml", "w+") as to:
    to.write(TestSuite.to_xml_string([ts]))


"""

   create badge.svg

"""


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

summary_text = "### Test Summary :bulb:\n"
summary_text += "|--- |----|\n"
summary_text += "|Total: {0} |:white_check_mark:|\n".format( test_results["TOTAL"] )
summary_text += "|Pass: {0} |:white_check_mark:|\n".format( test_results["PASS"] )
summary_text += "|Fail: {0} |:x:|\n".format( test_results["FAIL"] )
summary_text += "|skip: {0} |:zzz:|\n".format( test_results["SKIP"] )


with open("test_summary.md","w") as f:
    f.write( summary_text )
