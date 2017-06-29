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
main ../data/220.csv
```
This will create a file *filename.out* that contains:

    r_peak_position_1, class_prediction_1
    ...
    r_peak_position_n, class_prediction_n
    

and also print at console.

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

Download from:
....

Installation:
Simply extract the *.zip* folder on the same dir.


# Files

### main.cpp
_________________
Test sample for calling prediction functions from *ECG.cpp*.
The signal is loaded from a *.csv file* recived from command line arguments.


### ECG.cpp
_________________


functions... example of calls...


input: (float* signal, int Hz, int max_A, int min_A, int num_bits)

Steps:

1. clean signal 	   

2. pam_tomkins: Detect R peaks 
(momentaneamente exported code from Matlab to C++)
(TODO: perform the conversion)

3. load libSVM

( From matlab o exportar las matrices de entrenamiento de matlab y entrenar directamente en C++... en la etapa final entrenar con todos los datos!)


Output:

int[][]: posX, class


### QRS_detection.cpp
_________________
Function for QRS detection. 

NOTE: This function will be updated!
