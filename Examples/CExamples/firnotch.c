// SigLib FIR Single Conjugate Zero Notch Filter Example
// The conjugate zeros will be located on the unit circle at the specified frequency
// Copyright (c) 2023 Delta Numerix All rights reserved.

// Include files
#include <stdio.h>
#include <siglib.h>                                                 // SigLib DSP library
#include <gnuplot_c.h>                                              // Gnuplot/C

// Define constants
#define FILTER_LENGTH           3
#define SAMPLE_RATE_HZ          16000.
#define NOTCH_FREQUENCY         2000.

#define IMPULSE_RESPONSE_LENGTH 1024

#define FFT_LENGTH              IMPULSE_RESPONSE_LENGTH
#define LOG2_FFT_LENGTH         SAI_FftLengthLog2(FFT_LENGTH)       // Log2 FFT length,

#define PLOT_LENGTH             (IMPULSE_RESPONSE_LENGTH/2)

// Declare global variables and arrays
static SLData_t pFilterTaps[FILTER_LENGTH];
static SLData_t pFilterState[FILTER_LENGTH];


int main (
  void)
{
  h_GPC_Plot     *h2DPlot;                                          // Plot object

  SLArrayIndex_t  FilterIndex;

// Allocate memory
  SLData_t       *pRealData = SUF_VectorArrayAllocate (FFT_LENGTH);
  SLData_t       *pImagData = SUF_VectorArrayAllocate (FFT_LENGTH);
  SLData_t       *pResults = SUF_VectorArrayAllocate (PLOT_LENGTH);
  SLData_t       *pSrc = SUF_VectorArrayAllocate (IMPULSE_RESPONSE_LENGTH);
  SLData_t       *pFFTCoeffs = SUF_FftCoefficientAllocate (FFT_LENGTH);

  if ((NULL == pRealData) || (NULL == pImagData) || (NULL == pResults) || (NULL == pSrc) || (NULL == pFFTCoeffs)) {

    printf ("\n\nMemory allocation failed\n\n");
    exit (0);
  }

// Generate the notch filter coefficients
  SIF_FirZeroNotchFilter (pFilterTaps,                              // Coefficients array
                          NOTCH_FREQUENCY / SAMPLE_RATE_HZ);        // Notch centre frequency normalized to Fs = 1 Hz


// Generate impulse response
  h2DPlot =                                                         // Initialize plot
    gpc_init_2d ("FIR Notch Filter",                                // Plot title
                 "Time / Frequency",                                // X-Axis label
                 "Magnitude",                                       // Y-Axis label
                 GPC_AUTO_SCALE,                                    // Scaling mode
                 GPC_SIGNED,                                        // Sign mode
                 GPC_KEY_DISABLE);                                  // Legend / key mode
  if (NULL == h2DPlot) {
    printf ("\nPlot creation failure.\n");
    exit (-1);
  }

  SIF_Fir (pFilterState,                                            // Pointer to filter state array
           &FilterIndex,                                            // Pointer to filter index register
           FILTER_LENGTH);                                          // Filter length
// Initialise FFT
  SIF_Fft (pFFTCoeffs,                                              // Pointer to FFT coefficients
           SIGLIB_BIT_REV_STANDARD,                                 // Bit reverse mode flag / Pointer to bit reverse address table
           FFT_LENGTH);                                             // FFT length

// Generate test impulse
  SDA_SignalGenerate (pSrc,                                         // Pointer to destination array
                      SIGLIB_IMPULSE,                               // Signal type - Impulse function
                      SIGLIB_ONE,                                   // Signal peak level
                      SIGLIB_FILL,                                  // Fill (overwrite) or add to existing array contents
                      SIGLIB_ZERO,                                  // Signal frequency - Unused
                      SIGLIB_ZERO,                                  // D.C. Offset
                      SIGLIB_ZERO,                                  // Delay (samples periods) applied to impulse
                      SIGLIB_ZERO,                                  // Signal end value - Unused
                      SIGLIB_NULL_DATA_PTR,                         // Unused
                      SIGLIB_NULL_DATA_PTR,                         // Unused
                      IMPULSE_RESPONSE_LENGTH);                     // Output dataset length

// Apply fir filter and store filtered data
  SDA_Fir (pSrc,                                                    // Pointer to input array to be filtered
           pRealData,                                               // Pointer to filtered output array
           pFilterState,                                            // Pointer to filter state array
           pFilterTaps,                                             // Pointer to filter coefficients
           &FilterIndex,                                            // Pointer to filter index register
           FILTER_LENGTH,                                           // Filter length
           IMPULSE_RESPONSE_LENGTH);                                // Output dataset length

  gpc_plot_2d (h2DPlot,                                             // Graph handle
               pRealData,                                           // Dataset
               PLOT_LENGTH,                                         // Dataset length
               "Impulse response",                                  // Dataset title
               SIGLIB_ZERO,                                         // Minimum X value
               ((double) (PLOT_LENGTH - 1) / SAMPLE_RATE_HZ),       // Maximum X value
               "lines",                                             // Graph type
               "blue",                                              // Colour
               GPC_NEW);                                            // New graph
  printf ("\nImpulse response\nPlease hit <Carriage Return> to continue . . .");
  getchar ();

// Generate frequency response
  SDA_Rfft (pRealData,                                              // Pointer to real array
            pImagData,                                              // Pointer to imaginary array
            pFFTCoeffs,                                             // Pointer to FFT coefficients
            SIGLIB_BIT_REV_STANDARD,                                // Bit reverse mode flag / Pointer to bit reverse address table
            FFT_LENGTH,                                             // FFT length
            LOG2_FFT_LENGTH);                                       // log2 FFT length

// Calculate real power from complex
  SDA_LogMagnitude (pRealData,                                      // Pointer to real source array
                    pImagData,                                      // Pointer to imaginary source array
                    pResults,                                       // Pointer to log magnitude destination array
                    PLOT_LENGTH);                                   // Dataset length

  gpc_plot_2d (h2DPlot,                                             // Graph handle
               pResults,                                            // Dataset
               PLOT_LENGTH,                                         // Dataset length
               "Frequency response",                                // Dataset title
               SIGLIB_ZERO,                                         // Minimum X value
               (SAMPLE_RATE_HZ / SIGLIB_TWO),                       // Maximum X value
               "lines",                                             // Graph type
               "blue",                                              // Colour
               GPC_NEW);                                            // New graph
  printf ("\nFrequency response\n");


  printf ("\nHit <Carriage Return> to continue ....\n");
  getchar ();                                                       // Wait for <Carriage Return>
  gpc_close (h2DPlot);

  SUF_MemoryFree (pSrc);                                            // Free memory
  SUF_MemoryFree (pRealData);
  SUF_MemoryFree (pImagData);
  SUF_MemoryFree (pResults);
  SUF_MemoryFree (pFFTCoeffs);

  return (0);
}
