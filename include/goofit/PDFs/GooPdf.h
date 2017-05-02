#pragma once

#include <thrust/functional.h>

#include "goofit/PdfBase.h"

#ifdef GOOFIT_MPI
#include <mpi.h>
#endif


#ifdef SEPARABLE
extern __constant__ fptype cudaArray[maxParams];
extern __constant__ unsigned int paramIndices[maxParams];
extern __constant__ fptype functorConstants[maxParams];
extern __constant__ fptype normalisationFactors[maxParams];

extern __device__ void* device_function_table[200];
extern void* host_function_table[200];
extern unsigned int num_device_functions;
#endif

__device__ int dev_powi(int base, int exp);  // Implemented in SmoothHistogramPdf.
void* getMetricPointer(std::string name);

 // Pass event, parameters, index into parameters.
typedef fptype(*device_function_ptr)(fptype*, fptype*, unsigned int*);

typedef fptype(*device_metric_ptr)(fptype, fptype*, unsigned int);

extern void* host_fcn_ptr;

__device__ fptype callFunction(fptype* eventAddress, unsigned int functionIdx, unsigned int paramIdx);

class MetricTaker;

class GooPdf : public PdfBase {
public:

    GooPdf(Variable* x, std::string n);
    __host__ virtual double calculateNLL() const;
    __host__ void evaluateAtPoints(std::vector<fptype>& points) const;
    __host__ void evaluateAtPoints(Variable* var, std::vector<fptype>& res);

    /// A normalize function. This fills in the host_normalize
    __host__ virtual fptype normalise() const;
    __host__ virtual fptype integrate(fptype lo, fptype hi) const {
        return 0;
    }
    __host__ virtual bool hasAnalyticIntegral() const {
        return false;
    }
    __host__ fptype getValue();
    __host__ void getCompProbsAtDataPoints(std::vector<std::vector<fptype>>& values);
    __host__ void initialise(std::vector<unsigned int> pindices, void* dev_functionPtr = host_fcn_ptr);
    __host__ void scan(Variable* var, std::vector<fptype>& values);
    __host__ virtual void setFitControl(FitControl* const fc, bool takeOwnerShip = true);
    __host__ virtual void setMetrics();
    __host__ void setParameterConstantness(bool constant = true);

    __host__ virtual void transformGrid(fptype* host_output);
    static __host__ int findFunctionIdx(void* dev_functionPtr);
    __host__ void setDebugMask(int mask, bool setSpecific = true) const;

protected:
    __host__ virtual double sumOfNll(int numVars) const;
    MetricTaker* logger;
private:

};

class MetricTaker : public thrust::unary_function<thrust::tuple<int, fptype*, int>, fptype> {
public:

    MetricTaker(PdfBase* dat, void* dev_functionPtr);
    MetricTaker(int fIdx, int pIdx);
    __device__ fptype operator()(thrust::tuple<int, fptype*, int> t)
    const;             // Event number, dev_event_array (pass this way for nvcc reasons), event size
    __device__ fptype operator()(thrust::tuple<int, int, fptype*> t)
    const;             // Event number, event size, normalisation ranges (for binned stuff, eg integration)

private:

    unsigned int metricIndex; // Function-pointer index of processing function, eg logarithm, chi-square, other metric.
    unsigned int functionIdx; // Function-pointer index of actual PDF
    unsigned int parameters;


};

