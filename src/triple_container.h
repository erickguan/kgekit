#pragma once
#include "kgekit.h"

namespace kgekit {

class TripleContainer {
public:
    virtual shared_ptr<vector<RawTriple>> read() = 0;
};

class LineTripleContainer : public TripleContainer {
public:
    LineTripleContainer(const vector<string>& lines);
    shared_ptr<vector<RawTriple>> read();
};

};
