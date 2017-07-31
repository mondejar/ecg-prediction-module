"""
    QRS_classifier.py
    
VARPA, University of Coruna
Mondejar Guerra, Victor M.
31 Jul 2017
"""
import operator
import matplotlib.pyplot as plt
import numpy as np

import sys
sys.path.insert(0, '../3rdparty/libsvm-3.22/python')
from svmutil import *

class QRSClassifier(object):
    """
    Python ECG prediction class. 
    Given the qrs location this code return a prediction for each beat.
    The classes used in this module follow the standard AAMI-Recomendations:
    
        Class 	N 	SVEB 	VEB 	F 	Q
        id 	0 	1 	2 	3 	4
        Class                                  id
        N: Normal                               0    
        SVEB: Supraventricular                  1
        VEB: Ventricular Ectopic Beat           2
        F: Fusion                               3
        Q: unknown beat                         4
    """

    def __init__(self, svm_models_path, ecg_data, qrs_peaks_indices, qrs_peaks_indices_fs, fs, min_A, max_A, show_results):
        """
        QRSClassifier class initialization method.
        :param list qrs_peaks_indices: Positions in x-axis of the qrs peak indeices
        """   
        self.ecg_data = ecg_data
        self.qrs_peaks_indices = qrs_peaks_indices
        self.qrs_peaks_indices_fs = qrs_peaks_indices_fs
        self.n_classes = 4
        # Adjust R-peak at maximum (ML-II)
        self.adjust_qrs_at_max()
        # Do preprocess?
        # median_filter1D
        # Remove Baseline
        # Remove High Freqs

	    # Normalization min_A - max_A to range [0-1]
        for i in range(0, len(self.ecg_data)):
            self.ecg_data[i] = (self.ecg_data[i] - min_A) / (max_A - min_A)

        # Compute RR-intervals
        self.compute_RR_intervals()
        
        # Compute HOS (High Order Statistics)
        
        # Compute my morphology descriptor
       
        # Load SVM models
        self.load_svm_models(svm_models_path)

        predictions = list()
        # Compute the feature and make the prediction
        for i in range(0, len(self.qrs_peaks_indices)):
            features = self.compute_features(i)
            predicted_class, max_votes = self.predict_beat_one_vs_on_SVM(features)            
            
            print 'R-peak: ', self.qrs_peaks_indices_fs[i], ' predicted class: ', predicted_class, ' votes: ', max_votes
            predictions.append(predicted_class)

        print predictions

        if show_results:
            plt.close('all')
            fig, axarr = plt.subplots(2, sharex=True, figsize=(15, 18))
            axarr[0].set_title('Signal', fontsize=10)
            axarr[0].grid(which='both', axis='both', linestyle='--')
            axarr[0].plot(self.ecg_data, color="salmon", zorder=1)
            axarr[0].scatter(x=qrs_peaks_indices, y=np.take(self.ecg_data, qrs_peaks_indices), c="black", s=50, zorder=2)

            #axarr[1].set_title('Original Fs', fontsize=10)
            #axarr[1].grid(which='both', axis='both', linestyle='--')
            #axarr[1].plot(self.ecg_data, color="salmon", zorder=1)

            # at original frecuency
            # qrs_peaks_indices_fs
        
            plt.show()
            plt.close()


    """
    Given a beat feature perform the prediction from the 6 binary SVM models
    and perform the majority voting to make the final prediction
    """
    def predict_beat_one_vs_on_SVM(self, features):
        index = 0
        output = [0] * 4
        label = list()
        label.append(1)
        for k in range(0, self.n_classes-1):
            for kk in range(k+1, self.n_classes):
                prediction, acc, val = svm_predict(label, [features], self.models[index])

                print 'prediction: ', prediction
                if prediction[0] == 1:
                    output[k] = output[k] + 1
                else:
                    output[kk] = output[kk] + 1
                   
                index = index + 1
        
        max_votes = 0
        predicted_class = 0
        for n in range(0, self.n_classes):
            if output[n] > max_votes:
                max_votes = output[n]
                predicted_class = n

        # TODO: in case of draw??
        return predicted_class, max_votes

    """
    Combine all the features used to make the descriptor
    """
    def compute_features(self, i):
        features = list()

        features.append(self.pre_R[i])
        features.append(self.post_R[i])
        features.append(self.local_R[i])
        features.append(self.global_R[i])

        # Standardiation z-score
        # TODO: load from file
        media = [276.2640, 276.2435, 273.9888, 272.5610]
        st_desviation = [75.4833, 74.1250, 60.6748, 57.1679]

        for f in range(0, 4):
            features[f] = (features[f] - media[f]) / st_desviation[f]

    	return features

    """
    Load the 6 one vs one SVM models 
    """
    def load_svm_models(self, svm_models_path):
        svm_model_name = 'svm_ovo_RR_'
        self.models = list()
        for i in range(0, 6):
            m = svm_load_model(svm_models_path + '/' + svm_model_name + str(i) + '.model')
            self.models.append(m)

    """
    Adjust R-peaks to their nearest maximum. The signal ML-II usually contains R-peaks at maximum 
    """
    def adjust_qrs_at_max(self):
        index = 0
        for detected_peak_index in (self.qrs_peaks_indices):
            l_w = max(detected_peak_index - 20, 0)
            r_w = min(detected_peak_index + 20, len(self.ecg_data))

            max_index, max_value = max(enumerate(self.ecg_data[l_w:r_w]), \
                                       key=operator.itemgetter(1))
            self.qrs_peaks_indices[index] = l_w + max_index
            index = index + 1


    def compute_RR_intervals(self):

        self.pre_R = np.array([], dtype=int)
        self.post_R = np.array([], dtype=int)
        self.local_R = np.array([], dtype=int)
        self.global_R = np.array([], dtype=int)

        # Pre_R and Post_R
        self.pre_R = np.append(self.pre_R, 0)
        self.post_R = np.append(self.post_R, self.qrs_peaks_indices[1] - self.qrs_peaks_indices[0])

        for i in range(1, len(self.qrs_peaks_indices)-1):
            self.pre_R = np.append(self.pre_R, self.qrs_peaks_indices[i] - self.qrs_peaks_indices[i-1])
            self.post_R = np.append(self.post_R, self.qrs_peaks_indices[i+1] - self.qrs_peaks_indices[i])

        self.pre_R[0] = self.pre_R[1]
        self.pre_R = np.append(self.pre_R, self.qrs_peaks_indices[-1] - self.qrs_peaks_indices[-2])  

        self.post_R = np.append(self.post_R, self.post_R[-1])

        # Local_R: AVG from last 10 pre_R values
        for i in range(0, len(self.qrs_peaks_indices)):
            num = 0
            avg_val = 0
            for j in range(-9, 0):
                if j+i >= 0:
                    avg_val = avg_val + self.pre_R[i+j]
                    num = num +1
            if num > 0:
                self.local_R = np.append(self.local_R, avg_val / float(num))
            else:
                self.local_R = np.append(self.local_R, 0.0)

	    # Global R AVG: from full past-signal
	    self.global_R = np.append(self.global_R, self.pre_R[0])
        for i in range(1, len(self.qrs_peaks_indices)):
            num = 0
            avg_val = 0
            for j in range( 0, i):
                avg_val = avg_val + self.pre_R[j]
            num = i
            self.global_R = np.append(self.global_R, avg_val / float(num))
