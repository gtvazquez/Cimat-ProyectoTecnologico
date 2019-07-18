import matplotlib.pyplot as plt
import sys


names = ["elDE", "elDE_EDM", "elDE_SHADE"]

method = names[int(sys.argv[1])-1]
func_num = sys.argv[2]
dimension = int(sys.argv[3])


name = "Resumen_"+method+"_"+func_num+"_"+str(dimension)+".txt"

X = [ i for i in range(4000) ]
Y = [ [] for i in range(dimension) ]



for line in open(name, 'r'):
    values = line.split()
    k = 2
    for j in range(dimension):
        Y[j].append(float(values[k]))
        k = k + 2
    



plt.title("SD")
plt.ylabel(r'$SD( \overrightarrow{x_i})$')
plt.xlabel('iterations')


k = 1
for y in Y:
    name = "x" + str(k)
    y = y[0:int(sys.argv[4])]
    X = X[0:int(sys.argv[4])]
    plt.plot(X, y , label = name )
    k = k + 1


nameOutout = "Graphs/"+method+"_"+func_num+"_"+str(dimension)+".png"
plt.legend( loc = 'upper right')
plt.savefig(nameOutout)
