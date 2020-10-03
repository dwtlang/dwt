import matplotlib.pyplot as plt; plt.rcdefaults()
import numpy as np
import matplotlib.pyplot as plt
import csv

objects = []
performance = []

with open('results.csv','r') as csvfile:
    plots = csv.reader(csvfile, delimiter=',')
    for row in plots:
        objects.append(row[0])
        performance.append(float(row[1]))

y_pos = np.arange(len(objects))

fig, ax = plt.subplots(1,1)
bars = ax.barh(y_pos, performance, align='center', alpha=1, color=['purple','yellowgreen','orange','darkblue','firebrick','royalblue'])
#ax.xaxis.grid()

for b in bars:
    x_val = b.get_width()
    y_val = b.get_y() + b.get_height() / 2

    space = 5
    ha = 'left'

    label = "{:.1f}".format(x_val)

    plt.annotate(label, (x_val, y_val), xytext=(space, 0), textcoords="offset points", va='center', ha=ha)

plt.axvline(performance[1], linestyle=':', color='lightgrey')
plt.yticks(y_pos, objects)
plt.xlabel('Seconds')
plt.title('Benchmark - Fibonacci')

plt.savefig("results.png")
plt.show()
