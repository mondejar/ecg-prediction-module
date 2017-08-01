#!/usr/bin/python
################################################################################
# Given the original signal and the file output generated from module: 
# ecg_prediction_module this code display a plot that remark: 
#
# 	1. each R-peaks (vertical lines) 
#	2. show a different color depending on the class predicted for each beat:
#   N		0	(Green) 
# 	SVEB	1 	(Red)
# 	VEB		2	(Pink)
#   F 		3	(Yellow)
#	Q		4	(Blue)
#
# Example of use:
#	python show_results ../data/220.csv ../data/220.csv.out
#
#
# Mondejar-Guerra, V.M
# 10 Jul 2017
################################################################################

import matplotlib.pyplot as plt
import numpy as np
import csv
import sys

if len(sys.argv) < 3:
	print 'Error two arguments needed! Example of use:\n\tpython show_results ../data/220.csv ../data/220.csv.out'
	sys.exit()

# Read original signal
ecg_signal = list()
with open(sys.argv[1], 'rb') as f:
    reader = csv.reader(f)
    for row in reader:
        ecg_signal.append(row)

# Read R-peaks
r_peaks = list()
predicted_class = list()

with open(sys.argv[2], 'rb') as f:
    reader = csv.reader(f)
    for row in reader:
		r_peaks.append(int(row[0]))
		predicted_class.append(int(row[1]))

# Show data
begin = 5
colors = ["green", "red", "pink", "yellow", "blue"]
for r in range(0, len(r_peaks)):
	plt.axvline(x = r_peaks[r], color = 'k', linestyle='--')
	print 'R peak: ', r_peaks[r], ' predicted class : ', predicted_class[r]
	x = range(begin,r_peaks[r])
	y = ecg_signal[begin:r_peaks[r]]
	plt.plot(x, y, color=colors[predicted_class[r]], linewidth=2.5, linestyle="-")
	begin = r_peaks[r]

plt.show()

