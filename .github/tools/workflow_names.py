#!/usr/bin/python

import os
import re
import glob


from ruamel.yaml import YAML


def extrahiere_gcc_version(s):
    # Definiere das reguläre Ausdrucksmuster
    muster = r"gcc-(\d+)"
    
    # Suche nach dem Muster im String
    treffer = re.search(muster, s)
    
    # Überprüfe, ob ein Treffer vorhanden ist
    if treffer:
        # Extrahiere die gefundene Zahl
        return treffer.group(1)
    else:
        # Falls kein Treffer vorhanden ist, gib None zurück oder handle es entsprechend
        
        
        muster = r"gcc_(\w+)-(\d+)\.(\d+\.\d+)"
        treffer = re.search(muster, s)
        if treffer:
            # Extrahiere die gefundene Zahl
            return treffer.group(2)
        
        print( s )
        return None


def config_arch( conf ):
    with open( "./.github/configs/" + conf , "r" ) as f:
        lines = f.readlines()

    arch = ""
    
    is_multi_endian = False
    
    for l in lines:
        if l.startswith("ARCH_ANY_ENDIAN=y\n"):
            is_multi_endian = True
            break
    
    
    if is_multi_endian:
        for l in lines:
            if l.startswith("TARGET_") and not "SUBARCH" in l:
                arch = l.replace("TARGET_","").replace("=y\n","")
                
                if arch == "aarch64":     break
                if arch == "arc":         break
                if arch == "arm":         break
                if arch == "c6x":         break
                if arch == "csky":        break
                if arch == "microblaze":  break
                if arch == "mips":        break
                if arch == "nds32":       break
                if arch == "sh":          break
                if arch == "xtensa":      break
                
                
    else:
        for l in lines:
            if l.startswith("TARGET_") and not "SUBARCH" in l:
                arch = l.replace("TARGET_","").replace("=y\n","")
                
                if arch == "alpha":    return arch
                if arch == "avr32":    return arch
                if arch == "bfin":     return arch
                if arch == "cris":     break
                if arch == "h8300":    break
                if arch == "hppa":     return arch
                if arch == "kvx":      break
                if arch == "m68k":     return arch
                if arch == "nios2":    return arch
                if arch == "or1k":     return arch
                if arch == "powerpc":  break
                if arch == "riscv32":  return arch
                if arch == "riscv64":  return arch
                if arch == "sparc64":  return arch
                if arch == "sparc":    break
                if arch == "tile":     return arch
                if arch == "i386":     break
                if arch == "x86_64":   return arch
        
        
    
    if arch == "aarch64" or arch == "arc"  or arch == "csky" or arch == "microblaze" or arch == "nds32" or arch == "xtensa":
        for l in lines:
            if l == "ARCH_LITTLE_ENDIAN=y\n":
                return arch + "-le"
            
            if l == "ARCH_BIG_ENDIAN=y\n":
                return arch + "-be"


    if arch == "arm":
        
        for l in lines:
            if l == "CONFIG_ARM_EABI=y\n":
                arch += "-eabi"
        
        for l in lines:
            if l == "ARCH_LITTLE_ENDIAN=y\n":
                arch += "-le"
            
            if l == "ARCH_BIG_ENDIAN=y\n":
                arch += "-be"
        
        return arch

    
    if arch == "c6x":
        for l in lines:
            if l == "CONFIG_GENERIC_C6X=y\n":
                pass
            
            if l == "CONFIG_TMS320C64X=y\n":
                arch = "c64x"
            
            if l == "CONFIG_TMS320C64XPLUS=y\n":
                arch = "c64xplus"
     
     
        for l in lines:
                
            if l == "ARCH_LITTLE_ENDIAN=y\n":
                arch += "-le"
            
            if l == "ARCH_BIG_ENDIAN=y\n":
                arch += "-be"
    
        return arch
            
    
    if arch == "cris":
        for l in lines:
             if l == "CONFIG_CRIS=y\n":     return "cris"
             if l == "CONFIG_CRISV32=y\n":  return "cris32"
    
    if arch == "h8300":
        for l in lines:
            if l == "CONFIG_H8300H=y\n":     return "h8300h"
            if l == "CONFIG_H8S=y\n":        return "h8s"
    
    if arch == "kvx":
        for l in lines:
            if l == "CONFIG_KVX_COOLIDGE_V1=y\n":     return "kvx-v1"
            if l == "CONFIG_KVX_COOLIDGE_V2=y\n":     return "kvx-v2"
    
    if arch == "powerpc":
        for l in lines:
            if l == "CONFIG_CLASSIC=y\n":             return "powerpc"
            if l == "CONFIG_E500=y\n":                return "powerpc-e500"
    
    
    if arch == "mips":
        for l in lines:
            if l == "CONFIG_MIPS_N64_ABI=y\n":
                arch = "mips64-n64"
                break
                
            if l == "CONFIG_MIPS_N32_ABI=y\n":
                arch = "mips64-n32"
                break
                
            if l == "CONFIG_MIPS_O32_ABI=y\n":
                arch = "mips32"
                break
            

    if arch == "mips64-n64":
        for l in lines:
            if l == "ARCH_LITTLE_ENDIAN=y\n":
                return "mips64-le-n64"
            
            if l == "ARCH_BIG_ENDIAN=y\n":
                return "mips64-be-n64"                
    
    if arch == "mips64-n32":
        for l in lines:
            if l == "ARCH_LITTLE_ENDIAN=y\n":
                return "mips64-le-n32"
            
            if l == "ARCH_BIG_ENDIAN=y\n":
                return "mips64-be-n32"
    
    if arch == "mips32":
        for l in lines:
            if l == "ARCH_LITTLE_ENDIAN=y\n":
                return "mips32-le"
                
            if l == "ARCH_BIG_ENDIAN=y\n":
                return "mips32-be"
    
        
    
    if arch == "sh":
        for l in lines:
            if l == "CONFIG_SH2=y\n":
                arch = "sh2"
                break
        
        for l in lines:
            if l == "CONFIG_SH4=y\n":
                arch = "sh4"
                break
    
    if arch == "sh2":
        for l in lines:
            if l == "ARCH_LITTLE_ENDIAN=y\n":
                return "sh2-le"
            
            if l == "ARCH_BIG_ENDIAN=y\n":
                return "sh2-be"
    
    if arch == "sh4":
        for l in lines:
            if l == "ARCH_LITTLE_ENDIAN=y\n":
                return "sh4-le"
            
            if l == "ARCH_BIG_ENDIAN=y\n":
                return "sh4-be"
        
    if arch == "sparc":
        for l in lines:
            if l == "CONFIG_SPARC_V7=y\n":
                return "sparc-v7"
                
            if l == "CONFIG_SPARC_V8=y\n":
                return "sparc-v8"
    
    if arch == "i386":
        for l in lines:
            if l == "CONFIG_386=y\n": return "i386"
            if l == "CONFIG_486=y\n": return "i486"
            if l == "CONFIG_586=y\n": return "i586"
            if l == "CONFIG_686=y\n": return "i686"
        
    
        
    print( "arch : " + arch )
    print( conf )
    exit(1)
            

