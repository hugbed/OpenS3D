#!/usr/bin/env python

# Script to separate raw 3D video files
# that have both images in the same file
# (e.g, frame1Left, frame1Right, frame2Left, frame2Right)
#
# Separates these frames in 2 files
# Left: frame1, frame2
# Right: frame1, frame2
#
# Usage: ./separate_3d.py input_file out_L out_R
#
# Currently, the image size in bytes is hardcoded in nbBytes

import sys

# Set the number of bytes per frame here
nbBytes = 1920*1080*2

if len(sys.argv) < 4:
    print ("oh oh, should be: %s in out_L out_R")%(sys.argv[0])
    exit(0)

in_fn = sys.argv[1] 
outL_fn = sys.argv[2]
outR_fn = sys.argv[3]

print ("From %s, to %s, %s")%(in_fn, outL_fn, outR_fn)

with open(in_fn, 'rb') as in_f:
    with open(outL_fn, 'wb') as outL_f:
        with open(outR_fn, 'wb') as outR_f:
            frameL = in_f.read(nbBytes)
            frameR = in_f.read(nbBytes)
            while frameL and frameR:
                outL_f.write(frameL)
                outR_f.write(frameR)
                frameL = in_f.read(nbBytes)
                frameR = in_f.read(nbBytes)
