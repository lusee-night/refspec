// General stuff needed for Ref Spectrometer

#include "SpecConfig.h"
#include "SpecOutput.h"
#include "SignalGenerator.h"
#include "SignalSource.h"
#include "RefSpectrometer.h"

// More classes for Calibrator Review
#include "PowerSpecSource.h"



#include <assert.h>
#include <iostream>

#include <pybind11/pybind11.h>

namespace py = pybind11;

// -mxp-
PYBIND11_MODULE(refspec, m) {
    // optional module docstring
    m.doc() = "pybind11 plugin for refspec";

    // bindings to SpecConfig class
    py::class_<SpecConfig>(m, "SpecConfig")
        .def(py::init<>())
        .def_readwrite("Ntaps",         &SpecConfig::Ntaps)
        .def("Nbins",                   &SpecConfig::Nbins)
        .def_readwrite("Nchannels",     &SpecConfig::Nchannels)
        .def_readwrite("sampling_rate", &SpecConfig::sampling_rate)        
        .def_readwrite("Average1Size",  &SpecConfig::Average1Size)
        .def_readwrite("Average2Size",  &SpecConfig::Average2Size)
        .def("AverageSize",             &SpecConfig::AverageSize)
        .def("fundamental_frequency",   &SpecConfig::fundamental_frequency)
        .def_readwrite("Ncalib",        &SpecConfig::Ncalib)
        .def_readwrite("calib_odd",     &SpecConfig::calib_odd)
        .def_readwrite("calib_subint",  &SpecConfig::calib_subint)
        .def_readwrite("Nfft",          &SpecConfig::Nfft)
        .def_readwrite("notch",         &SpecConfig::notch);

    // Declare the base class SignalSource (abstract)
    py::class_<SignalSource>(m, "SignalSource");

    // bindings to SignalGenerator class
    py::class_<SignalGenerator, SignalSource>(m, "SignalGenerator")
        .def(py::init<size_t, size_t, size_t, float, float, float, float>());


    // bindings to SpecOutput class
    py::class_<SpecOutput>(m, "SpecOutput")
        .def(py::init<SpecConfig const *>())
        .def("get_avg_pspec",           &SpecOutput::get_avg_pspec)
        .def_readwrite("Nchannels",     &SpecOutput::Nchannels)
        .def_readwrite("Nspec",         &SpecOutput::Nspec)
        .def_readwrite("Nbins",         &SpecOutput::Nbins);


    // bindings to RefSpectrometer class
    py::class_<RefSpectrometer>(m, "RefSpectrometer")
        .def(py::init<SignalSource *, SpecConfig const *>())
        .def("run",                     &RefSpectrometer::run)        
        .def("blocks_processed",        &RefSpectrometer::blocks_processed);

    // bindings to PowerSpecSource class
    py::class_<PowerSpecSource, SignalSource>(m, "PowerSpecSource")
        .def(py::init<const std::vector<double>&, const std::vector<double>&, float, size_t, size_t, size_t, bool, bool, int>());
    

}

