#pragma once

#include <sstream>

#include "core/autodiff/Grad.h"
#include "core/reductions/Reduction.h"
#include "core/formulas/constants/Zero.h"
#include "core/formulas/PrintFormula.h"
#include "core/pre_headers.h"

namespace keops {

// Implements the summation reduction operation

template < class F, int tagI >
struct Sum_Reduction_Alias;

template < class F, int tagI = 0 >  // This syntaxic sugar will allow us to simplify sum(0) = 0
using Sum_Reduction = typename Sum_Reduction_Alias< F, tagI >::type;

template < class F, int tagI >
struct Sum_Reduction_Impl : public Reduction< F, tagI > {

  static const int DIM =
      F::DIM;        // DIM is dimension of output of convolution ; for a sum reduction it is equal to the dimension of output of formula

  static const int DIMRED = DIM;        // dimension of temporary variable for reduction

  // recursive function to print the formula as a string
  static void PrintId(::std::stringstream &str) {
    str << "Sum_Reduction (with tagI=" << tagI << ") of :" << ::std::endl;
    str << PrintFormula< F >();                // prints the formula F
  }

  template < typename TYPE >
  struct InitializeReduction {
    DEVICE INLINE void operator()(TYPE *tmp) {
#pragma unroll
      for (int k = 0; k < DIM; k++)
        tmp[k] = 0.0f; // initialize output
    }
  };

  // equivalent of the += operation
  template < typename TYPEACC, typename TYPE >
  struct ReducePairShort {
    DEVICE INLINE void operator()(TYPEACC *tmp, TYPE *xi, int j) {
#pragma unroll
      for (int k = 0; k < DIM; k++) {
        tmp[k] += xi[k];
      }
    }
  };

  // equivalent of the += operation
  template < typename TYPEACC, typename TYPE >
  struct ReducePair {
    DEVICE INLINE void operator()(TYPEACC *acc, TYPE *xi) {
#pragma unroll
      for (int k = 0; k < DIM; k++) {
        acc[k] += xi[k];
      }
    }
  };

  // Kahan scheme
  template < typename TYPEACC, typename TYPE >
  struct KahanScheme {
    static const int DIMACC = DIM;
    DEVICE INLINE void operator()(TYPEACC *acc, TYPE *xi, TYPE *tmp) {
#pragma unroll
	for (int k=0; k<DIM; k++)
        {
		TYPEACC a = xi[k] - tmp[k];
		TYPEACC b = acc[k] + a;
		tmp[k] = (b - acc[k]) - a;
		acc[k] = b;
	}
    }
  };

  template < typename TYPEACC, typename TYPE >
  struct FinalizeOutput {
    DEVICE INLINE void operator()(TYPEACC *acc, TYPE *out, TYPE **px, int i) {
#pragma unroll
      for (int k = 0; k < DIM; k++)
        out[k] = acc[k];
    }
  };

  template < class V, class GRADIN, class FO=void >
  using DiffT = Sum_Reduction< Grad< F, V, GRADIN >, (V::CAT) % 2 >;
  // remark : if V::CAT is 2 (parameter), we will get tagI=(V::CAT)%2=0, so we will do reduction wrt j.
  // In this case there is a summation left to be done by the user.

};

template < class F, int tagI >
struct Sum_Reduction_Alias {
  using type = Sum_Reduction_Impl< F, tagI >;
};

template < int DIM, int tagI >
struct Zero_Reduction;

template < int DIM, int tagI > // Simplification rule: sum(0) = 0
struct Sum_Reduction_Alias< Zero< DIM >, tagI > {
  using type = Zero_Reduction< DIM, tagI >;
};

#define Sum_Reduction(F, I) KeopsNS<Sum_Reduction<decltype(InvKeopsNS(F)),I>>()

}
