### To Run : python3  plotter.py a b c where ´a´ is the function number, ´b´ represent the dimension  and ´c´ max generations
# python3 plotter.py 1 10 1000


import matplotlib.pyplot as plt
import sys


typeStatistics = ["Mean f", "Mean Diversity", "SR"]
names = ["elDE", "elDE_SHADE", "elDE_EDM", "elDE_EDM_v2","elDE_EDM_C"]


for statistics in typeStatistics:
  plt.title(statistics+sys.argv[1])
  plt.ylabel(r'$f( \overrightarrow{x})$')
  plt.xlabel('iterations')

  if statistics == "Mean Diversity":
        names.append("Dt")

  for method in names:
    func_num = sys.argv[1]
    dimension = sys.argv[2]
    name = "Result/"+statistics+"/Resumen_"+method+"_"+func_num+"_"+dimension+".txt"
    
    X1, Y1 = [], [];
    k = 0;

    for line in open(name, 'r'):
      values = [s for s in line.split()]
      X1.append(float(values[1]))
      Y1.append(k)
      k += 1

    X1 = X1 [0:int(sys.argv[3])]
    Y1 = Y1 [0:int(sys.argv[3])]

    plt.plot(Y1, X1 , label = method)

  if statistics == "Mean Diversity":
        names.remove("Dt")

  nameOutout = "Result/Graphs/"+func_num+"_"+dimension+"_"+statistics+".png"
  plt.legend( loc = 'upper right');
  plt.savefig(nameOutout)
  plt.close()
