@REM qemu-system-x86_64.exe -hda Image/x64BareBonesImage.qcow2 -m 512 -soundhw pcspk
@echo off

IF "%1"=="gdb" (
    qemu-system-x86_64.exe -s -S -hda Image/x64BareBonesImage.qcow2 -m 512 -d int -soundhw pcspk
) ELSE (
    qemu-system-x86_64.exe -hda Image/x64BareBonesImage.qcow2 -m 512 -soundhw pcspk
)
