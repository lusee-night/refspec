#include "SpecConfig.h"
#include "SpecOutput.h"
#include "SignalGenerator.h"
#include "SignalSource.h"
#include "RefSpectrometer.h"

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

    // bindings to SignalGenerator class
    py::class_<SignalGenerator>(m, "SignalGenerator")
        .def(py::init<size_t, size_t, size_t, float, float, float, float>());


    // bindings to SpecOutput class
    py::class_<SpecOutput>(m, "SpecOutput")
        .def(py::init<SpecConfig const *>())
        .def_readwrite("Nchannels",     &SpecOutput::Nchannels)
        .def_readwrite("Nspec",         &SpecOutput::Nspec)
        .def_readwrite("Nbins",         &SpecOutput::Nbins);


    // bindings to RefSpectrometer class
    py::class_<RefSpectrometer>(m, "RefSpectrometer")
        .def(py::init<SignalSource *, SpecConfig const *>())
        .def("run",                     &RefSpectrometer::run)        
        .def("blocks_processed",        &RefSpectrometer::blocks_processed);

    // NB. VIRTUAL, can't do bindings to SignalSource class
    // py::class_<SignalSource>(m, "SignalSource")
    //     .def(py::init<size_t, size_t>())
    //     .def("get_block_size", &SignalSource::get_block_size);

}

// PYBIND11_MODULE(refspec, m) {
//     // optional module docstring
//     m.doc() = "pybind11 plugin for refspec";

//     // bindings to SpecOutput class
//     py::class_<SpecOutput>(m, "SpecOutput")
//         .def(py::init<SpecConfig const *>())
//         .def_readwrite("Nchannels",     &SpecOutput::Nchannels)
//         .def_readwrite("Nspec",         &SpecOutput::Nspec)
//         .def_readwrite("Nbins",         &SpecOutput::Nbins)        
//         .def("get_Nfft",                &SpecOutput::get_Nfft)
//         .def("get_mode",                &SpecOutput::get_mode);







// }