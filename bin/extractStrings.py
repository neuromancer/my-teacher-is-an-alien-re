#!/usr/bin/env python3
"""
Extract strings from a PE executable's data sections.
Outputs strings with their virtual addresses.
"""
import sys
import struct
import argparse

def parse_pe(filename):
    """Parse PE and return sections list"""
    with open(filename, 'rb') as f:
        f.seek(0x3C)
        pe_off = struct.unpack('<I', f.read(4))[0]
        
        f.seek(pe_off + 6)
        num_sections = struct.unpack('<H', f.read(2))[0]
        
        f.seek(pe_off + 20)
        opt_size = struct.unpack('<H', f.read(2))[0]
        
        f.seek(pe_off + 52)
        image_base = struct.unpack('<I', f.read(4))[0]
        
        section_start = pe_off + 24 + opt_size
        f.seek(section_start)
        
        sections = []
        for i in range(num_sections):
            name = f.read(8).rstrip(b'\x00').decode('ascii', errors='ignore')
            vsize = struct.unpack('<I', f.read(4))[0]
            va = struct.unpack('<I', f.read(4))[0]
            rawsize = struct.unpack('<I', f.read(4))[0]
            rawptr = struct.unpack('<I', f.read(4))[0]
            f.read(12)
            flags = struct.unpack('<I', f.read(4))[0]
            sections.append({
                'name': name,
                'va': image_base + va,
                'vsize': vsize,
                'rawptr': rawptr,
                'rawsize': rawsize
            })
        
        return image_base, sections

def extract_strings(filename, min_length=4):
    """Extract printable strings from .data and .rdata sections"""
    image_base, sections = parse_pe(filename)
    
    strings = []
    
    with open(filename, 'rb') as f:
        for sec in sections:
            if sec['name'] not in ['.data', '.rdata']:
                continue
                
            f.seek(sec['rawptr'])
            data = f.read(sec['rawsize'])
            
            # Find null-terminated ASCII strings
            current_str = b''
            str_start = 0
            
            for i, byte in enumerate(data):
                if 32 <= byte < 127:  # Printable ASCII
                    if len(current_str) == 0:
                        str_start = i
                    current_str += bytes([byte])
                elif byte == 0 and len(current_str) >= min_length:
                    # End of string
                    va = sec['va'] + str_start
                    strings.append({
                        'va': va,
                        'content': current_str.decode('ascii'),
                        'section': sec['name']
                    })
                    current_str = b''
                else:
                    current_str = b''
    
    return strings

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('exe', help='PE executable to extract strings from')
    parser.add_argument('--min-length', type=int, default=4, help='Minimum string length')
    args = parser.parse_args()
    
    strings = extract_strings(args.exe, args.min_length)
    
    for s in strings:
        print(f"0x{s['va']:08X}: {repr(s['content'])}")

if __name__ == '__main__':
    main()
