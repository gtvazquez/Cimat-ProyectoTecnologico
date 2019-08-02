### To Run: "python3 a b c" where  'a' is the method number, 'b' represent the  dimension and  'c' table divisions
### python3 1 10 7

import sys

names = ["elDE", "elDE_SHADE", "elDE_EDM", "elDE_EDM_v2"]

method  = names[int(sys.argv[1])-1]
dimension = int(sys.argv[2])



Best = []
Median = []
c = []
v = []
Mean = []
Worst = []
STD = []
SR = []
vio = []


for i in range (1,29):
    fileName = "Result/Table/"+method+"_"+str(i)+"_"+str(dimension)+".txt"
    file = open(fileName, "r")
    text = file.read().split()
    Best.append(text[1])
    Median.append(text[3])
    c.append(text[5])
    v.append(text[7])
    Mean.append(text[9])
    Worst.append(text[11])
    STD.append(text[13])
    SR.append(text[15])
    vio.append(text[17])
    file.close()


fileNameOut = "Result/Latex/"+method+"_"+str(dimension)
file = open(fileNameOut, "w")

file.write("\\begin{center} \n")
file.write("  \\begin{tabular}{|c|c|c|c|c|c|c|c|}\n")
file.write("    \\hline \n")
size_h = int(sys.argv[3])
size = int(28/ size_h)
for i in range (size):
    file.write("    \\textbf{Problem} ")
    for j in range(size_h):
        if (size_h*i+j+1 > 9):
            file.write("& \\textbf{C"+str(size_h*i+j+1)+"} ")
        else:
            file.write("& \\textbf{C0"+str(size_h*i+j+1)+"} ")
    file.write("\\\\ \n    \\hline\\hline \n")
    
    file.write("    \\textbf{Best}")
    for j in range(size_h):
        file.write(" & "+Best[size_h*i+j])
    file.write("\\\\ \n")

    file.write("    \\textbf{Median}")
    for j in range(size_h):
        file.write(" & "+Median[size_h*i+j])
    file.write("\\\\ \n")

    file.write("    \\textbf{c}")
    for j in range(size_h):
        file.write(" & "+c[size_h*i+j])
    file.write("\\\\ \n")

    file.write("    \\textbf{v}")
    for j in range(size_h):
        file.write(" & "+v[size_h*i+j])
    file.write("\\\\ \n")

    file.write("    \\textbf{Mean}")
    for j in range(size_h):
        file.write(" & "+Mean[size_h*i+j])
    file.write("\\\\ \n")

    file.write("    \\textbf{Worst}")
    for j in range(size_h):
        file.write(" & "+Worst[size_h*i+j])
    file.write("\\\\ \n")

    file.write("    \\textbf{STD}")
    for j in range(size_h):
        file.write(" & "+STD[size_h*i+j])
    file.write("\\\\ \n")
    
    file.write("    \\textbf{SR}")
    for j in range(size_h):
        file.write(" & "+SR[size_h*i+j])
    file.write("\\\\ \n")

    file.write("    \\textbf{vio}")
    for j in range(size_h):
        file.write(" & "+vio[size_h*i+j])
    file.write("\\\\ \n")

    file.write("    \\hline \n")
    if(i + 1 <size):
        file.write("    \\hline \n")

    
file.write("  \\end{tabular}\n")
file.write("\\end{center}\n")

file.close()
