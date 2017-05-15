#pragma once

#include <string>
#include <iostream>

#include <vector>

#include "goofit/GlobalCudaDefines.h"

// Declaring friends
namespace GooFit {
class FCN;
class Minuit1;
}

class Indexable {
public:
    
    Indexable(std::string n, fptype val = 0) : name(n), value(val) {}
    
    // These classes can not be duplicated
    Indexable(Indexable &) = delete;
    Indexable& operator=(Indexable&) = delete;
    
    virtual ~Indexable() {}
    
    /// Get the GooFit index
    int getIndex() const {return index;}
    /// Set the GooFit index
    void setIndex(int value) {index = value;}
    
    /// Get the index from the fitter
    int getFitterIndex() const {return fitter_index;}
    /// Set the index (should be done by the fitter)
    void setFitterIndex(int value) {fitter_index = value;}

    /// Get the name
    const std::string& getName() const {return name;}
    /// Set the name
    void setName(const std::string& val) {name = val;}
    
    /// Get the value
    fptype getValue() const {return value;}
    /// Set the value
    void setValue(fptype val) {value = val;}
    
    // Utilities
    
    /// Support var = 3
    void operator=(const fptype& val) {setValue(val);}
    
    /// Support fptype val = var
    operator fptype() const {return getValue();}
    
    /// Support for less than, etc.
    
protected:
    
    /// The variable name. Should be unique
    std::string name;
    
    /// The value of the variable
    fptype value;
    
    /// The goofit index, -1 if unset
    int index {-1};
    
    /// The fitter index, -1 if unset
    int fitter_index {-1};
};

/// Contains information about a parameter allowed
/// to vary in MINUIT, or an observable passed to a
/// data set. The index can refer either to cudaArray
/// or to an event.
class Variable : public Indexable {
    friend GooFit::FCN;
    friend GooFit::Minuit1;
public:
    friend std::ostream& operator<< (std::ostream& o, const Variable& var);
    friend std::istream& operator>> (std::istream& o, Variable& var);

    // These classes can not be duplicated
    Variable(Variable &) = delete;
    Variable& operator=(Variable&) = delete;
    /// Support var = 3
    void operator=(const fptype& val) {setValue(val);}
    
    /// This is a constant varaible
    Variable(std::string n, fptype v)
      : Indexable(n, v)
      , error(0.002)
      , upperlimit(v + 0.01)
      , lowerlimit(v - 0.01)
      , fixed(true) {}
    
    /// This is an independent variable
    Variable(std::string n, fptype dn, fptype up)
    : Indexable(n)
    , upperlimit(up)
    , lowerlimit(dn) {}
    
    
    /// This is a normal variable, with value and upper/lower limits
    Variable(std::string n, fptype v, fptype dn, fptype up)
      : Indexable(n, v)
      , error(0.1*(up-dn))
      , upperlimit(up)
    , lowerlimit(dn) {}
    
    /// This is a full varaible with error scale as well
    Variable(std::string n, fptype v, fptype e, fptype dn, fptype up)
      : Indexable(n, v)
      , error(e)
      , upperlimit(up)
      , lowerlimit(dn) {}
    
    
    virtual ~Variable() = default;
    
    /// Get the error
    fptype getError() const {return error;}
    /// Set the error
    void setError(fptype val) {error = val;}
    
    /// Get the upper limit
    fptype getUpperLimit() const {return upperlimit;}
    /// Set the upper limit
    void setUpperLimit(fptype val) {upperlimit = val;}
    
    /// Get the lower limit
    fptype getLowerLimit() const {return lowerlimit;}
    /// Set the lower limit
    void setLowerLimit(fptype val) {lowerlimit = val;}
    
    /// Check to see if the value has changed this iteration (always true the first time)
    bool getChanged() const {return changed_;}
    
    /// Get and set the number of bins
    void setNumBins(size_t num);
    
    size_t getNumBins() const {return numbins;}
    
    /// Check to see if this is a constant
    bool IsFixed() const {return fixed;}
    
    /// Set the fixedness of a variable
    void setFixed(bool fix) {fixed = fix;}
    
    
    /// Check to see if this has been changed since last iteration
    void setChanged(bool val=true) {changed_ = val;}
    
    
    /// Get the bin size, (upper-lower) / bins
    fptype getBinSize() const {return (getUpperLimit() - getLowerLimit()) / getNumBins();}
    
    /// Hides the number; the real value is the result minus this value. Cannot be retreived once set.
    void setBlind(fptype val) {blind = val;}
    
    /// Ensure that the number of bins can not be changed once BinnedDataSets are created
    void setLockedBins(bool locked) {locked_ = locked;}
    
    /// Check to see if variable's bins have been locked by a BinnedDataSet
    bool getLockedBins() const {return locked_;};

    
protected:
    
    /// The error
    fptype error;
    
    /// The upper limit
    fptype upperlimit;
    
    /// The lower limit
    fptype lowerlimit;
    
    /// A blinding value to add (disabled at the moment, TODO)
    fptype blind {0};
    
    /// The number of bins (mostly for BinnedData, or plotting help)
    size_t numbins {100};
    
    /// True if the value was unchanged since the last iteration
    bool changed_ {true};
    
    /// This "fixes" the variable (constant)
    bool fixed {false};
    
    /// You can no longer change the binning after a BinnedDataSet is created
    bool locked_ {false};    
};

/// This is used to track event number for MPI versions.
/// A cast is done to know whether the values need to be fixed.
class CountingVariable : public Variable {
public:

    using Variable::Variable;
    virtual ~CountingVariable() = default;
    // These classes can not be duplicated
    CountingVariable& operator=(CountingVariable&) = delete;
    /// Support var = 3
    void operator=(const fptype& val) {setValue(val);}
};

/// This is similar to Variable, but the index points
/// to functorConstants instead of cudaArray.
class Constant : public Indexable {
public:
    
    // These classes can not be duplicated
    Constant(Constant &) = delete;
    Constant& operator=(Constant&) = delete;
    /// Support var = 3
    void operator=(const fptype& val) {setValue(val);}

    Constant(std::string n, fptype val) : Indexable(n, val) {}
    virtual ~Constant() {}
};

/// Nice print of Variable
std::ostream& operator<< (std::ostream& o, const Variable& var);

/// Allow Variable to be read in
std::istream& operator>> (std::istream& i, Variable& var);

/// Get the max index of a variable from a list
int max_index(const std::vector<Variable*> &vars);
