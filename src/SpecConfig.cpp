#include "SpecConfig.h"

#include <pybind11/pybind11.h>

namespace py = pybind11;

SpecConfig::SpecConfig() :  mode (production), Nchannels(1), sampling_rate(1e8), Nfft(4096),
			    Ntaps(8), window(None), AverageSize(64), Ncalib(0), notch(false)
{
  for (size_t i=0;i<MAX_CHANNELS;i++) {
    plus_channel[i] = i;
    minus_channel[i] = -1;
 }

}

size_t SpecConfig::get_Ntaps() {
  return Ntaps;
}


// WARNING -- deprecated, kept for reference. All moved to refspec.cpp
PYBIND11_MODULE(SpecConfig, m) {
    // optional module docstring
    m.doc() = "pybind11 example plugin";

    // bindings to class
    py::class_<SpecConfig>(m, "SpecConfig")
        .def(py::init<>())
        .def("get_Ntaps", &SpecConfig::get_Ntaps)
        .def("Nbins", &SpecConfig::Nbins)
        .def_readwrite("Ntaps", &SpecConfig::Ntaps)
        .def("fundamental_frequency", &SpecConfig::fundamental_frequency);
//      .def(py::init<spec_mode_t, size_t, double, size_t, size_t, window_t, uint32_t, size_t, bool>());
}

