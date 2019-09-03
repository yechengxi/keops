#pragma once

#include <assert.h>

#include "core/Pack.h"
#include "core/autodiff.h"
#include "core/formulas/maths/SumT.h"

namespace keops {

//////////////////////////////////////////////////////////////
////                 SUM : Sum< F >                       ////
//////////////////////////////////////////////////////////////

template<class F, int D>
struct SumT;

template<class F>
struct Sum : UnaryOp<Sum, F> {

  static const int DIM = 1;

  static void PrintIdString(std::stringstream &str) {
    str << "Sum";
  }

  static HOST_DEVICE INLINE
  void Operation(__TYPE__ *out, __TYPE__ *outF) {
    *out = 0;
#pragma unroll
    for (int k = 0; k < F::DIM; k++)
      *out += outF[k];
  }

  template<class V, class GRADIN>
  using DiffT = typename F::template DiffT<V, SumT<GRADIN, F::DIM>>;

};

}
