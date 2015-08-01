import sys
import os

command_template = 'arm-none-eabi-objcopy %s %s --set-section-flags .dsp_%s%s%s=contents,noload,readonly,code 2> /dev/null'

filename = sys.argv[1]

for arg in sys.argv[2:]:
	command = command_template % (filename, filename, arg, 'M4', 'dsp')
	os.system(command)
	command = command_template % (filename, filename, arg, 'M4', 'alloc')
	os.system(command)
