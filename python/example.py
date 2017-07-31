"""
    example.py
    
VARPA, University of Coruna
Mondejar Guerra, Victor M.
31 Jul 2017
"""

from QRS_detector import QRSDetectorOffline
from QRS_classifier import QRSClassifier
import csv
import sys

def load_signal( filename ):
    # Read data from file .csv 
    ecg_signal = list()
    with open(filename, 'rb') as f:
        reader = csv.reader(f)
        for row in reader:
            ecg_signal.append(float(row[0]))

    fs = ecg_signal[0]
    min_A = ecg_signal[1]
    max_A = ecg_signal[2]
    n_bits = ecg_signal[3]
    ecg_signal = ecg_signal[4:]   
    
    return ecg_signal, fs, min_A, max_A, n_bits

if __name__ == "__main__":

    if len(sys.argv) < 2:
	    print 'Error an argument is needed! Example of use:\n\tpython example.py ../data/220.csv'
	    sys.exit()
    
    # TODO: update this function to load your own data
    ecg_signal, fs, min_A, max_A, n_bits = load_signal(sys.argv[1])

    # Detect R-peak points
    # if fs != 360 ecg_data will be resampled
    qrs_detector = QRSDetectorOffline(ecg_data_raw = ecg_signal, fs = fs,\
                                      verbose=False, plot_data=False,    \
                                      show_plot=False)

    # qrs_detector.ecg_data_raw     signal at original frecuency
    # qrs_detector.ecg_data         signal resampled at 360Hz
    # qrs_detector.qrs_peaks_indices contains R peaks at 360 Hz signal
    # qrs_detector.qrs_peaks_indices_fs contains R peaks at original frequency

    # Classify Beat 
    qrs_classifier = QRSClassifier(svm_models_path = '../svm_models', \
                    ecg_data = qrs_detector.ecg_data, \
                    qrs_peaks_indices = qrs_detector.qrs_peaks_indices, \
                    qrs_peaks_indices_fs = qrs_detector.qrs_peaks_indices_fs, \
                    fs = fs, min_A = min_A, max_A = max_A, show_results = False) 
