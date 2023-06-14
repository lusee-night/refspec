#include "testing.h"
#include <iostream>

CholKMatrix test() {
    CholKMatrix C;
    C.resize(1000);
    std::cout<<"here"<<std::endl;
    // for (auto &C_: C) {
    //     C_.resize(1);
    //     for (auto C__: C_)
    //         C__.resize(1);
    //     C_[0][0]=1;
    // }
   return C;
}