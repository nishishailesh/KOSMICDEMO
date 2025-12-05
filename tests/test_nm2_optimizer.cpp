#include <random>
#include "CppUnitTest.h"

#include "../src/nm2_optimizer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace nm2_optimizer_tests
{
	TEST_CLASS(nm2_optimizer_test)
	{
		template<typename T> class rosenbrock_evaluator {
		public:
			struct objective {
				T val;
				T value() { return val; }
			};
			T a, b;
			rosenbrock_evaluator(T a, T b) : a(a), b(b) {}
			objective evaluate(T x, T y) {
				return objective{ (a - x) * (a - x) + b * (y - x * x) * (y - x * x) };
			}
		};

	public:
		TEST_METHOD(nm2_optimize_double) {
			rosenbrock_evaluator<double> ev(1, 100);
			auto res = nm2_optimizer<double, rosenbrock_evaluator<double>::objective, rosenbrock_evaluator<double>>::optimize(0, 0, ev, 0.0000000000000001);
			Assert::AreEqual(0.0, res.best_objective.value(), 0.00000000001);
			Assert::AreEqual(1.0, res.best_x0, 0.00001);
			Assert::AreEqual(1.0, res.best_x1, 0.00001);
		}

		TEST_METHOD(nm2_optimize_float) {
			rosenbrock_evaluator<float> ev(1, 100);
			auto res = nm2_optimizer<float, rosenbrock_evaluator<float>::objective, rosenbrock_evaluator<float>>::optimize(0, 0, ev, 0.00000000001f);
			Assert::AreEqual(0.0f, res.best_objective.value(), 0.0000001f);
			Assert::AreEqual(1.0f, res.best_x0, 0.0001f);
			Assert::AreEqual(1.0f, res.best_x1, 0.0001f);
		}

	};
};
