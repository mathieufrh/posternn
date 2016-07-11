from PIL import Image
from collections import defaultdict
import sys

if len(sys.argv) < 2:
    print('Please give me an image file')
    sys.exit(1)

im = Image.open(sys.argv[1])
colors = defaultdict(int)
for pixel in im.getdata():
    colors[pixel] += 1
print len(colors)
