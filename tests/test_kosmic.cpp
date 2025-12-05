#include <random>
#include "CppUnitTest.h"

#include "../src/kosmic_input.h"
#include "../src/kosmic.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace kosmic;

namespace kosmic_tests
{
	TEST_CLASS(kosmic_test)
	{
	public:
		TEST_METHOD(kosmic_test_hb_healthy_25000_path_0) {
			typedef ri_estimator<double, kosmic::cost_evaluator_ks_classic<double>> ri_estimator;
			const int n = 25000;
			std::mt19937 generator(0);
			std::normal_distribution<double> distribution(14.0, 1.0 / 0.979982);
			hist_builder<double> hist(1);
			for (int i = 0; i < n; i++)
				hist.add(distribution(generator));
			algorithm_settings alg_settings = { 0.05, 0.30, 0.70, 0.95, 0.80, 1e-7 };
			best_box_cox_normal<double> res;
			ri_estimator::run(hist, alg_settings, res, NULL, 0, 0, 1);
			Assert::AreEqual(12.0, quantile(.025, res), 0.2);
			Assert::AreEqual(14.0, quantile(.500, res), 0.2);
			Assert::AreEqual(16.0, quantile(.975, res), 0.2);
		}

		TEST_METHOD(kosmic_test_hb_healthy_20000_path_2_2500) {
			typedef ri_estimator<double, kosmic::cost_evaluator_ks_classic<double>> ri_estimator;
			const int n1 = 20000, n2 = 2500;
			std::mt19937 generator(0);
			std::normal_distribution<double> distribution1(14.0, 1.0 / 0.979982), distribution2(12.0, 1.0 / 0.979982), distribution3(16.0, 1.0 / 0.979982);
			hist_builder<double> hist(1);
			for(int i = 0; i < n1; i++)
				hist.add(distribution1(generator));
			for(int i = 0; i < n2; i++)
				hist.add(distribution2(generator));
			for(int i = 0; i < n2; i++)
				hist.add(distribution3(generator));
			algorithm_settings alg_settings = { 0.05, 0.30, 0.70, 0.95, 0.80, 1e-7 };
			best_box_cox_normal<double> res;
			ri_estimator::run(hist, alg_settings, res, NULL, 0, 0, 1);
			Assert::AreEqual(12.0, quantile(.025, res), 0.4);
			Assert::AreEqual(14.0, quantile(.500, res), 0.4);
			Assert::AreEqual(16.0, quantile(.975, res), 0.4);
		}
		TEST_METHOD(kosmic_test_hb_healthy_20000_path_2_2500_avg_100) {
			typedef ri_estimator<double, kosmic::cost_evaluator_ks_classic<double>> ri_estimator;
			double p025[100], p975[100];
			for(int bootstrap = 0; bootstrap < 100; bootstrap++) {
				const int n1 = 20000, n2 = 2500;
				std::mt19937 generator(bootstrap);
				std::normal_distribution<double> distribution1(14.0, 1.0 / 0.979982), distribution2(12.0, 1.0 / 0.979982), distribution3(16.0, 1.0 / 0.979982);
				hist_builder<double> hist(1);
				for(int i = 0; i < n1; i++)
					hist.add(distribution1(generator));
				for(int i = 0; i < n2; i++)
					hist.add(distribution2(generator));
				for(int i = 0; i < n2; i++)
					hist.add(distribution3(generator));
				algorithm_settings alg_settings = { 0.05, 0.30, 0.70, 0.95, 0.80, 1e-7 };
				best_box_cox_normal<double> res;
				ri_estimator::run(hist, alg_settings, res, NULL, 0, 0, 1);
				p025[bootstrap] = quantile(.025, res);
				p975[bootstrap] = quantile(.975, res);
			}
			std::sort(&p025[0], &p025[100]);
			std::sort(&p975[0], &p025[100]);
			Assert::AreEqual(12.0, quantile(0.5, p025, 100), 0.4);
			Assert::AreEqual(16.0, quantile(0.5, p975, 100), 0.4);
			/*			Logger::WriteMessage(std::to_string(math::quantile(0.5, p025, 100)).data());
			Logger::WriteMessage("\n");
			Logger::WriteMessage(std::to_string(math::quantile(0.5, p975, 100)).data());*/
		}

	};
};
