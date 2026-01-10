#ifndef IOPERF_H
#define IOPERF_H

#include <iostream>

static class _ioperf {
  public:
    _ioperf() {
        std::ios_base::sync_with_stdio(false);
    }
} _ioperf;

#endif // IOPERF_H
