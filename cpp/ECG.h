/*
* ECG.h
* 
* VARPA, University of Coruña
* Mondejar Guerra, Victor M.
* 29 Jun 2017
*/

// ECG.h
#ifndef ECG_H
#define ECG_H

#include "libsvm-3.22/svm.h"
#include "resample/resample.h"
#include <fstream>
#include <string>
#include <exception>
#include "QRS_detection.h"
#include "DSPFilters/include/DspFilters/Dsp.h"

class ECG {

	public:
		//Constructor
		//Params:
			//svm_model_path: full path to the folder which contains the trained svm models
		ECG(std::string svm_model_path);

		//Description: Function for predict the state of each beat given an ECG signal
		//Params:
			//Input:
			//ecg: contains the ECG signal of MLII (if fs!=360, ecg will be resampled)
			//fs: frecuency sampling of the signals
			//minA: minimum Amplitude value
			//maxA: maximum Amplitude value
			//n_bits: number of bits used for represent the signal
			//
			//Output:
    		//r_peaks: a vector that contains the values of each R peak detection at original frequency fs
			//predictions: a vector that containst the state of each beat (N, SVEB, VEB, F, Q) [0-4]
		void predict_ecg(std::vector<double> ecg, float fs, float minA, float maxA, float n_bits, std::vector<int> &r_peaks, std::vector<int> &predictions);

	private:

		/* Given a feature representing one beat this method returns 
		the decision for one-vs-one SVM trained models
		*/
		int predict_beat_one_vs_one_SVM(svm_node* feature);

		/*
		Compute RR_intervals from the full signal:
			Pre_R: distance from actual R_pose to previous R_pose
			Post_R: distance from next R_pose to actual R_pose
			Local_R: average of 10 previous Pre_R values
			Global_R: 
		*/  
		void compute_RR_intervals(std::vector<int> poses, std::vector<double> &pre_R,
								  std::vector<double> &post_R, std::vector<double> &local_R,
								  std::vector<double> &global_R);  


							//(std::vector<double>beat, 
		svm_node *compute_feature(float pre_R, float post_R, float local_R, float global_R);

		void center_RR(std::vector<double> &ecg, std::vector<int> &r_peaks);


		// Variables
		std::vector<svm_model*> models;
		svm_model *model;

		int n_classes;
		std::string _svm_models_path, _svm_models_name;
		int _w_l, _w_r;
		std::vector<int> r_peaks;
		std::vector<double> ecg;
		bool _use_wavelets, _use_RR_intervals;
		Dsp::Filter* butterwoth_filter;
};

#endif
