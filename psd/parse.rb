psd = open "test.psd", "rb"
# file_header_size = 4 + 2 + 6 + 2 + 4 + 4 + 2 + 2
file_header_size = 2 + 6 + 2 + 4 + 4 + 2 + 2
puts psd.read(4)
psd.read(2)
psd.read(6)
psd.read(2)
psd.read(4)
psd.read(4)
psd.read(2)
color_mode = psd.read(2)
puts color_mode.class
puts color_mode.size
puts color_mode.unpack("L")
