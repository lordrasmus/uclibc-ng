

# uClibc-ng Git Version Testing Fork

This fork is intended for testing the current Git version of uClibc-ng in various configurations. We will be testing combinations of architecture, compiler, kernel, and configuration to ensure its functionality.

To utilize GitHub Actions for testing, you can simply create a pull request.

In case of any errors, you can download the archive artifacts. It contains the kernel, toolchain, and configuration files that were used in the testing.

The toolchains have been built for Ubuntu 22.04 but may also work on other distributions.

Feel free to contribute and help improve uClibc-ng by testing it in different setups.


Discord : https://discord.gg/x7xm7EXW


Upstream Links

https://uclibc-ng.org/

https://cgit.uclibc-ng.org/cgi/cgit/uclibc-ng.git/

|Arch  |Status|Testsuite Image|
|------|------|------|
|aarch64-4.19.56|[![aarch64-4.19.56](https://img.shields.io/github/actions/workflow/status/lordrasmus/uclibc-ng/make-aarch64-4.19.56.yml?style=flat)](https://github.com/lordrasmus/uclibc-ng/actions/workflows/make-aarch64-4.19.56.yml)|:white_check_mark:
|alpha-4.19.56|[![alpha-4.19.56](https://img.shields.io/github/actions/workflow/status/lordrasmus/uclibc-ng/make-alpha-4.19.56.yml?style=flat)](https://github.com/lordrasmus/uclibc-ng/actions/workflows/make-alpha-4.19.56.yml)|:white_check_mark:
|arc|[![arc](https://img.shields.io/github/actions/workflow/status/lordrasmus/uclibc-ng/make-arc.yml?style=flat)](https://github.com/lordrasmus/uclibc-ng/actions/workflows/make-arc.yml)|:white_check_mark:
|arm-cortex-a7|[![arm-cortex-a7](https://img.shields.io/github/actions/workflow/status/lordrasmus/uclibc-ng/make-arm.yml?style=flat)](https://github.com/lordrasmus/uclibc-ng/actions/workflows/make-arm.yml)|:white_check_mark:
|bfin|[![bfin](https://img.shields.io/github/actions/workflow/status/lordrasmus/uclibc-ng/make-bfin.yml?style=flat)](https://github.com/lordrasmus/uclibc-ng/actions/workflows/make-bfin.yml)|:x:
|c6x|[![c6x](https://img.shields.io/github/actions/workflow/status/lordrasmus/uclibc-ng/make-c6x.yml?style=flat)](https://github.com/lordrasmus/uclibc-ng/actions/workflows/make-c6x.yml)|:x:
|cris|[![cris](https://img.shields.io/github/actions/workflow/status/lordrasmus/uclibc-ng/make-cris.yml?style=flat)](https://github.com/lordrasmus/uclibc-ng/actions/workflows/make-cris.yml)|:white_check_mark:
|csky|[![csky](https://img.shields.io/github/actions/workflow/status/lordrasmus/uclibc-ng/make-csky.yml?style=flat)](https://github.com/lordrasmus/uclibc-ng/actions/workflows/make-csky.yml)|:x:
|h8300|[![h8300](https://img.shields.io/github/actions/workflow/status/lordrasmus/uclibc-ng/make-h8300.yml?style=flat)](https://github.com/lordrasmus/uclibc-ng/actions/workflows/make-h8300.yml)|:x:
|hppa|[![hppa](https://img.shields.io/github/actions/workflow/status/lordrasmus/uclibc-ng/make-hppa.yml?style=flat)](https://github.com/lordrasmus/uclibc-ng/actions/workflows/make-hppa.yml)|:white_check_mark:
|ia64|[![ia64](https://img.shields.io/github/actions/workflow/status/lordrasmus/uclibc-ng/make-ia64.yml?style=flat)](https://github.com/lordrasmus/uclibc-ng/actions/workflows/make-ia64.yml)|:x:
|kvx|[![kvx](https://img.shields.io/github/actions/workflow/status/lordrasmus/uclibc-ng/make-kvx.yml?style=flat)](https://github.com/lordrasmus/uclibc-ng/actions/workflows/make-kvx.yml)|:x:
|m68k|[![m68k](https://img.shields.io/github/actions/workflow/status/lordrasmus/uclibc-ng/make-m68k.yml?style=flat)](https://github.com/lordrasmus/uclibc-ng/actions/workflows/make-m68k.yml)|:x:
|microblazeel|[![microblazeel](https://img.shields.io/github/actions/workflow/status/lordrasmus/uclibc-ng/make-microblazeel.yml?style=flat)](https://github.com/lordrasmus/uclibc-ng/actions/workflows/make-microblazeel.yml)|:white_check_mark:
|mips32el|[![mips32el](https://img.shields.io/github/actions/workflow/status/lordrasmus/uclibc-ng/make-mips.yml?style=flat)](https://github.com/lordrasmus/uclibc-ng/actions/workflows/make-mips.yml)|:white_check_mark:
|mips64-n32-4.19.56|[![mips64-n32-4.19.56](https://img.shields.io/github/actions/workflow/status/lordrasmus/uclibc-ng/make-mips64-n32-4.19.56.yml?style=flat)](https://github.com/lordrasmus/uclibc-ng/actions/workflows/make-mips64-n32-4.19.56.yml)|:white_check_mark:
|mips64el-n32-6.1.60|[![mips64el-n32-6.1.60](https://img.shields.io/github/actions/workflow/status/lordrasmus/uclibc-ng/make-mips64le-n32-6.1.60.yml?style=flat)](https://github.com/lordrasmus/uclibc-ng/actions/workflows/make-mips64le-n32-6.1.60.yml)|:white_check_mark:
|nds32le|[![nds32le](https://img.shields.io/github/actions/workflow/status/lordrasmus/uclibc-ng/make-nds32le.yml?style=flat)](https://github.com/lordrasmus/uclibc-ng/actions/workflows/make-nds32le.yml)|:white_check_mark:
|nios2|[![nios2](https://img.shields.io/github/actions/workflow/status/lordrasmus/uclibc-ng/make-nios2.yml?style=flat)](https://github.com/lordrasmus/uclibc-ng/actions/workflows/make-nios2.yml)|:white_check_mark:
|or1k|[![or1k](https://img.shields.io/github/actions/workflow/status/lordrasmus/uclibc-ng/make-or1k.yml?style=flat)](https://github.com/lordrasmus/uclibc-ng/actions/workflows/make-or1k.yml)|:white_check_mark:
|riscv64-shared|[![riscv64-shared](https://img.shields.io/github/actions/workflow/status/lordrasmus/uclibc-ng/make-riscv64-shared.yml?style=flat)](https://github.com/lordrasmus/uclibc-ng/actions/workflows/make-riscv64-shared.yml)|:white_check_mark:
|riscv64-static|[![riscv64-static](https://img.shields.io/github/actions/workflow/status/lordrasmus/uclibc-ng/make-riscv64-static.yml?style=flat)](https://github.com/lordrasmus/uclibc-ng/actions/workflows/make-riscv64-static.yml)|:x:
|sparc64-4.19.56|[![sparc64-4.19.56](https://img.shields.io/github/actions/workflow/status/lordrasmus/uclibc-ng/make-sparc64-4.19.56.yml?style=flat)](https://github.com/lordrasmus/uclibc-ng/actions/workflows/make-sparc64-4.19.56.yml)|:white_check_mark:
