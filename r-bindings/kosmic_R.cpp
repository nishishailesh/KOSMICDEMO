// [[Rcpp::plugins(cpp11)]]

#include "../src/kosmic.h"
#include "../src/kosmic_library.h"

// AMmer
#include <Rcpp.h>
using namespace Rcpp;
//[[Rcpp::plugins(cpp11)]]
//[[Rcpp::export]]
NumericVector call_kosmic_alg(NumericVector inputVec, int n, int decimals, int bootstrap, int bootstrap_seed, int threads,
                              double t1min, double t1max, double t2min, double t2max, double sd, double tol){
  
  kosmic_input<double>::hist_builder hist(decimals);
  for (int i = 0; i < n; i++){
    hist.add(inputVec[i]);
  }
  
  //Rcpp::Rcout << inputVec << std::endl;
  best_box_cox_normal_lib* result = new best_box_cox_normal_lib();
  best_box_cox_normal_lib* bootstrap_results;
  
  kosmic<double>::best_box_cox_normal internal_result;
  kosmic<double>::best_box_cox_normal* internal_bootstrap_results;
  
  if(bootstrap > 0){
    bootstrap_results = new best_box_cox_normal_lib[bootstrap];
    internal_bootstrap_results = new kosmic<double>::best_box_cox_normal[bootstrap];
  }else{
    bootstrap_results = NULL;
    internal_bootstrap_results = NULL;
  }
  kosmic<double>::algorithm_settings settings = { t1min, t1max, t2min, t2max, sd, tol };
  
  //double *input_test = new double[n];
  
  //double const *input = input_test;
  
  //Rcpp::Rcout << "input" << std::endl << input_test << std::endl;
  
  //int return_code = kosmic_lib(input, n, decimals, bootstrap, bootstrap_seed, threads, t1min, t1max, t2min, t2max, sd, tol, result, bootstrap_results);
  int return_code = kosmic<double>::run(hist, settings, internal_result, internal_bootstrap_results, bootstrap, bootstrap_seed, threads);
  // Copy results:
  if(return_code == 0) {
    *result = {
      internal_result.lambda,
      internal_result.mu,
      internal_result.sigma,
      internal_result.interval.ks,
      hist.x(internal_result.interval.t1_i),
      hist.x(internal_result.interval.t2_i)
    };
    Rcpp::Rcout << "result: lambda: " << internal_result.lambda << std::endl;
    Rcpp::Rcout << "mu: " << internal_result.mu << std::endl;
    Rcpp::Rcout << "sigma: "<< internal_result.sigma <<std::endl;
    Rcpp::Rcout << "ks: "<< internal_result.interval.ks << std::endl;
    for(int i = 0; i < bootstrap; i++)
      bootstrap_results[i] = {
        internal_bootstrap_results[i].lambda,
        internal_bootstrap_results[i].mu,
        internal_bootstrap_results[i].sigma,
        internal_bootstrap_results[i].interval.ks,
        hist.x(internal_bootstrap_results[i].interval.t1_i),
        hist.x(internal_bootstrap_results[i].interval.t2_i)
      };
  }
  Rcpp::Rcout <<"return code "  << return_code << std::endl;
  NumericVector r_result = NumericVector::create(Named("l", result->l), Named("mu", result->mu), Named("sigma", result->sigma), Named("ks", result->ks),
                                                 Named("t1", result->t1), Named("t2", result->t2));
  return r_result;
}
// /Ammer

