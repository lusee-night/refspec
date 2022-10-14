#include "SpecOutput.h"
#include <assert.h>
#include <iostream>

#include <pybind11/pybind11.h>

namespace py = pybind11;

SpecOutput::SpecOutput (SpecConfig const *config) : mode(config->mode),
    Nchannels(config->Nchannels), Nfft(config->Nfft), constructed(true) {
  //std::cout << "constructing"<<std::endl;
  Nspec = Nchannels*Nchannels; // think about matrix;
  Nbins = Nfft / 2 + 1;
  switch (mode) {
    case idle:
      break;
    case production:
      avg_pspec = new float*[Nspec];
      for (size_t i=0;i<Nspec;i++) 
        avg_pspec[i] = new float[Nbins];
      // add calibration thing
      break;
    default:
      assert(false); // not implemented
   }
}

SpecOutput::~SpecOutput() {
  //std::cout << "deconstructing"<<constructed<<std::endl;
  if (constructed) {
    switch (mode) {
    case idle:
      break;
    case production:
      for (size_t i=0;i<Nspec;i++) delete avg_pspec[i];
      delete avg_pspec;
    default:
      assert(false); // not implemented        
   }
  }
}

size_t SpecOutput::get_Nfft() {
  return Nfft;
}

int SpecOutput::get_mode() {
  return static_cast<int>(mode);
}

PYBIND11_MODULE(SpecOutput, m) {
    // optional module docstring
    m.doc() = "pybind11 example plugin";


    // bindings to class
    py::class_<SpecOutput>(m, "SpecOutput")
        .def(py::init<SpecConfig const *>())
        .def("get_Nfft", &SpecOutput::get_Nfft)
        .def("get_mode", &SpecOutput::get_mode);

}