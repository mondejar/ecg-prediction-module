/*
* ECG.cpp
* 
* The descriptors created for each beat prediction is composed of:
* 1. Intervals RR: 
* 		-Pre R
*		-Post R
*		-Local R
*		-Global R
* 2. Morphology:
*
* 3. ...
*
* VARPA, University of Coruña
* Mondejar Guerra, Victor M.
* 29 Jun 2017
*/

#include "ECG.h"
#include <numeric>

// Constructor
ECG::ECG (std::string svm_models_path)
{
	n_classes = 4;

	_w_l = 90;// Window Left of the beat centered on R-peak for compute morphology descriptor
	_w_r = 90;// Window Right of the beat centered on R-peak for compute morphology descriptor
	_svm_models_path = svm_models_path; // full-path to svm model 

	_use_RR_intervals = true;
	_use_wavelets = false;

	_svm_models_name = "svm_ovo_";

	if(_use_RR_intervals)
		_svm_models_name += "RR_";

	if(_use_wavelets)
		_svm_models_name += "wvlt_";
		 
	// Load SVM models one-vs-one 
	for(int m = 0; m < 6; m++){
		std::cout<< " model name "<< (_svm_models_path + "/" + _svm_models_name + std::to_string(m) + ".model"  ) << std::endl;
		models.push_back(svm_load_model( (_svm_models_path + "/" +  _svm_models_name + std::to_string(m) + ".model"  ).c_str()));
	}

	// Initialize butterworth filter for preprocessing

    butterwoth_filter = new Dsp::SmoothedFilterDesign<Dsp::Butterworth::Design::LowPass <12>, 1, Dsp::DirectFormII> (32);
    Dsp::Params params;
    params[0] = 360; // sample rate
    params[1] = 12; // order
    params[2] = 35; // center frequency
    butterwoth_filter->setParams (params);

}



void median_filter1D(std::vector<double> &in, std::vector<double> &out, int N)
{
	out.resize(in.size());

	// Center window -N/2, N/2 on each element
	// NOTE: for borders treatment use only the information available
	
	// i < N/2
	int n_half = std::round(N/2.0);
	double sum = 0;
	int num_elems;
	for(int i = 0; i < n_half; i++)
	{
		sum = std::accumulate(in.begin(), in.begin() + (i + n_half), 0.0);
		num_elems = (i + n_half);
		out[i] = sum / num_elems;
	} 

	// i > N/2 && i < in.size - N/2
	for(int i = n_half; i < in.size() - n_half; i++)
	{
		sum = std::accumulate(in.begin() + (i - n_half), in.begin() + (i + n_half), 0.0);
		out[i] = sum / N;
	}

	// i > in.size - N/2

	for(int i = in.size() - n_half; i < in.size(); i++)
	{
		sum = std::accumulate(in.begin() + (i - n_half), in.begin() + i  + ( in.size() - i -1), 0.0);
		num_elems = (i  + ( in.size() - i -1)) - (i - n_half);
		out[i] = sum / num_elems;
	} 
}



void ECG::center_RR(std::vector<double> &ecg, std::vector<int> &r_peaks)
{
	int w = 30;
	int pos = 0;
	int new_pos = 0;
	for(int r = 0; r < r_peaks.size(); r++)
	{
		pos = r_peaks[r];
		if(pos > w && pos < (ecg.size() - w))
		{
			new_pos = std::distance(ecg.begin(), std::max_element(ecg.begin() + (pos -w), ecg.begin() + (pos + w)));
			std::cout<< "R_peak["<< r << "] = "<< r_peaks[r] << " new = "<< new_pos << " value = "<< ecg[new_pos] << std::endl;

			r_peaks[r] = new_pos;
		}
	}
}


void ECG::predict_ecg(std::vector<double> ecg, float fs, float minA, float maxA,
			          float n_bits, std::vector<int> &r_peaks, std::vector<int> &predictions)
{
	bool do_preprocess = true;
	bool center_RR_max = true;

	if(fs != 360)// Re-sample to 360Hz
	{
		std::cout<< "Resampling to "<< 360 << " Hz..." << std::endl;
		std::vector<double> output;

		resample(360, fs, ecg, output);
		ecg = output;
	}


	//////// PREPROCESS
    // Each signal was processed with a median filter of 200-ms width to remove QRS complexes and P-waves.
    // 200ms at 360Hz = 72

	if(do_preprocess)
	{	
		std::vector<double> baseline(ecg.size());
		std::vector<double> baseline2(ecg.size());

		median_filter1D(ecg, baseline, 72);

		// The resulting signal was then processed with a median filter of 600 ms width to remove T-waves.
		// 600ms at 360Hz = 216
		median_filter1D(baseline, baseline2, 216);

		// Remove baseline
		for(int i = 0; i < ecg.size(); i++)
			ecg[i] -= baseline2[i];	


		// Remove high frecuencies butterwoth filter
		// Low pass butter
		// Unwanted power-line and high-frequency noise was removed from the baseline corrected ECG with a 12-tap low-pass filter.
		// Will be the filtered signal The filter was a finite impulse response filter with equal ripple in the pass and stop bands.
		// The 3-dB point of the filter was 35 Hz.

		double* data[1];
		data[0] = ecg.data();
			
		// NOTE: this step may shift the signal x-pose 
		butterwoth_filter->process(ecg.size(), data);

		for(int i = 0; i < ecg.size(); i++)
			ecg[i] = data[0][i];
	}

	// First preprocessing because filter can shift off the signal x-asis
	// TODO: Center R-peak on the maximum value
	detect_QRS(ecg, r_peaks);

	if(center_RR_max)
		center_RR(ecg, r_peaks);

	// Normalization min_A - max_A to range [0-1]
	for(int i = 0; i < ecg.size(); i++)
		ecg[i] = (ecg[i] - minA) / (maxA - minA);

	// Compute RR_intervals information from the full signal
	std::vector<double> pre_R, post_R, local_R, global_R;
	if(_use_RR_intervals)
		compute_RR_intervals(r_peaks, pre_R, post_R, local_R, global_R);

	// TODO: compute High Order Statistics and my morphology descriptor

	int prediction; 
	for(int i = 0; i < r_peaks.size(); i++)
	{
		//Check if the window can be set on that peak
		if(r_peaks[i] > _w_l && r_peaks[i] < ecg.size() - _w_r)
		{
			prediction = 0;
			//beat = ecg.begin() + (r_peaks[i] - _w_l), ecg:begin() + (r_peaks[i] + _w_r)
			//std::vector<double> feature;
			svm_node *features;

			//beat
			features = compute_feature(pre_R[i], post_R[i], local_R[i], global_R[i]);

			// TODO: predict one by one or predict all beats together?
			//       maybe is more efficient the second
			prediction = predict_beat_one_vs_one_SVM(features);
			predictions.push_back(prediction);
			free(features);
		}
	}

	// Resample R-peak points to original freq
	if(fs != 360)
	{
		std::cout<< "Resampling R-peaks positions to "<< fs << " Hz..." << std::endl;
		for(int i = 0; i < r_peaks.size(); i++)
		{
			r_peaks[i] = r_peaks[i] / (360 / fs);
		}
	}

}




