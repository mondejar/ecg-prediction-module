#!/usr/bin/env python

"""
QRS_classifier.py
    
VARPA, University of Coruna
Mondejar Guerra, Victor M.

Created:            31 Jul 2017
Last modification:  15 Nov 2017
"""
import operator
import numpy as np
import sys
sys.path.insert(0, '../3rdparty/libsvm-3.22/python')
from svmutil import *
from scipy.signal import medfilt
import scipy.stats
import matplotlib.pyplot as plt
import csv

class ClassifierError(Exception):
    pass

class QRSClassifier(object):

    """
    Python ECG prediction class. 
    Given the qrs location this code return a prediction for each beat.
    The classes used in this module follow the standard AAMI-Recomendations:
    
        Class 	N 	SVEB 	VEB 	F 	Q
        id 	    0 	1 	      2 	3 	4
        Class                                  id
        N: Normal                               0    
        SVEB: Supraventricular                  1
        VEB: Ventricular Ectopic Beat           2
        F: Fusion                               3
        Q: unknown beat                         4
    """
    def __init__(self, svm_models_path, ecg_data, qrs_peaks_indices, min_A, max_A, verbose):
        """
        QRSClassifier class initialization method.
        :param string svm_models_path: path of the directory that contains the trained svm models
        :param numpy.array ecg_data: the lead II data
        :param list qrs_peaks_indices: Positions in x-axis of the qrs peak indeices
        :param int min_A: minimum amplitude value of the lead
        :param int max_A: maximum amplitude value of the lead
        :param bool verbose: flag for printing the results
        """   
        self.ecg_data = ecg_data
        self.qrs_peaks_indices = qrs_peaks_indices
        self.n_classes = 4

        # Size of the window surrounding each beat
        self.window_l = 90
        self.window_r = 90

        predictions = np.zeros(len(qrs_peaks_indices), dtype=int)
        do_preprocess = True

        # Adjust R-peak at maximum (ML-II)
        self.adjust_qrs_at_max()

        svm_options = ''
        if verbose == False:
            svm_options += '-q'

        if do_preprocess:
            # median_filter1D
            baseline = medfilt(self.ecg_data, 71) #scipy.signal
            baseline2 = medfilt(baseline, 215) #scipy.signal

            # Remove Baseline
            for i in range(0, len(self.ecg_data)):
                self.ecg_data[i] = self.ecg_data[i] - baseline2[i]

            # TODO Remove High Freqs?

	    # Normalization min_A - max_A to range [0-1]
        for i in range(0, len(self.ecg_data)):
            self.ecg_data[i] = (self.ecg_data[i] - min_A) / (max_A - min_A)

        if len(qrs_peaks_indices) > 2:
            # Compute RR-intervals descriptor of each beat
            features_RR = self.compute_RR_intervals()
            
            # Compute the morphology descriptor of each beat
            features_Morph = self.compute_Morphology()
                
            # Perform Z-score with the mean and std from training data      
            features_RR = self.norm_z_score(features_RR, svm_models_path, '/RR_z_score.csv')
            features_Morph = self.norm_z_score(features_Morph, svm_models_path, '/HOS_z_score.csv')

            # Load One-Against-One SVM models
            svm_model_RR = svm_load_model(svm_models_path + '/svm_ovo_RR.model')
            svm_model_Morph = svm_load_model(svm_models_path + '/svm_ovo_HOS.model')
            # Compute the feature and make the prediction

            for i in range(0, len(self.qrs_peaks_indices)):
                # predict RR
                predicted_class_RR, acc, probs_RR = svm_predict([0], [features_RR[i]], svm_model_RR, svm_options)
                vote_class_RR = self.ovo_compute_prob_posteriori(probs_RR)
                # predict HOS
                predicted_class_Morph, acc, probs_Morph = svm_predict([0], [features_Morph[i]], svm_model_Morph, svm_options)
                vote_class_Morph = self.ovo_compute_prob_posteriori(probs_Morph)
                
                # fuse predictions
                vote_fused = self.fuse_product_rule(vote_class_RR, vote_class_Morph)

                # Get the max class
                predicted_class = np.argmax(vote_fused)

                predictions[i] = predicted_class
            
        else:
            error_str = "Warning: at least three QRS beats must be detected on ECG signal to perform the classification and only " +\
                str(len(qrs_peaks_indices)) + " was detected.\nCheck the segmentation values in QRS_detector.py"
            raise ClassifierError(error_str)
       
        self.predictions = predictions


    def fuse_product_rule(self, p1, p2):
        return np.multiply(p1, p2)

    " Compute the prob posteriori and accumulate over the corresponding class"
    def ovo_compute_prob_posteriori(self, probs):
        C1 = [0, 0, 0, 1, 1, 2]
        C2 = [1, 2, 3, 2, 3, 3]

        vote_class = [0] * self.n_classes
        for j in range(0, 6):
            vote_class[C1[j]] = vote_class[C1[j]] + (1 / (1 + np.exp(-probs[0][j])))
            vote_class[C2[j]] = vote_class[C2[j]] + (1 / (1 + np.exp(probs[0][j])))
        return vote_class


    """
    Z score normalization:
    Input:
        -features: input data
        -dir_path: to search file
        -z_score_file: file name with mean and std data
    Ouput:
        -features: z score normalized
    """
    def norm_z_score(self, features, dir_path, z_score_file):

        mean_z, std_z = self.load_z_score_data(dir_path, z_score_file)

        for f in range(0, len(features)):
            for i in range(0, len(features[f])):
                features[f][i] = (features[f][i] - mean_z[i]) / std_z[i]

    	return features

    """ Load mean and std of z-score training data 
        for each dimension of the features"
    """
    def load_z_score_data(self, dir_path, name):
        
        mean_z = []
        std_z = []

        with open(dir_path+ name, 'rb') as f:
            reader = csv.reader(f)
            for row in reader:
                mean_z.append(float(row[0]))
                std_z.append(float(row[1]))

        return mean_z, std_z

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


    # Compute features RR interval for each  beat
    def compute_RR_intervals(self):
        features_RR = list()

        pre_R = np.array([], dtype=int)
        post_R = np.array([], dtype=int)
        local_R = np.array([], dtype=int)
        global_R = np.array([], dtype=int)

        # Pre_R and Post_R
        pre_R = np.append(pre_R, 0)
        post_R = np.append(post_R, self.qrs_peaks_indices[1] - self.qrs_peaks_indices[0])

        for i in range(1, len(self.qrs_peaks_indices)-1):
            pre_R = np.append(pre_R, self.qrs_peaks_indices[i] - self.qrs_peaks_indices[i-1])
            post_R = np.append(post_R, self.qrs_peaks_indices[i+1] - self.qrs_peaks_indices[i])

        pre_R[0] = pre_R[1]
        pre_R = np.append(pre_R, self.qrs_peaks_indices[-1] - self.qrs_peaks_indices[-2])  

        post_R = np.append(post_R, post_R[-1])

        # Local_R: AVG from last 10 pre_R values
        for i in range(0, len(self.qrs_peaks_indices)):
            num = 0
            avg_val = 0
            for j in range(-9, 0):
                if j+i >= 0:
                    avg_val = avg_val + pre_R[i+j]
                    num = num +1
            if num > 0:
                local_R = np.append(local_R, avg_val / float(num))
            else:
                local_R = np.append(local_R, 0.0)

	    # Global R AVG: from full past-signal
	    global_R = np.append(global_R, pre_R[0])
        for i in range(1, len(self.qrs_peaks_indices)):
            num = 0
            avg_val = 0
            for j in range( 0, i):
                avg_val = avg_val + pre_R[j]
            num = i
            global_R = np.append(global_R, avg_val / float(num))

        for i in range(0, len(self.qrs_peaks_indices)):
            features_RR.append([pre_R[i], post_R[i], local_R[i], global_R[i]])
            
        return features_RR

    # Compute morphology of the beats
    def compute_Morphology(self):
        features_Morph = list()

        n_intervals = 6
        lag = int(round( (self.window_l + self.window_r )/ n_intervals))
    
        #self.display_signal(self.ecg_data)
        for i in range(0, len(self.qrs_peaks_indices)):
            if self.qrs_peaks_indices[i] > self.window_l and self.qrs_peaks_indices[i] < (len(self.ecg_data) - self.window_r):
                beat = self.ecg_data[self.qrs_peaks_indices[i]-self.window_l:self.qrs_peaks_indices[i] + self.window_r]
                #self.display_signal(beat)
                
                # Compute HOS (Hihg Order Statistics)
                hos_skw, host_kur = self.compute_HOS(beat, n_intervals, lag)

                features_Morph.append(hos_skw + host_kur)
            else:
                features_Morph.append([0] * (n_intervals * 2))        

        return features_Morph

    # Compute HOS (Hihg Order Statistics)
    # Input param:  
    # -n: number of intervals in which the beat is divided
    # -lag: the length of each interval
    # -beat: the hearth beat 
    def compute_HOS(self, beat, n, lag):

        hos_skewness = [0] * 6
        hos_kurtosis = [0] * 6
        for i in range(0, n):
            pose = (lag * i)
            interval = beat[pose:(pose + lag - 1)]
            # Skewness  
            hos_skewness[i] = scipy.stats.skew(interval, 0, True)
            # Kurtosis
            hos_kurtosis[i] = scipy.stats.kurtosis(interval, 0, False, True)

        return hos_skewness, hos_kurtosis


    
    def display_signal(self, beat):
        plt.plot(beat)
        plt.ylabel('Signal')
        plt.show()
        

