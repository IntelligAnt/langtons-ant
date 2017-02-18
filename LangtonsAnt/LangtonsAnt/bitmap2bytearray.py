from PIL import Image
import os
import sys


black = (0, 0, 0)
white = (255, 255, 255)

def bitmap2bytearray(filename):
    ba = bytearray()
    
    with Image.open(filename) as img:
        byte = ''
        for i, pixel in enumerate(img.getdata()):
            byte += '1' if pixel != white else '0'
            if (i+1) % 8 == 0:
                ba.append(int(byte, 2))
                byte = ''
        if len(byte) > 0:
            ba.append(int(byte, 2))
    
    return ba


def c_literal(ba, name):
    s = 'static const byte %s[] = {' % name
    ba_hex = ["0x%02X" % b for b in ba]
    w = max(int((len(ba)*8)**0.5 / 2), 8)
    
    for i, bh in enumerate(ba_hex):
        if i % w == 0:
            s += '\n\t' if i == 0 else ',\n\t'
        else:
            s += ', '
        s += bh
        
    return s + '\n};'


if __name__ == '__main__':
    while True:
        filename = input("Bitmap file: ")
        if not os.path.isfile(filename):
            print("File not found", file=sys.stderr)
            continue
        
        ba = bitmap2bytearray(filename)
        print(ba)
        name = os.path.splitext(os.path.basename(filename))[0]
        print(c_literal(ba, name))
