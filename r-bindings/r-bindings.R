library(Rcpp)
sourceCpp("r-bindings/kosmic_R.cpp")
call_kosmic_alg(rnorm(10000, 16, 1), 10000, 1, 0, 0, 1, 0.05, 0.30, 0.70, 0.95, 0.8, 1e-7)