def config_mmu( conf ):
    with open( "./.github/configs/" + conf , "r" ) as f:
        lines = f.readlines()
        
    
    for l in lines:
        if l == "ARCH_USE_MMU=y\n":
            return ""
        
        if l == "# ARCH_USE_MMU is not set\n":
            return "-nommu"
            
        if l == "ARCH_HAS_NO_MMU=y\n":
            return "-nommu"

    return "-nommu"

    print("mmu")            
    print( conf )
    exit(1)        

        
def config_static( conf ):
    with open( "./.github/configs/" + conf , "r" ) as f:
        lines = f.readlines()
        
    
    for l in lines:
        if l == "ARCH_HAS_NO_SHARED=y\n":
            return "-static"
            
        if l == "# HAVE_SHARED is not set\n":
            return "-static"
        
        
        if l == "HAVE_SHARED=y\n":
            return "-shared"

    
    print("static")            
    print( conf )
    exit(1)        
        

def config_vdso( conf ):
    with open( "./.github/configs/" + conf , "r" ) as f:
        lines = f.readlines()
        
    
    for l in lines:
        
        if l == "VDSO_SUPPORT=y\n":
            return "-vdso"
    
    return ""

def config_flat( conf ):
    with open( "./.github/configs/" + conf , "r" ) as f:
        lines = f.readlines()
        
    
    for l in lines:
        
        if l == "UCLIBC_FORMAT_FLAT=y\n":           return "-flat"
        if l == "UCLIBC_FORMAT_FLAT_SEP_DATA=y\n":  return "-flat"
        if l == "UCLIBC_FORMAT_SHARED_FLAT=y\n":    return "-flat"
        if l == "UCLIBC_FORMAT_FDPIC_ELF=y\n":      return "-elf"
        if l == "# Using ELF file format\n":        return "-elf"
    
        
    
    print( conf )
    exit(1)


