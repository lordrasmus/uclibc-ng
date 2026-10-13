#!/usr/bin/python

import os
import sys

from pprint import pprint


old_string = b'HIER_DAS_ECHTE_CPIO_HIN'

# Groesse des Platzhalters im Kernel-Image. Mehr darf nicht geschrieben
# werden, sonst laeuft das rootfs ueber das initramfs hinaus. Per Argument
# ueberschreibbar, weil nicht jedes Image denselben Platzhalter hat.
max_size = 17825792

if len ( sys.argv ) > 1 and sys.argv[1] != "check_sig":
    max_size = int( sys.argv[1] )

# Welche Datei in den Platzhalter wandert. Voreinstellung ist das rohe cpio.
# leon3 setzt stattdessen rootfs.img.xz ein: unpack_to_rootfs() laesst
# decompress_method() auf den eingebauten Blob los und ueberspringt die Nullen
# dahinter einzeln, der Platzhalter bleibt damit klein (16 statt 80 MB).
rootfs_file = 'rootfs.img'

if len ( sys.argv ) > 2:
    rootfs_file = sys.argv[2]

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

    with open(rootfs_file, 'rb') as file2:
        # Binärdaten lesen
        new_string = file2.read()
        
    print("  RootFS Size   : " + str( len( new_string ) ))
    
    print("  Max Size      : " + str( max_size ))

    if len( new_string ) > max_size:
        print("   Size > " + str( max_size ))
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
        print(rootfs_file + " written to kernel image")
        exit(0)
    
    print("HIER_DAS_ECHTE_CPIO_HIN not found in kernel image")
    exit(1)
