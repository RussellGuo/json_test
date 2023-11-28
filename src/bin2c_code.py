#!python

with open("C:\\Users\\101002537\\Documents\\projects\\financial\\sign_verify_mcu\\src\\sign.bin", "rb") as f:
    data = f.read()
for i in range(len(data)):
    print("    0x%02X," % data[i], end=' ' if i % 16 != 15 else '\n')