def config_thread( conf ):
    with open( "./.github/configs/" + conf , "r" ) as f:
        lines = f.readlines()
        
    
    for l in lines:
        if l == "HAS_NO_THREADS=y\n":            return "-no"
        if l == "UCLIBC_HAS_THREADS_NATIVE=y\n": return "-npt"
        if l == "UCLIBC_HAS_LINUXTHREADS=y\n":   return "-lt"
        
        
        
    print( conf )
    exit(1)

        
yaml=YAML(typ='safe')   # default, if not specfied, is 'rt' (round-trip)
yaml.default_flow_style = False


matching_files = glob.glob(".github/workflows/*.yml")

for f in sorted(matching_files):
    
    if os.path.basename(f).startswith("z_"): continue
    
    #print( f.ljust(50) ,  end='')
    
    with open(f, 'r') as f2:
        data = yaml.load(f2)
    
    infos = {}
    infos["kernel"] = data["jobs"]["call_build"]["with"]["kernel_vers"]
    infos["gcc"] = extrahiere_gcc_version( data["jobs"]["call_build"]["with"]["toolchain"] )
    infos["conf"] = data["jobs"]["call_build"]["with"]["config_file"]
    infos["arch"] = config_arch( infos["conf"] )    
    infos["mmu"] = config_mmu( infos["conf"] )    
    infos["static"] = config_static( infos["conf"] )    
    infos["vdso"] = config_vdso( infos["conf"] )    
    infos["flat"] = config_flat( infos["conf"] )    
    infos["thread"] = config_thread( infos["conf"] )    
    
    if not "pull_request" in data["on"]:
        print( f.ljust(80) ,  end='')
        print("\033[01;33m pull_request: fehlt in on:\033[00m")
        continue
    
    if "call_qemu" in data["jobs"] and not "if" in data["jobs"]["call_qemu"]:
        print( f.ljust(80) ,  end='')
        print("\033[01;33mif: github.event_name == 'push' fehlt im call_qemu:\033[00m")
        continue
        
    
    # print( infos )
    
    soll_name = ""
    soll_name += infos["arch"]
    soll_name += infos["thread"]
    soll_name += "-gcc-"+ infos["gcc"]
    soll_name += "-kernel-"+ infos["kernel"]
    soll_name += infos["static"]
    soll_name += infos["mmu"]
    soll_name += infos["vdso"]
    soll_name += infos["flat"]
    
    # thread s noch
    
    
    if not soll_name == data["name"]:
        print( f.ljust(80) ,  end='')
        print( "Workflow name soll : \033[01;31m" + soll_name + "\033[00m",  end='' )
        print("")
        continue
    
    
    soll_file = "make-" + data["name"]
    
    soll_file += ".yml"
    
    
    if not os.path.basename(f) == soll_file:
        print( f.ljust(80) ,  end='')
        print("Datei name soll    : \033[01;32m" + os.path.dirname(f) + "/" + soll_file + "\033[00m",  end='' )
        
        antwort = input("\nDatei umbennen ? (ja/nein): ")
        if antwort.lower() == "ja" or antwort.lower() == "j":
            cmd = "git mv " + f + " " + os.path.dirname(f) + "/" + soll_file 
            #print( cmd )
            os.system( cmd )
            cmd = 'git commit -m "github rename ' + f + ' to ' + os.path.dirname(f) + "/" + soll_file + ' "'
            #print( cmd )
            os.system( cmd )
    
