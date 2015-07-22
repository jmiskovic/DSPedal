import argparse
import struct
import os
import re

DELIMIT = '_|_'
MAGIC   = ord('@')

def struct_size_from_dwarf(dwarf_file):
	"""	
	 <1><39f>: Abbrev Number: 3 (DW_TAG_typedef)
	    <3a0>   DW_AT_name        : (indirect string, offset: 0x666): mydsp	
	    <3a4>   DW_AT_decl_file   : 1	
	    <3a5>   DW_AT_decl_line   : 35	
	    <3a6>   DW_AT_type        : <0x2fe>		
	 <1><2fe>: Abbrev Number: 9 (DW_TAG_structure_type)
	    <2ff>   DW_AT_byte_size   : 60	
	    <300>   DW_AT_decl_file   : 1	
	    <301>   DW_AT_decl_line   : 22	
	    <302>   DW_AT_sibling     : <0x37f>	    
	"""
	struct_size = None
	found_record = False
	struct_id = 'xxx'
	for line in dwarf_file:
		if 'DW_AT_name' in line and 'mydsp' in line:
			found_record = True
			continue
		if found_record and 'DW_AT_type' in line:
			struct_id = re.findall(r'(?<=\<0x)[0-9a-f]+', line)[0]
			break;

	dwarf_file.seek(0)

	found_record = False
	for line in dwarf_file:
		if 'DW_TAG_structure_type' in line and struct_id in line:
			found_record = True
			continue
		if found_record and 'DW_AT_byte_size' in line:
			struct_size = re.findall(r'(?<=\: )[x0-9a-f]+', line)[0]
			break
	return int(struct_size,0)

parser = argparse.ArgumentParser(description='Construct fx effect container from code dumps')

# --name looper --M0gui out/M0gui.dump --M0init out/M0init.dump --M4dsp out/M4dsp.dump

parser.add_argument('-o', '--out', 	required=True, help='output filename')
parser.add_argument('--name',   	required=True, help='name of effect')
parser.add_argument('--M0gui',  	required=True, help='path of M0 gui init code dump')
parser.add_argument('--M0init', 	required=True, help='path of M0 dsp init code dump')
parser.add_argument('--M4dsp',  	required=True, help='path of M4 dsp code dump')
parser.add_argument('--M4alloc',	required=True, help='path of M4 data allocation code dump')

args = parser.parse_args()
name = args.name[:14].title()

with open(args.M0gui, 'rb') as f:
	M0gui = f.read()
with open(args.M0init, 'rb') as f:
	M0init = f.read()
with open(args.M4dsp, 'rb') as f:
	M4dsp = f.read()
with open(args.M4alloc, 'rb') as f:
	size_data = struct_size_from_dwarf(f)

# struct EffectHeader {
#     char magicPattern;
#     char name[15];
#     uint16_t size_M0guicode;
#     uint16_t size_M0initcode;
#     uint16_t size_M4dspcode;
#     uint32_t size_data;
# };

header = struct.pack('<B15sHHHI',
	MAGIC,
	name,
	len(M0gui),
	len(M0init),
	len(M4dsp),
	size_data)

total_size = 0

with open(args.out, 'wb') as fx:
	fx.write(header)
	fx.write(DELIMIT)
	fx.write(M0gui)
	fx.write(DELIMIT)
	fx.write(M0init)
	fx.write(DELIMIT)
	fx.write(M4dsp)
	fx.write(DELIMIT)
	total_size = fx.tell()

print('Created %-15s M0gui=%04X\tM0init=%04X\tM4dsp=%04X\tSDRAM=%08X\ttotal=%08X (%08d)' % (name, len(M0gui), len(M0init), len(M4dsp), size_data, total_size, total_size))