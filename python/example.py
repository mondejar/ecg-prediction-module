"""
example.py
    
VARPA, University of Coruna
Mondejar Guerra, Victor M.
31 Jul 2017
"""

from QRS_detector import QRSDetectorOffline
from QRS_classifier import QRSClassifier
import sys
import csv
import matplotlib.pyplot as plt

"""
Read signal from text file
"""
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

"""
Show plots with signal data, r-peaks and predictions

N       Green
SVEB    Red
VEB     Pink
F       Yellow
Q       Blue
"""
def show_signal_and_predictions(qrs_detector, qrs_classifier):
    # Show data
    begin = 0
    begin_360 = 0
    colors = ["green", "red", "pink", "yellow", "blue"]

    predictions = qrs_classifier.predictions

    # Data at original frequency sampling 

    fig, axarr = plt.subplots(2, sharex=True)

    for r in range(0, len(qrs_detector.qrs_peaks_indices)):
        # Original ECG RAW
        R_peak = int(qrs_detector.qrs_peaks_indices_fs[r])
        x = range(begin, R_peak)
        y = qrs_detector.ecg_data_raw[begin:R_peak]
        #print 'R peak: ', R_peak, ' predicted class : ', predictions[r]

        axarr[0].set_title('ECG RAW', fontsize=16)
        #axarr[0].grid(which='both', axis='both', linestyle='--')
        axarr[0].axvline(x = R_peak, color = 'k', linestyle='--')
        axarr[0].plot(x, y, color=colors[predictions[r]], linewidth=2.5, linestyle="-")
        begin =  R_peak


        # Preprocesed ECG data sampled at 360Hz
        R_peak_360 = int(qrs_detector.qrs_peaks_indices[r])
        x = range(begin_360, R_peak_360)
        y = qrs_detector.ecg_data[begin_360:R_peak_360]
        #print 'R peak: ', R_peak, ' predicted class : ', predictions[r]

        axarr[1].set_title('Filtered and normalized ECG 360Hz', fontsize=16)
        #axarr[0].grid(which='both', axis='both', linestyle='--')
        axarr[1].axvline(x = R_peak_360, color = 'k', linestyle='--')
        axarr[1].plot(x, y, color=colors[predictions[r]], linewidth=2.5, linestyle="-")
        begin_360 =  R_peak_360
       


        ## TODO: subplot 2 with procesed, norm and sampling signal at 360

    plt.show()

if __name__ == "__main__":

    if len(sys.argv) < 2:
	    print 'Error an argument is needed! \
               Example of use:\n\tpython example.py ../data/220.csv'
	    sys.exit()
    
    # TODO: update this function to load your own data
    ecg_signal, fs, min_A, max_A, n_bits = load_signal(sys.argv[1])


    #######################################################################
    # Detect R-peak points
    # if fs != 360 ecg_data will be resampled
    qrs_detector = QRSDetectorOffline(ecg_data_raw = ecg_signal, fs = fs,\
                                      verbose=False, plot_data=False,    \
                                      show_plot=False)

    # qrs_detector.ecg_data_raw             signal at original sampling frecuency
    # qrs_detector.ecg_data                 signal resampled at 360Hz
    # qrs_detector.qrs_peaks_indices        contains R peaks at 360 Hz sampling frequency
    # qrs_detector.qrs_peaks_indices_fs     contains R peaks at original sampling frequency



    #######################################################################
    # Classify Beats 
    qrs_classifier = QRSClassifier(svm_models_path = '../svm_models', \
                    ecg_data = qrs_detector.ecg_data, \
                    qrs_peaks_indices = qrs_detector.qrs_peaks_indices, \
                    min_A = min_A, max_A = max_A) 

    # qrs_detector.ecg_data                 signal resampled at 360Hz and filtered
    # qrs_classifier.predictions            class prediction for beat [0-4]
    #                                       Following AAMI-Recomendations: 
    #                                       N, SVEB, VEB, F, Q



    #######################################################################
    # Display results
    show_signal_and_predictions(qrs_detector, qrs_classifier)    
