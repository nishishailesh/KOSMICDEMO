#include <random>
#include "CppUnitTest.h"

#include "../src/kosmic_input.h"
#include "../src/kosmic.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace kosmic;

namespace math_tests
{
	TEST_CLASS(math_test)
	{
	public:
		TEST_METHOD(math_percentile_double) {
			double x[] = { 15, 20, 35, 40, 50 };
			Assert::AreEqual(29.0, quantile<double>(0.4, x, 5));
			Assert::AreEqual(35.0, quantile<double>(0.5, x, 5));
			Assert::AreEqual(17.0, quantile<double>(0.1, x, 5));
			Assert::AreEqual(15.0, quantile<double>(0.0, x, 5));
			Assert::AreEqual(48.0, quantile<double>(0.95, x, 5));
		}

		TEST_METHOD(math_percentile_float) {
			float x[] = { 15, 20, 35, 40, 50 };
			Assert::AreEqual(29.0f, quantile<float>(0.4f, x, 5));
			Assert::AreEqual(35.0f, quantile<float>(0.5f, x, 5));
			Assert::AreEqual(17.0f, quantile<float>(0.1f, x, 5));
			Assert::AreEqual(15.0f, quantile<float>(0.0f, x, 5));
			Assert::AreEqual(48.0f, quantile<float>(0.95f, x, 5));
		}

	};
};
