file1 = open('flex - simplex_2', 'r')
file2 = open('flex - interior_2', 'r')
aantal1 = 0
gelijk = 0
aantal2 = 0
for line in file1 :
    c2 = float(next(file2))
    if float(line) < c2 :
        aantal1 += 1
    elif float(line) == c2 :
        gelijk += 1
    else :
        aantal2 += 1

print aantal1
print gelijk
print aantal2
