file1 = open('flex - interior_1', 'r')
file2 = open('flex - interior_2', 'r')
aantal1 = 0
aantal2 = 0
for line in file1 :
    if float(line) > float(next(file2)) :
        aantal1 += 1
    else :
        aantal2 += 1

print aantal1
print aantal2
