# ECG prediction module

Given an ML-II (derivation II) ECG signal, this module detects its R-peaks and return a prediction for each beat.

The classes used in this module follow the standard **AAMI-Recomendations**:
 
| Class |  N| SVEB| VEB | F | Q |
|--|--|--|--|--|-----|
|id| 0| 1|2|3|4|

**N**: Normal

**SVEB**: Supraventricular

**VEB**: Ventricular Ectopic Beat 

**F**: Fusion

**Q**: unknown beat

# Python implementation

## Compilation:

## Example of use:

## Requirements:

*NOTE: these requirements are only informative since the files required for these libraries have been already included in the project*

### LibSVM
To use the trained SVM models and predict the result for new incoming data

Download from: https://www.csie.ntu.edu.tw/~cjlin/libsvm/

Installation:
Type 'make clean' and then 'make' on dir: 3rdparty/libsvm-3.22
and then type 'make' on dir 3rdparty/libsvm-3.22/python to generate the file 'libsvm.so.2'

### Python libraries

#### matplotlib

#### scipy

## Files

### .py

# C++ implementation
