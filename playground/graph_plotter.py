import matplotlib.pyplot as plt 
  
names = [] 
marks = [] 
  
f = open('output.csv','r')
for row in f: 
    row = row.split(',')
    names.append(row[0]) 
    marks.append(int(float(row[1].strip())))
  
plt.plot(names, marks, color = 'g', label = 'File Data') 

# Set the number of x-ticks to display
plt.xticks(ticks=range(0, len(names), len(names)//5), labels=[names[i] for i in range(0, len(names), len(names)//5)])

plt.xlabel('Time', fontsize = 12) 
plt.ylabel('Voltage', fontsize = 12) 

plt.legend() 
plt.show()