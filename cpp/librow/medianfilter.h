//   medianfilter.h - declarations for 
//   1D and 2D median filter routines
//
//   The code is property of LIBROW
//   You can use it on your own
//   When utilizing credit LIBROW site

#ifndef _MEDIANFILTER_H_
#define _MEDIANFILTER_H_

//   Signal/image element type
typedef double element;

//   1D MEDIAN FILTER, window size 5
//     signal - input signal
//     result - output signal, NULL for inplace processing
//     N      - length of the signal
void medianfilter(element* signal, element* result, int N);

//   2D MEDIAN FILTER, window size 3x3
//     image  - input image
//     result - output image, NULL for inplace processing
//     N      - width of the image
//     M      - height of the image
void medianfilter(element* image, element* result, int N, int M);

#endif
