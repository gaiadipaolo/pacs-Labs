// compile with g++ -std=c++20 -O3 saxpy.cpp -ltbb -o saxpy (or -DUSEBOOST)
// run with ./saxpy [size]
#include <algorithm>
#include <chrono> //to compute bandwidth (measure of velocity/flow, how much data we are transferring)
#include <execution> //add if we want to run in parallel
#include <iostream>
#include <string>
#include <vector>

#ifdef USE_BOOST 
  #include <boost/iterator/counting_iterator.hpp>
#else
  #include <ranges>
#endif

using idx_t  = std::size_t;
using real_t = float;

int main(int argc, char** argv)
{
  // ----------------------------------
  // CLI size (default 2^10)
  // ----------------------------------
  idx_t n = 1 << 10;

  if (argc == 2)
    n = static_cast<idx_t>(std::stoull(argv[1]));

  std::cout << "Vector size: " << n << std::endl;

  // ----------------------------------
  // Allocate vectors
  // ----------------------------------
  std::vector<real_t> x(n), y(n);
  real_t a = 5.f;

  // Initialize
  for (idx_t i = 0; i < n; ++i) {
    x[i] = static_cast<real_t>(i);
    y[i] = 1.f;
  }

  // ----------------------------------
  // SAXPY lambda (S=single precision)
  // ----------------------------------
  auto saxpy = [xp = x.data(), yp = y.data(), a](idx_t i)
  {
    yp[i] += a * xp[i]; // SAXPY y = y + a * x
  };

  // ----------------------------------
  // Create index range
  // ----------------------------------
#ifdef USE_BOOST
  boost::counting_iterator<idx_t> first(0), last(n);
#else
  auto indices = std::views::iota(idx_t{0}, n);
  auto first = indices.begin();
  auto last = indices.end();
#endif

  // ----------------------------------
  // Timing
  // ----------------------------------
  using clock_t = std::chrono::steady_clock;
  const int nit = 100;

  auto start = clock_t::now();

  for (int it = 0; it < nit; ++it)
    std::for_each(std::execution::par_unseq, first, last, saxpy); //par_unseq parallel execution policy

  double seconds = std::chrono::duration<double>(clock_t::now() - start).count();

  // ----------------------------------
  // Bandwidth computation (how many objects we are working on)
  // y = y + a*x
  // x read + y read + y write = 3n (3 times the length of the vector)
  // ----------------------------------
  double gigabytes = static_cast<double>((3 * n) * sizeof(real_t) * nit) / 1.e9; 

  std::cout << "Bandwidth [GB/s]: " << gigabytes / seconds << std::endl;

  return 0;
}
