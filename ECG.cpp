/*
* ECG.cpp
* 
* VARPA, University of Coruña
* Mondejar Guerra, Victor M.
* 29 Jun 2017
*/

#include "ECG.h"

// Constructor
ECG::ECG (std::string svm_model_name, int w_l, int w_r, bool u_RR_i, bool u_w)
{
	_w_l = w_l;
	_w_r = w_r;
	_svm_model_name = svm_model_name;

	_use_RR_intervals = u_RR_i;
	_use_wavelets = u_w;

	load_SVM_model();
}


bool ECG::load_SVM_model()
{
	//_svm_model_name
	return true;
}

/* 
Given an ecg signal detect the R-pose of each beat and return a prediction
following the standard AAMI-recomendations (N, SVEB, VEB, F, Q) [0-4]

Export the result to a file .csv
R_pos (x-axis from signal), class [0-4]
output:
	output[r_peak_pos, class_output]
*/
void ECG::predict_ecg(std::vector<float> ecg, float fs, float minA, float maxA,
			          float n_bits, std::vector<std::vector<int> > output, 
                      std::string output_file)
{
	// TODO: Preprocess for QRS detection? Band-filtering?
	std::vector<int> r_peaks;
	detect_QRS(ecg, r_peaks);

	// Normalization min_A - max_A to range [0-1]
	for(int i = 0; i < ecg.size(); i++)
		ecg[i] = (ecg[i] - minA) / (maxA - minA);


	// TODO: Re-sample to 360Hz
	//if(fs != 360)
	//	resample_freq(&ecg, fs, 360);

	// TODO: Preprocess before feature compute ? filtering?


	// Compute RR_intervals information from the full signal
	std::vector<float> pre_R, post_R, local_R, global_R;
	if(_use_RR_intervals)
		compute_RR_intervals(r_peaks, pre_R, post_R, local_R, global_R);


	/* Display
	for(int i = 0; i < r_peaks.size(); i++)
	{
		std::cout << " Beat: "<< i << std::endl;

		std::cout<< "posR = "<< r_peaks[i] << " preR["<< i <<"] = "<< pre_R[i]<< " postR["<< i <<"] = "<< post_R[i] << " localR["<< i <<"] = "<< local_R[i] << " globalR["<< i <<"] = "<< global_R[i]<< std::endl;

		
	}*/

	for(int i = 0; i < r_peaks.size(); i++)
	{
		//Check if the window can be set on that peak
		if(r_peaks[i] > _w_l && r_peaks[i] < _w_R)
		{
			//beat = ecg.begin() + (r_peaks[i] - _w_l), ecg:begin() + (r_peaks[i] + _w_r)
			std::vector<float> feature;
							//beat
			compute_feature(pre_R[i], post_R[i], local_R[i], global_R[i], feature)


			// TODO: predict one by one or predict all features together? maybe is more efficient the second
			predict_beat(feature, prediction)

		}
	}


	//TODO fprintf .csv file... R_pos, prediction

}

// Resample ecg signal from fs_orig to fs
void ECG::resample_freq(std::vector<float> &ecg, float fs_orig, float fs)
{
	// TODO complete...
}


/*
Compute RR_intervals from the full signal:
	Pre_R: distance from actual R_pose to previous R_pose
	Post_R: distance from next R_pose to actual R_pose
	Local_R: average of 10 previous Pre_R values
	Global_R: 
*/
void ECG::compute_RR_intervals(std::vector<int> R_poses, std::vector<float> &pre_R, std::vector<float> &post_R, std::vector<float> &local_R, std::vector<float> &global_R)
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


//std::vector<float>beat, 
void ECG::compute_feature(float pre_R, float post_R, float local_R, float global_R, std::vector<float> &feature)
{
	//Compute feature from beat

	//TODO: complete...	if(_use_wavelets)
	//{
	//	feature.push_back()
	//}

	if(_use_RR_intervals)
	{
		feature.push_back(pre_R);
		feature.push_back(post_R);
		feature.push_back(local_R);
		feature.push_back(global_R);
	}
}


void ECG::predict_beat(feature, prediction)
{
	// LibSVM call	
	// svm_predict()


}


