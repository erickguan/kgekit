
    # auto io = m.def_submodule("io", "io service for kgekit");
    # io.def("read_triple_index", &kgekit::read_triple_index, "read dataset file includes triple index.",
    #       py::arg("filename"),
    #       py::arg("triple_order") = "hrt",
    #       py::arg("delimiter") = '\t');

def read_triple_index():
    print("hello")
