#include "SpecConfig.h"
#include "SpecOutput.h"
#include <assert.h>
#include <iostream>

#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(refspec, m) {
    // optional module docstring
    m.doc() = "pybind11 plugin for refspec";

    // bindings to SpecOutput class
    py::class_<SpecOutput>(m, "SpecOutput")
        .def(py::init<SpecConfig const *>())
        .def("get_Nfft", &SpecOutput::get_Nfft)
        .def("get_mode", &SpecOutput::get_mode);

    // bindings to SpecConfig class
    py::class_<SpecConfig>(m, "SpecConfig")
        .def(py::init<>())
        .def("get_Ntaps", &SpecConfig::get_Ntaps)
        .def("Nbins", &SpecConfig::Nbins)
        .def("fundamental_frequency", &SpecConfig::fundamental_frequency);

}