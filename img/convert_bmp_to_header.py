#!/usr/bin/env python3
import argparse

parser = argparse.ArgumentParser()

parser.add_argument("bmp",
    help="Path to bmp to convert to C header file")
parser.add_argument("--var-name",
   help="name of the variable to make the bmp data available",
   default="IMAGE_DATA")
parser.add_argument("-o", "--output",
   help="Path where to create C header file",
   required=True)
args = parser.parse_args()

with open(args.output, "w", encoding="utf-8") as f:
    # conversion script based on:
    # https://stackoverflow.com/questions/18422123/sdl-embed-image-inside-program-executable
    f.write("static const unsigned char {:s}[] = {{{:s}}};".format(
        args.var_name,
        ",".join(str(b) for b in open(args.bmp, "rb").read())))
