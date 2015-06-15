import argparse
import struct
import os

DELIMIT = '_|_'
MAGIC   = ord('@')

parser = argparse.ArgumentParser(description='Construct fx effect container from code dumps')

# --name looper --M0gui out/M0gui.dump --M0init out/M0init.dump --M4dsp out/M4dsp.dump

parser.add_argument('--name',   required=True, help='name of effect')
parser.add_argument('--M0gui',  required=True, help='path of M0 gui init code dump')
parser.add_argument('--M0init', required=True, help='path of M0 dsp init code dump')
parser.add_argument('--M4dsp',  required=True, help='path of M4 dsp code dump')
parser.add_argument('--M4alloc',required=True, help='path of M4 data allocation code dump')

args = parser.parse_args()
name = args.name[:14].title()

with open(args.M0gui, 'rb') as f:
	M0gui = f.read()
with open(args.M0init, 'rb') as f:
	M0init = f.read()
with open(args.M4dsp, 'rb') as f:
	M4dsp = f.read()
with open(args.M4alloc, 'rb') as f:
	M4alloc = f.read()

# struct EffectHeader {
#     char magicPattern;
#     char name[15];
#     uint16_t size_M0guicode;
#     uint16_t size_M0initcode;
#     uint16_t size_M4dspcode;
#     uint32_t size_data;
# };

size_data = struct.unpack('<I', M4alloc[-4:])[0] # last four bytes contain data size, for passing into malloc function

header = struct.pack('<B15sHHHI',
	MAGIC,
	name,
	len(M0gui),
	len(M0init),
	len(M4dsp),
	size_data)

with open(os.path.join('out', args.name + '.fx'), 'wb') as fx:
	fx.write(header)
	fx.write(DELIMIT)
	fx.write(M0gui)
	fx.write(DELIMIT)
	fx.write(M0init)
	fx.write(DELIMIT)
	fx.write(M4dsp)
	fx.write(DELIMIT)

print('Created %-15s M0gui=%04X\tM0init=%04X\tM4dsp=%04X\tdata=%08X' % (name, len(M0gui), len(M0init), len(M4dsp), size_data))