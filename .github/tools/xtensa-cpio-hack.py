#!/usr/bin/python

import os
import sys

from pprint import pprint


old_string = b'HIER_DAS_ECHTE_CPIO_HIN'


print("Kernel cpio hack")

if len ( sys.argv ) > 1:
    #pprint ( sys.argv )
    
    if sys.argv[1] == "check_sig":
        
        print("  check_sig")
    
        with open('kernel.img', 'r+b') as file:
            # Binärdaten lesen
            data = file.read()
            
        index = data.find(old_string)
        print( "  Kernel Offset : " + str( index ) )
        
        if index == -1:
            print("    Error signature not found")
            exit(1)
        
        exit(0)


# Öffnen der Binärdatei im Schreibmodus
with open('kernel.img', 'r+b') as file:
    # Binärdaten lesen
    data = file.read()

    with open('rootfs.img', 'rb') as file2:
        # Binärdaten lesen
        new_string = file2.read()
        
    print("  RootFS Size   : " + str( len( new_string ) ))
    
    if len( new_string ) > 17825792:
        print("   Size > 17825792")
        exit(1)

    # Suchen nach dem zu ersetzenden String
    
    
    index = data.find(old_string)
    print( "  Kernel Offset : " + str( index ) )
    # Überprüfen, ob der zu ersetzende String gefunden wurde
    if index != -1:
        # Zur Position im File-Objekt springen
        file.seek(index)
        
        # Den neuen String schreiben
        file.write(new_string)
        print("rootfs.img written to kernel image")
        exit(0)
    
    print("HIER_DAS_ECHTE_CPIO_HIN not found in kernel image")
    exit(1)
