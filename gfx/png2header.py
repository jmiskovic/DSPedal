from os import listdir
import os
from math import ceil
import collections
from PIL import Image
from PIL.ImageOps import invert

group = lambda flat, size: [flat[i:i+size] for i in range(0,len(flat), size)]

transpose = lambda m: map(list,zip(*m))

def flatten(l):
    for el in l:
        if isinstance(el, collections.Iterable) and not isinstance(el, basestring):
            for sub in flatten(el):
                yield sub
        else:
            yield el

menu_list = filter(lambda x: x.endswith('png'), listdir('gfx'))

header = \
"""/* WARNING: auto-generated file */
#ifndef BITMAPS_H
#define BITMAPS_H

typedef struct
{
    uint16_t width;
    uint16_t height;
    const char * data_pointer;
} bitmap;
\n\n"""

source = \
"""/* WARNING: auto-generated file */
#include "stdint.h"
#include "bitmaps.h"
\n\n"""

def imagefile2array(filename):
    ''' Returns tuple of (width, height, string_definition). '''
    img = Image.open(filename)
    if img.mode == 'RGBA':
        img.load()
        bmp = Image.new("RGB", img.size, (255,255,255)) # create new white image
        r,g,b,a = img.split()
        bmp = Image.composite(img, bmp, a) # create a composite
    elif img.mode == 'LA':
        img.load()
        bmp = Image.new("RGB", img.size, (255,255,255)) # create new white image
        l,a = img.split()
        bmp = Image.composite(img, bmp, a) # create a composite
    else:
        bmp = img
    bmp = invert(bmp)
    bmp = bmp.rotate(-90).transpose(Image.FLIP_LEFT_RIGHT)
    s = bmp.convert('1').tostring()
    byte_height = bmp.size[0] / 8 + (bmp.size[0] % 8 > 0)
    matrix = group(list(s), byte_height)
    matrix = transpose(matrix)
    bmp_array = ''.join(['\\x{0:02x}'.format(ord(x)) for x in flatten(matrix)])
    return (bmp.size[1], bmp.size[0], bmp_array) # bitmap was rotated, need to swap width and height

# Scan directiory for various symbols
for fname in menu_list:
    width, height, stringdef = imagefile2array(os.path.join('gfx', fname))
    img_name = fname[:-4]
    header += 'extern const bitmap %s_bitmap;\n' % img_name
    source += 'const bitmap %s_bitmap = {%d, %d, "%s"};\n' % (img_name, width, height, stringdef)

header += '#endif /* BITMAPS_H */\n'

with open('./src/bitmaps.h', 'w') as fp:
    fp.write(header)

with open('./src/bitmaps_M0.c', 'w') as fp:
    fp.write(source)