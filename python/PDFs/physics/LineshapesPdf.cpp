#include <pybind11/pybind11.h>

#include <goofit/Variable.h>
#include <goofit/PDFs/physics/LineshapesPdf.h>
#include <goofit/PDFs/physics/ResonancePdf.h>

using namespace GooFit;
namespace py = pybind11;

void init_Lineshape(py::module &m) {
    py::class_<Lineshape, GooPdf>(m, "Lineshape")
        .def(py::init<std::string, Variable *, Variable *, unsigned int, unsigned int, LS, FF, fptype, std::vector<Variable*>>())

        ;
}