void ECG::compute_RR_intervals(std::vector<int> R_poses, std::vector<double> &pre_R,
							   std::vector<double> &post_R, std::vector<double> &local_R, 
						       std::vector<double> &global_R)
{
	// Pre_R and Post_R 
	pre_R.push_back(0);
	post_R.push_back(R_poses[1] - R_poses[0]);

	for(int i= 1; i < R_poses.size() -1; i++)
	{
		pre_R.push_back(R_poses[i] - R_poses[i-1]);    
        post_R.push_back(R_poses[i+1] - R_poses[i]);
	}    


	pre_R[0] = pre_R[1];
    pre_R.push_back(R_poses[R_poses.size()-1] - R_poses[R_poses.size()-2]);// end

    post_R.push_back(post_R[post_R.size()-1]);

    // Local_R: AVG from past 10 pre_R 
	int num;
	float avg_val;
	for(int i = 0; i < R_poses.size(); i++)
	{
		num = 0;
		avg_val = 0;
		for(int j = -9; j <= 0; j++)
		{
			if(j+i >= 0)
			{
				avg_val += pre_R[i+j];
				num++;
			}
		}
		local_R.push_back( avg_val / (float)num );
	}
   

	// Global R AVG: from full past-signal
	global_R.push_back(pre_R[0]);
	for(int i = 1; i < R_poses.size(); i++)
	{
		num = 0;
		avg_val = 0;
		for(int j = 0; j < i; j++)
			avg_val += pre_R[j];	

		num = i;

		global_R.push_back(avg_val / (float) num);
	}
}

/* Given a ecg-beat this function compute the features selected:
	- Morphology features:
		1 Raw signal
		2 Wavelets
		...
	- Intervals RR
*/

//std::vector<double>beat, 
svm_node* ECG::compute_feature(float pre_R, float post_R, float local_R, float global_R)
{
	//Compute feature from beat
	//TODO: complete...	if(_use_wavelets)
	//{
	//	feature.push_back()
	//}

	//TODO: this value depends on the number of features used!
	svm_node *features;
	features = new svm_node[4];

	//struct svm_node *x = (struct svm_node *) malloc((4)*sizeof(struct svm_node));
	if(_use_RR_intervals)
	{
		features[0].index = 0; 
		features[0].value = pre_R;
		features[1].index = 1; 
		features[1].value = post_R;
		features[2].index = 2; 
		features[2].value = local_R;
		features[3].index = 3; 
		features[3].value = global_R;
	}

	//Standardization
	//TODO load data
	double media[4] = {276.2640, 276.2435, 273.9888, 272.5610};
	double st_desviation[4] = {75.4833, 74.1250, 60.6748, 57.1679};

	for(int i = 0; i < 4; i++)
		features[i].value  = (features[i].value - media[i] ) / st_desviation[i];

	return features;
}

int ECG::predict_beat_one_vs_one_SVM(svm_node *features)
{
	double prediction;
	int predicted_class;
	double output[n_classes];
	int index = 0;

	for(int i = 0; i < n_classes; i++)
		output[i] = 0;

	//TODO: param nr_class for C-SVM
	for(int k = 0; k < n_classes; k++)
	{
		for(int kk = k+1; kk < n_classes; kk++)
		{
			//Toy Test prediction
			prediction = svm_predict(models[index], features);
			if(prediction == 1)
				output[k]++;
			else
				output[kk]++;

			index++;
		}
	}

	int max = 0;
	for(int i = 0; i < n_classes; i++)
	{
		if(output[i] > max)
			max = output[i];
	}

	std::vector<int> best_classes;
	for(int i = 0; i < n_classes; i++)
	{
		if(output[i] == max)
		{
			best_classes.push_back(i);
		}
	}	

	predicted_class = best_classes[0];

	if(best_classes.size() > 1)
	{
		if(best_classes[1] > best_classes[0])
			predicted_class = best_classes[1];
	}

	return predicted_class;
}
