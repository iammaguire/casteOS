from __future__ import print_function

with open('ter-powerline-x12b.bdf') as fp:
    indata = False
    index = 0
    char_index = 0

    face_name = ""
    weight_name = ""
    cur_char = ""

    pixel_size = 0
    char_count = 0
    data = 0
    char_names = 0

    for line in fp:
            if pixel_size != 0 and char_count != 0 and data == 0:
                data = [['00' for x in range(pixel_size)] for y in range(char_count)] 
                char_names = ['' for x in range(char_count)]
            if "PIXEL_SIZE " in line:
                pixel_size = int(line[11:])
            if "FAMILY_NAME " in line:
                face_name = line[13:-2]
            if "WEIGHT_NAME " in line:
                weight_name = line[13:-2]
            if "CHARS " in line and not "CHARSET" in line:
                char_count = int(line[6:])
            if "STARTCHAR" in line:
                cur_char = line[10:-1]
            if "BITMAP" in line:
                indata = True
                continue
            elif "ENDCHAR" in line:
                indata = False
                char_index += 1
            if indata:
                data[char_index][index] = line[:-1]
                char_names[char_index] = cur_char
                index += 1
                if index >= pixel_size:
                    index = 0

print("uint8_t " + face_name + "_" + weight_name + "[" + str(len(data)) + "][" + str(len(data[0])) + "] = {")
for x in range(len(data)):
    print("\t{ ", end="")
    for y in range(len(data[x])):
        print("0x" + data[x][y] + (", " if y != len(data[x]) - 1 else " "), end="")
    print("}" + ("," if x != len(data) - 1 else "") + " // " + str(x) + " is " + char_names[x])
print("};")
