/*
 Reference Interval Estimation from Mixed Distributions using Truncation Points
 and the Kolmogorov-Smirnov Distance (kosmic)

 Copyright (c) 2020 Jakob Zierk
 jakobtobiaszierk@gmail.com or jakob.zierk@uk-erlangen.de

 Based on ideas and prior implementations by Farhad Arzideh
 farhad.arzideh@gmail.com or farhad.arzideh@uk-koeln.de

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 this program. If not, see <https://www.gnu.org/licenses/>.
*/
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <thread>

#include "../lib/args.hxx"

#include "kosmic.h"
#include "kosmic_input.h"
#include "kosmic_input_file.h"
#include "license_txt_gen.h"

namespace kosmic {

	/*
	 kosmic command line application.
	*/
	template<typename T> class cli_application {

		// Argument parser/parameters:
		args::ArgumentParser parser;
		args::ValueFlag<std::string> file_arg;
		args::ValueFlag<int> decimals_arg;
		args::ValueFlag<int> bootstrap_arg;
		args::ValueFlag<int> bootstrap_seed_arg;
		args::Flag show_bootstrap_arg;
		args::ValueFlag<int> threads_arg;
		args::ValueFlag<T> t1min_arg, t1max_arg, t2min_arg, t2max_arg, sd_arg;
		args::ValueFlag<T> tol_arg;
		args::ValueFlag<std::string> cost_arg;
		args::Flag json_arg;
		args::Flag runtime_arg;
		args::HelpFlag help_arg;
		args::HelpFlag license_arg;

		// kosmic settings/data:
		int decimals;
		algorithm_settings settings;
		hist_builder<T> hist;
		int bootstrap;
		int bootstrap_seed;
		best_box_cox_normal<T>* bootstrap_results;

		/*
		 Output a single reference interval result.
		*/
		void output_result(const best_box_cox_normal<T>& result, bool json) {
			T p025 = quantile(0.025, result);
			T p50 = quantile(0.500, result);
			T p975 = quantile(0.975, result);
			T t1 = hist.x(result.interval.t1_i), t2 = hist.x(result.interval.t2_i);
			if(!json)
				std::cout << p025 << "-" << p975 << " (P50: " << p50
				<< ", lambda: " << result.lambda << ", mu: " << result.mu << ", sigma: " << result.sigma
				<< ", T1: " << t1 << ", T2: " << t2 << ")";
			else
				std::cout << "{ \"p2.5\": " << p025 << ", \"p50\": " << p50 << ", \"p97.5\": " << p975
				<< ", \"lambda\": " << result.lambda << ", \"mu\": " << result.mu << ", \"sigma\": " << result.sigma
				<< ", \"t1\": " << t1 << ", \"t2\": " << t2 << " }";
		}

		/*
		 Calculate (using temporary array) confidence intervals of bootstrapped reference interval results.
		*/
		void output_cis(const best_box_cox_normal<T>& result, T q, bool json) {
			double* results = new double[bootstrap + 1];
			for(int i = 0; i < bootstrap; i++)
				results[i] = quantile<double>(q, bootstrap_results[i]);
			results[bootstrap] = quantile<double>(q, result);
			std::sort(&results[0], &results[bootstrap + 1]);
			if(json)
				std::cout << "[" << quantile<double>(0.05, results, bootstrap + 1) << ", " << quantile<double>(0.95, results, bootstrap + 1) << "]";
			else
				std::cout << quantile<double>(0.05, results, bootstrap + 1) << "-" << quantile<double>(0.95, results, bootstrap + 1);
			delete[] results;
		}

		/*
		 Convenience function, current time in ms.
		*/
		std::chrono::milliseconds now() {
			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
		}

	public:

		cli_application() :
			parser("kosmic\n(c) 2020 Jakob Zierk"),
			file_arg(parser, "filename", "Input file name. (One floating point number per line, decimal separator: \".\")", { 'f', "file" }, args::Options::Required | args::Options::Single),
			decimals_arg(parser, "decimals", "Number of decimal digits.", { 'd', "decimals" }, args::Options::Required | args::Options::Single),
			bootstrap_arg(parser, "bootstrap", "Number of bootstrap iterations, default=0.", { 'b', "bootstrap" }, 0, args::Options::Single),
			bootstrap_seed_arg(parser, "bootstrap-seed", "Bootstrap seed, default=0.", { 's', "bootstrap-seed" }, 0, args::Options::Single),
			show_bootstrap_arg(parser, "bootstrap-results", "Output bootstrapping results.", { "bootstrap-results" }),
			threads_arg(parser, "threads", "Number of threads for bootstrapping, default=#cores.", { "threads" }, std::thread::hardware_concurrency(), args::Options::Single),
			t1min_arg(parser, "t1min", "T1 search range start (quantile), default=0.05.", { "t1min" }, 0.05, args::Options::Single),
			t1max_arg(parser, "t1max", "T1 search range end (quantile), default=0.30.", { "t1max" }, 0.30, args::Options::Single),
			t2min_arg(parser, "t2min", "T2 search range start (quantile), default=0.70.", { "t2min" }, 0.70, args::Options::Single),
			t2max_arg(parser, "t2max", "T2 search range end (quantile), default=0.95.", { "t2max" }, 0.95, args::Options::Single),
			sd_arg(parser, "sd", "Quantile to use for initial sd guess (mode - sd), default=0.80.", { "sd" }, 0.80, args::Options::Single),
			tol_arg(parser, "tolerance", "Optimizer tolerance for conversion. Optimization stops if < |dObjective|, default=1e-7.", { 't', "tolerance" }, 1e-7, args::Options::Single),
			cost_arg(parser, "cost", "Cost function that is optimized (minimized) when estimating reference intervals, default=classic. Slightly optimized variant: ks_log", { "cost" }, "classic", args::Options::Single),
			json_arg(parser, "json", "Output JSON instead of text output.", { "json" }),
			runtime_arg(parser, "runtime", "Output time stats.", { "runtime" }),
			help_arg(parser, "help", "Show this help message.", { 'h', "help" }),
			license_arg(parser, "license", "Show license information.", { 'l', "license" }) {
			parser.Prog("kosmic");
			parser.helpParams.showValueName = false;
			parser.helpParams.helpindent = 30;
			parser.helpParams.flagindent = 4;
		}

		int run(int argc, char* argv[]) {
			try {
				std::chrono::milliseconds ms_0 = now();

				parser.ParseCLI(argc, argv);

				decimals = decimals_arg.Get();
				hist.set_decimals(decimals);
				input_file<T> hist_reader(file_arg.Get());
				hist_reader.read(hist);

				std::chrono::milliseconds ms_1 = now();

				best_box_cox_normal<T> result;
				bootstrap = bootstrap_arg.Get();
				if(bootstrap > 0)
					bootstrap_results = new best_box_cox_normal<T>[bootstrap];
				bootstrap_seed = bootstrap_seed_arg.Get();
				settings = { t1min_arg.Get(), t1max_arg.Get(), t2min_arg.Get(), t2max_arg.Get(), sd_arg.Get(), tol_arg.Get() };
				int return_code = 0;
				if(cost_arg.Get() == "classic")
					return_code = ri_estimator<T, kosmic::cost_evaluator_ks_classic<T>>::run(hist, settings, result, bootstrap_results, bootstrap, bootstrap_seed, threads_arg.Get());
				else if(cost_arg.Get() == "ks_log")
					return_code = ri_estimator<T, kosmic::cost_evaluator_ks_log<T>>::run(hist, settings, result, bootstrap_results, bootstrap, bootstrap_seed, threads_arg.Get());
				else
					throw args::ParseError("Invalid cost function specified");

				bool json = json_arg.Get();
				if(return_code == 0) {
					// Output main result:
					std::cout << (json ? "{\n \"result\": " : "Result: ");
					output_result(result, json);
					std::cout << (json ? ",\n" : "\n");

					// Output confidence intervals and bootstrapping results:
					if(bootstrap > 0) {
						std::cout << (json ? " \"confidence-intervals\": { \"p2.5\": " : "RI 90% CIs: ");
						output_cis(result, 0.025, json);
						std::cout << (json ? ", \"p50\": " : ", ");
						output_cis(result, 0.5, json);
						std::cout << (json ? ", \"p97.5\": " : ", ");
						output_cis(result, 0.975, json);
						std::cout << (json ? " },\n" : "\n");

						if(show_bootstrap_arg.Get()) {
							std::cout << (json ? " \"bootstrap\": [\n" : "Bootstrapping results:\n");
							for(int i = 0; i < bootstrap; i++) {
								if(json)
									std::cout << "  ";
								else
									std::cout << "Bootstrapping result #" << (i + 1) << ": ";
								output_result(bootstrap_results[i], json);
								if(json)
									std::cout << (i == bootstrap - 1 ? "\n" : ",\n");
								else
									std::cout << "\n";
							}
							if(json)
								std::cout << " ],\n";
						}
					}

					// Output settings/stats:
					if(json) {
						std::cout << " \"n\": " << hist.n() << ",\n";
						std::cout << " \"decimals\": " << decimals << ",\n";
						std::cout << " \"bootstrap-seed\": " << bootstrap_seed << ",\n";
						std::cout << " \"t1min\": " << settings.t1min_p << ",\n";
						std::cout << " \"t1max\": " << settings.t1max_p << ",\n";
						std::cout << " \"t2min\": " << settings.t2min_p << ",\n";
						std::cout << " \"t2max\": " << settings.t2max_p << ",\n";
						std::cout << " \"sd\": " << settings.sd_p << ",\n";
						std::cout << " \"tol\": " << settings.abs_tol << (runtime_arg.Get() ? ",\n" : "\n");
					} else {
						std::cout << "n: " << hist.n() << "\n";
						std::cout << "Decimals: " << decimals << "\n";
						std::cout << "Bootstrap seed: " << bootstrap_seed << "\n";
						std::cout << "T1/T2 search range, SD guess: " << settings.t1min_p << "-" << settings.t1max_p << "/"
							<< settings.t2min_p << "-" << settings.t2max_p << ", " << settings.sd_p << "\n";
						std::cout << "Convergence tolerance: " << settings.abs_tol << "\n";
					}
					std::chrono::milliseconds ms_2 = now();

					if(runtime_arg.Get()) {
						if(json)
							std::cout << " \"runtime\": { \"input\": " << (ms_1.count() - ms_0.count()) << ", \"calc\": " << (ms_2.count() - ms_1.count()) << " }\n";
						else
							std::cout << "Runtime (input, calculation): " << (ms_1.count() - ms_0.count()) << "ms, " << (ms_2.count() - ms_1.count()) << "ms\n";
					}

					if(json)
						std::cout << "}\n";
				} else {
					if(json)
						std::cout << "{ \"error\": true, \"error-code\": " << return_code << " }\n";
					else
						std::cout << "No reference interval estimation possible using the given dataset. Use -h or --help.\nError code: " << return_code << "\n";
				}

				if(bootstrap > 0)
					delete[] bootstrap_results;

				return return_code;
			} catch(args::Help) {
				// Output auto-generated help or license:
				if(help_arg.Get())
					std::cout << parser;
				else if(license_arg.Get())
					std::cout << license_txt;
				return 0;
			} catch(args::ParseError e) {
				// Output auto-generated error message:
				std::cerr << "Error: " << e.what() << "." << (help_arg.Get() ? "\n" : " Use -h or --help.\n");
				if(help_arg.Get())
					std::cout << "\n" << parser;
				return 1;
			} catch(args::ValidationError e) {
				// Output auto-generated error message:
				std::cerr << "Error: " << e.what() << "." << (help_arg.Get() ? "\n" : " Use -h or --help.\n");
				if(help_arg.Get())
					std::cout << "\n" << parser;
				return 1;
			}
		}

	};

}


int main(int argc, char* argv[]) {
	kosmic::cli_application<double> app;
	return app.run(argc, argv);
}
