#!/usr/bin/python


import glob

confs = glob.glob(".github/configs/config.*")


checks= ["UCLIBC_HAS_ARGP"]

for c in confs:
    with open(c,"r") as f:
        data = f.read()


    print( c )

    for ch in checks:
        if not ch+"=y" in data:
            print( "   \033[01;32m" + ch + "\033[00m aktivieren" )

