inst = 'J120'
file1 = open('heur'+inst, 'r')
file2 = open('first'+inst, 'r')
file3 = open('random'+inst, 'r')
n = 0
precTotHeur = 0
flexTotHeur = 0
precTotFirst = 0
flexTotFirst = 0
precTotRandom = 0
flexTotRandom = 0

for line in file1 :
    #next(file1)
    next(file2)
    next(file3)
    precHeur = float(next(file1))
    flexHeur = float(next(file1))
    precFirst = float(next(file2))
    flexFirst = float(next(file2))
    precRandom = float(next(file3))
    flexRandom = float(next(file3))
    precTotHeur += precHeur
    flexTotHeur += flexHeur
    precTotFirst += precFirst
    flexTotFirst += flexFirst
    precTotRandom += precRandom
    flexTotRandom += flexRandom
    n += 1
    next(file1)
    next(file2)
    next(file3)

print("Heuristic:")
print("Mean precedences: " + str(precTotHeur/n))
print("Mean flexibility: " + str(flexTotHeur/n))
print("")
print("First chain:")
print("Mean precedences: " + str(precTotFirst/n))
print("Mean flexibility: " + str(flexTotFirst/n))
print("")
print("Random chain:")
print("Mean precedences: " + str(precTotRandom/n))
print("Mean flexibility: " + str(flexTotRandom/n))
