# Requirement
```
sudo apt-get install libopencv-dev
```

# Install
```
cmake .
```

# Usage
```
./posternn [-i input_file] [-l posterization_level] [-o output_file]
```
Example:
./posternn -i myimage.jpg -l 4 -o posterized.jpg
