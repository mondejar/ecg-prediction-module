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
#include <iostream>
#include <stdlib.h>  
#include <vector>
#include "QRS_detection.h"

class ECG {

	public:
		//Constructor
		ECG(std::string svm_model_name, int w_l, int w_r, bool u_RR_i, bool u_w);

		void predict_ecg(std::vector<double> &ecg, float fs, float minA, float maxA,
                     float n_bits, std::vector<int> &r_peaks, std::vector<int> &predictions);
	private:

		int predict_beat_one_vs_one_SVM(svm_node* feature);
  
		void compute_RR_intervals(std::vector<int> poses, std::vector<double> &pre_R,
								  std::vector<double> &post_R, std::vector<double> &local_R,
								  std::vector<double> &global_R);  

							//(std::vector<double>beat, 
		svm_node *compute_feature(float pre_R, float post_R, float local_R, float global_R);

		// Variables
		std::vector<svm_model*> models;
		svm_model *model;

		int n_classes;
		std::string _svm_model_name;
		int _w_l, _w_r;
		std::vector<int> r_peaks;
		std::vector<double> ecg;
		bool _use_wavelets, _use_RR_intervals;
};

#endif
