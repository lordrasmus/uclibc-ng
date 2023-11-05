#!/usr/bin/python

import sys
import time
import re

from junit_xml import TestSuite, TestCase

test_cases = []

with open("log.txt","r") as f:
    for line in f.readlines():
        #print( l )
        
        if 'PASS ' in line:
            r = re.match("PASS (.*)", line)
            if r:
                test_name = r.group(1).split()[0]
                test = TestCase(test_name, '', time.time())
                test_cases.append(test)

        if 'FAIL ' in line:
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
