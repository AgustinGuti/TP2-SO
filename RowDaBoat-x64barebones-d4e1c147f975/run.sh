#!/bin/bash
sudo chmod 777 Image/x64BareBonesImage.qcow2
qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -soundhw pcspk -m 512 

