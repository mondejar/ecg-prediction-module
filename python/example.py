from QRS_detector import QRSDetectorOffline
import csv

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

    # TODO: update this function to load your own data
    ecg_signal, fs, min_A, max_A, n_bits = load_signal('../data/220.csv')

    # Detect R-peak points
    qrs_detector = QRSDetectorOffline(ecg_data_raw = ecg_signal, fs = fs, verbose=False, plot_data=False, show_plot=False)
    #qrs_detector.qrs_peaks_indices:
    
    # Classify Beat 
