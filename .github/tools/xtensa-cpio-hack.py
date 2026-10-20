#!/usr/bin/python

import gzip
import os
import sys

from pprint import pprint


old_string = b'HIER_DAS_ECHTE_CPIO_HIN'

# Groesse des Platzhalters im Kernel-Image. Mehr darf nicht geschrieben
# werden, sonst laeuft das rootfs ueber das initramfs hinaus. Ohne Argument
# wird sie wie in uc_devel (uc_dev/qemu.py) ausgemessen: Marker plus die
# Nullen dahinter. Eine feste Zahl stimmte nicht mehr, seit
# kernel-xtensa-nommu.img 48 statt 17 MB hat (uclibc-ng-qemu-imgs 664aaac),
# und xtensa-lt scheiterte an "Size > 17825792" mit einem 18,6-MB-Rootfs.
max_size = None

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

    if max_size is None:
        index = data.find(old_string)
        if index == -1:
            print("    Error signature not found")
            exit(1)
        end = index + len(old_string)
        while end < len(data) and data[end] == 0:
            end += 1
        max_size = end - index
    
    print("  Max Size      : " + str( max_size ))

    # Passt das rohe cpio nicht, ein gepacktes nehmen, wie uc_devel
    # (uc_dev/qemu.py): unpack_to_rootfs() erkennt die Kompression des
    # eingebauten Blobs und ueberspringt die Nullen dahinter. Nur ein Format,
    # das der Kernel laut kernel.config auspacken kann -- sonst stirbt er
    # ohne eine Logzeile. arcv2 brauchte das, als sein Rootfs 17,9 MB in
    # einem 17-MB-Platzhalter wurde (der Kernel hat CONFIG_RD_GZIP).
    if len( new_string ) > max_size and rootfs_file == 'rootfs.img':
        cfg = ""
        if os.path.exists( 'kernel.config' ):
            with open( 'kernel.config' ) as f:
                cfg = f.read()
        packed = []
        if "CONFIG_RD_GZIP=y" in cfg:
            packed.append( ( 'rootfs.img.gz', gzip.compress( new_string, 9, mtime=0 ) ) )
        if "CONFIG_RD_XZ=y" in cfg and os.path.exists( 'rootfs.img.xz' ):
            with open( 'rootfs.img.xz', 'rb' ) as f:
                packed.append( ( 'rootfs.img.xz', f.read() ) )
        for name, blob in packed:
            print("  " + name + " : " + str( len( blob ) ))
            if len( blob ) <= max_size:
                rootfs_file, new_string = name, blob
                break

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
