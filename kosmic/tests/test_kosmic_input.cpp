#include <random>
#include "CppUnitTest.h"

#include "../src/kosmic_input.h"
#include "../src/kosmic.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace kosmic;

namespace kosmic_input_tests
{
	TEST_CLASS(kosmic_input_test)
	{
	public:
		TEST_METHOD(kosmic_input_hist_1) {
			hist_builder<double> hist(1);
			Assert::AreEqual(0, (int) hist.classes());
			hist.add(12.09);
			Assert::AreEqual(1, (int)hist.classes());
			Assert::AreEqual(1, hist[0]);
			Assert::AreEqual(12.1, hist.min());
			Assert::AreEqual(12.1, hist.max());
			Assert::AreEqual(12.1, hist.x(0));
			hist.add(5.0);
			hist.add(20.0);
			Assert::AreEqual(151, (int)hist.classes());
			Assert::AreEqual(3, (int)hist.n());
			Assert::AreEqual(5.0, hist.x(0));
			Assert::AreEqual(6.0, hist.x(10));
			Assert::AreEqual(6.5, hist.x(15));
			Assert::AreEqual(20.0, hist.x(150));
			for (int i = 0; i < 1000; i++)
				hist.add(14.0);
			Assert::AreEqual((int) hist.classes(), 151);
			Assert::AreEqual((int) hist.n(), 1003);
			for (int i = 0; i < 998997; i++)
				hist.add(1.04);
			Assert::AreEqual((int) hist.classes(), 191);
			Assert::AreEqual((int) hist.n(), 1000000);
			Assert::AreEqual(hist.min(), 1.0);
			Assert::AreEqual(hist.max(), 20.0);
		}

		TEST_METHOD(kosmic_input_cdf) {
			hist_builder<double> hist(1);
			double values[10] = { 16.0, 12.0, 15.0, 13.0, 15.0, 13.0, 14.0, 14.0, 14.0, 14.0 };
			double* temp = new double[10 * 100000];
			for (int i = 0; i < 100000; i++) {
				for (int j = 0; j < 10; j++) {
					hist.add(values[j]);
					temp[10 * i + j] = values[j];
				}
			}
			Assert::AreEqual((int) hist.classes(), 41);
			Assert::AreEqual(hist.min(), 12.0);
			Assert::AreEqual(hist.max(), 16.0);
			cdf<double> cdf_1(hist);
//			kosmic_input<double>::cdf cdf_2(temp, 10*100000, 1);
			Assert::AreEqual((int) cdf_1.classes, 41);
/*			Assert::AreEqual(cdf_2.classes, 41);
			for (int i = 0; i < cdf_1.classes; i++) {
				Assert::AreEqual(cdf_1.x[i], cdf_2.x[i]);
				Assert::AreEqual(cdf_1.cum_freq[i], cdf_2.cum_freq[i]);
			}*/
			for (int i = 0; i < 10; i++)
				Assert::AreEqual(cdf_1.cum_freq[i], 1.0 / 10);
			for (int i = 10; i < 20; i++)
				Assert::AreEqual(cdf_1.cum_freq[i], 3.0 / 10);
			for (int i = 20; i < 30; i++)
				Assert::AreEqual(cdf_1.cum_freq[i], 7.0 / 10);
			for (int i = 30; i < 40; i++)
				Assert::AreEqual(cdf_1.cum_freq[i], 9.0 / 10);
			Assert::AreEqual(cdf_1.cum_freq[40], 1.0);
			delete[] temp;
		}

	};
};
