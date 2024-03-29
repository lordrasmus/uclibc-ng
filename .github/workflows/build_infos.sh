#!/bin/bash

#echo $1
mmu=$(grep ARCH_USE_MMU $1)
if [[ $mmu == "ARCH_USE_MMU=y"* ]]; then
        mmu="Yes"
else
        mmu="No"
fi

bits=$(grep TARGET_ARCH_BITS $1)
bits="${bits/TARGET_ARCH_BITS=}"


tmp=$(grep ARCH_LITTLE_ENDIAN $1)
if [[ $tmp == "ARCH_LITTLE_ENDIAN=y"* ]]; then
        endian="Little"
else
        endian="Big"
fi

arch=$(grep TARGET_ARCH= $1)
arch="${arch/TARGET_ARCH=}"
arch="${arch/\"}"
arch="${arch/\"}"


tmp=$(grep UCLIBC_HAS_FPU $1)
if [[ $tmp == "UCLIBC_HAS_FPU=y"* ]]; then
        float="Hard"
else
        float="Soft"
fi

threads="No"
tmp=$(grep UCLIBC_HAS_LINUXTHREADS $1)
if [[ $tmp == "UCLIBC_HAS_LINUXTHREADS=y"* ]]; then
        threads="Linux"
fi
tmp=$(grep UCLIBC_HAS_THREADS_NATIVE $1)
if [[ $tmp == "UCLIBC_HAS_THREADS_NATIVE=y"* ]]; then
        threads="NPTL"
fi


tmp=$(grep ^VDSO_SUPPORT $1)
if [[ $tmp == "VDSO_SUPPORT=y"* ]]; then
        vdso="Yes"
else
        vdso="No"
fi

tmp=$(grep HAVE_SHARED $1)
if [[ $tmp == "HAVE_SHARED=y"* ]]; then
        shared="Shared"
else
        shared="Static"
fi


subarch=""
if [[ $arch == "sh" ]] ; then
        subarch="unkown"
        
        if [[ $(grep CONFIG_SH2= $1) == "CONFIG_SH2=y"* ]]; then  subarch="sh2";  fi
        if [[ $(grep CONFIG_SH4= $1) == "CONFIG_SH4=y"* ]]; then  subarch="sh4";  fi
fi

if [[ $arch == "h8300" ]] ; then
        
        if [[ $(grep CONFIG_H8300H= $1) == "CONFIG_H8300H=y"* ]]; then  subarch="h8000h";  fi

fi


if [[ $subarch == "unkown" ]] ; then
        echo "Arch detect Error : ARCH $arch" ; exit 1
fi


flags=$(grep UCLIBC_EXTRA_CFLAGS= $1)
flags="${flags/UCLIBC_EXTRA_CFLAGS=}"
flags="${flags/\"}"
flags="${flags/\"}"

flat=$(grep UCLIBC_FORMAT_FLAT= $1)
flat="${flat/UCLIBC_FORMAT_FLAT=}"
flat="${flat/\"}"
flat="${flat/\"}"



gcc=$(${CONFIG_GCC_PREFIX}gcc --version | head -n 1)


echo "Workflow: $2"
echo "GCC     : $gcc"
echo "ARCH    : $arch"
echo "SUBARCH : $subarch"
echo "MMU     : $mmu"
echo "BITS    : $bits"
echo "ENDIAN  : $endian"
echo "FPU     : $float"
echo "THREADS : $threads"
echo "LINKMODE: $shared"
echo "VDSO    : $vdso"
echo "FLAGS   : $flags"
echo "FLAT    : $flat"




json=$(jq -n --arg workflow "$2" --arg gcc "$gcc" --arg arch "$arch" --arg subarch "$subarch" --arg mmu "$mmu" --arg bits "$bits" --arg endian "$endian" --arg float "$float" \
        --arg threads "$threads" --arg shared "$shared" --arg vdso "$vdso" --arg flags "$flags" --arg flat "$flat" \
        --arg CONFIG_KERNEL_VERS "$CONFIG_KERNEL_VERS" --arg CONFIG_KERNEL_DIR "$CONFIG_KERNEL_DIR" --arg CONFIG_KERNEL_ARCH "$CONFIG_KERNEL_ARCH" \
        --arg CONFIG_TOOLCHAIN "$CONFIG_TOOLCHAIN"  --arg CONFIG_GCC_PREFIX "$CONFIG_GCC_PREFIX"  --arg CONFIG_FILE "$CONFIG_FILE" \
        --arg CONFIG_QEMU_KERNEL "$CONFIG_QEMU_KERNEL" --arg CONFIG_QEMU_CMD "$CONFIG_QEMU_CMD"  \
        '{ "WORKFLOW": $workflow, "GCC": $gcc, "UCLIBC_ARCH": $arch, "UCLIBC_ARCHSUB": $subarch, "UCLIBC_MMU": $mmu, "UCLIBC_BITS": $bits, "UCLIBC_ENDIAN": $endian, "UCLIBC_FPU": $float, "UCLIBC_THREADS": $threads, "UCLIBC_LINKMODE": $shared, "UCLIBC_VDSO": $vdso, "UCLIBC_FLAGS": $flags, "UCLIBC_FORMAT_FLAT": $flat, "CONFIG_KERNEL_VERS": $CONFIG_KERNEL_VERS, "CONFIG_KERNEL_DIR": $CONFIG_KERNEL_DIR, "CONFIG_KERNEL_ARCH": $CONFIG_KERNEL_ARCH, "CONFIG_TOOLCHAIN": $CONFIG_TOOLCHAIN, "CONFIG_GCC_PREFIX": $CONFIG_GCC_PREFIX, "CONFIG_FILE": $CONFIG_FILE, "CONFIG_QEMU_KERNEL": $CONFIG_QEMU_KERNEL, "CONFIG_QEMU_CMD": $CONFIG_QEMU_CMD }')

mkdir -p artifacts
echo $json | python3 -m json.tool > artifacts/infos.json


