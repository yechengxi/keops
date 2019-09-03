#pragma once

#include <assert.h>

#include "core/Pack.h"
#include "core/autodiff.h"
#include "core/formulas/maths/Sum.h"

namespace keops {

//////////////////////////////////////////////////////////////
////        Transpose of Sum : SumT< F >                   ////
//////////////////////////////////////////////////////////////
template<class F>
struct Sum;

template<class F, int D>
struct SumT : UnaryOp<SumT, F, D> {

  static_assert(F::DIM == 1, "Dimension of input must be 1 for SumT");

  static const int DIM = D;

  static void PrintIdString(std::stringstream& str) { str << "SumT"; }

  static HOST_DEVICE INLINE void Operation(__TYPE__ *out, __TYPE__ *outF) {
#pragma unroll
    for (int k = 0; k < DIM; k++)
      out[k] = *outF;
  }

  template<class V, class GRADIN>
  using DiffT = typename F::template DiffT<V, Sum<GRADIN>>;

};

}
