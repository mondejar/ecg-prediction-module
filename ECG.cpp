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


	// TODO: Preprocess before feature compute ? filtering?


	// Compute RR_intervals information from the full signal
	std::vector< std::vector<float> > RR_intervals;
	if(_use_RR_intervals)
		compute_RR_intervals(r_peaks, RR_intervals);

/*
	for(int i = 0; i < r_peaks.size(); i++)
	{
		//Check if the window can be set on that peak
		if(r_peaks[i] > _w_l && r_peaks[i] < _w_R)
		{
			beat = ecg.begin() + (r_peaks[i] - _w_l), ecg:begin() + (r_peaks[i] + _w_r)
			compute_feature(beat, RR_intervals, feature)

			// TODO: predict one by one or predict all features together? maybe is more efficient the second
			predict_beat(feature, prediction)

		}
	}
*/


	//TODO fprintf .csv file... R_pos, prediction

}

/*
Compute RR_intervals from the full signal:
	Pre_R: distance from actual R_pose to previous R_pose
	Post_R: distance from next R_pose to actual R_pose
	Local_R: average of 10 previous Pre_R values
	Global_R: 
*/
void ECG::compute_RR_intervals(std::vector<int> poses, std::vector<std::vector<float> > &RR_intervals)
{

/*
    % Compute RR interval features at patients level!
    if(compute_RR_interval_feature)
        pre_R = 0;
        post_R = R_poses{r}(2) - R_poses{r}(1);
        local_R = []; % Average of the ten past R intervals
        global_R = []; % Average of the last 5 minutes of the signal
        
        for(i=2:length(R_poses{r})-1)
            pre_R = [pre_R, R_poses{r}(i) - R_poses{r}(i-1)];
            post_R = [post_R, R_poses{r}(i+1) - R_poses{r}(i)];
        end
        pre_R(1) = pre_R(2);
        pre_R = [pre_R, R_poses{r}(length(R_poses{r})) - R_poses{r}(length(R_poses{r})-1)];
        
        post_R = [post_R, post_R(length(R_poses{r})-1)];
        
        % Local R: AVG from past 10 RR intervals
        for(i=1:length(R_poses{r}))
            window = i-10:i;
            valid_window = window > 0;
            window = window .* valid_window;
            window = window(window~=0);
            avg_val = sum(pre_R(window));
            avg_val = avg_val / (sum(valid_window));
            
            local_R = [local_R, avg_val];
        end
        
        % Global R: AVG from past 5 minutes
        % 360 Hz  5 minutes = 108000 samples;
        for(i=1:length(R_poses{r}))
            back = -1;
            back_length = 0;
            if(R_poses{r}(i) < 108000)
                window = 1:i;
            else
                while(i+back > 0 && back_length < 108000)
                    back_length =  R_poses{r}(i) - R_poses{r}(i+back);
                    back = back -1;
                end
                window = max(1,(back+i)):i;
            end
            % Considerando distancia maxima hacia atras
            avg_val = sum(pre_R(window));
            avg_val = avg_val / length(window);
            
            global_R = [global_R, avg_val];
        end
*/
}

/* Given a ecg-beat this function compute the features selected:
	- Morphology features:
		1 Raw signal
		2 Wavelets
		...
	- Intervals RR
*/
/*
void ECG::compute_feature(std::vector<float>beat, std::vector<float> &feature)
{
	//Compute feature from beat

	//TODO: complete...	if(_use_wavelets)
	//{
	//	feature.push_back()
	//}

	if(_use_RR_intervals)
	{
		float pre_R, post_R, local_R, global_R;
		
		//feature.push_back(pre_R);
		//feature.push_back(post_R);
		//feature.push_back(local_R);
		//feature.push_back(global_R);
	}
}


void ECG::predict_beat(feature, prediction)
{

	// LibSVM call	
	// svm_predict()
}
*/

