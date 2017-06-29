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

		bool load_SVM_model();

		void predict_ecg(std::vector<float> ecg, float fs, float minA, float maxA,
                    float n_bits, std::vector<std::vector<int> > output, 
	                std::string output_file);
  
		void compute_RR_intervals(std::vector<int> poses, 
                                  std::vector<std::vector<float> > &RR_intervals);  


	private:

		std::string _svm_model_name;
		int _w_l, _w_r;
		std::vector<int> r_peaks;
		std::vector<float> ecg;
		bool _use_wavelets, _use_RR_intervals;
};

#endif

