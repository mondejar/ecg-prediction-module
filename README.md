# ECG prediction module

Given an ecg signal this module detects its R-peaks and return a prediction for each beat.

The classes used in this module follow the standard **AAMI-Recomendations**:
 
| Class |  N| SVEB| VEB | F | Q |
|--|--|--|--|--|-----|
|id| 0| 1|2|3|4|


# Compilation:
```
mkdir build
cd build
cmake ..
make
```

# Example of use:

```
example ../data/220.csv
```
This will create a file *filename.out* that contains:

    r_peak_position_1, class_prediction_1
    ...
    r_peak_position_n, class_prediction_n
    
The input *.csv* file must follow the structure:
```
frequency of signal\n
minimum Amplitude\n
maximum Amplitude\n
number of bits\n
signal[0]\n
signal[1]\n
..
signal[N]\n
```

# Requirements:

## LibSVM
To use the trained SVM models and predict the result for new incoming data

Download from: https://www.csie.ntu.edu.tw/~cjlin/libsvm/

Installation:
Simply extract the *.zip* folder on the same dir.

## Boost libraries
If the input signal has a different frequency sampling of 360 the *resample* function that uses the Boost libraries is needed.



Download the folder from the website http://www.boost.org/ and extract the folder. Then
on that directory execute:

```
./bootstrap.sh
```

and finally:
```
sudo ./b2 install 
```

# Files

### example.cpp
_________________
Test sample for calling prediction functions from *ECG.cpp*.
The signal is loaded from a *.csv file* recived from command line arguments.

The output is stored in a text file with the following structure:


    r_peak_position_1, class_prediction_1
    ...
    r_peak_position_n, class_prediction_n

### ECG.cpp
_________________
This file contains the function for compute the features for inconming signals an return an answear for each beat. 
NOTE: The signals with different frecuency sampling than 360 will be resampled and the analyzed.

Just created and object of the class *ecg* and call the method *predict_ecg*:

```cpp
#include "ecg.h"

// Read ECG signal 

...
// Run classifier 
ECG* ecg_classifier = new ECG();
ecg_classifier.predict_ecg(ecg, fs, minA, maxA, n_bits, r_peaks, predictions);
delete ecg_classifier;
```

ecg.h
```cpp
...
//Constructor
ECG();

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
    //r_peaks: a vector that contains the values of each R peak detection
    //predictions: a vector that containst the state of each beat (N, SVEB, VEB, F, Q) [0-4]
void predict_ecg(std::vector<double> &ecg, float fs, float minA, float maxA, float n_bits, std::vector<int> &r_peaks, std::vector<int> &predictions);

```


### QRS_detection.cpp
_________________
Function for QRS detection of each beat from a full ECG signal. 

NOTE: This function will be updated!
