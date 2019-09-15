import matplotlib.pyplot as plt
import sys

names = ["e1.txt", "e2.txt"]


for name in names:
    X1, Y1 = [], [];
    k = 0;

    for line in open(name, 'r'):
        values = [s for s in line.split()]
        X1.append(float(values[0]))
        Y1.append(k)
        k += 1
    
    X1 = X1 [0:int(sys.argv[1])]
    Y1 = Y1 [0:int(sys.argv[1])]

    plt.plot(Y1, X1 , label = name)


plt.legend( loc = 'upper right');
plt.savefig("EPS.png")
plt.close()

