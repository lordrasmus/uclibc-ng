#!/usr/bin/python

import os

print("Kernel cpio hack")

# Öffnen der Binärdatei im Schreibmodus
with open('kernel.img', 'r+b') as file:
    # Binärdaten lesen
    data = file.read()

    with open('rootfs.img', 'rb') as file2:
        # Binärdaten lesen
        new_string = file2.read()
        
    print("RootFS Size : " + str( len( new_string ) ))
    
    if len( new_string ) > 17825792:
        print("   Size > 17825792")
        exit(1)

    # Suchen nach dem zu ersetzenden String
    old_string = b'HIER_DAS_ECHTE_CPIO_HIN'
    
    index = data.find(old_string)
    print( index )
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
