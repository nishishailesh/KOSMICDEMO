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
#pragma once

#include <cassert>
#include <fstream>
#include <iterator>

#include "../lib/boost/spirit/include/qi.hpp"

#include "kosmic_input.h"

namespace kosmic {

	/*
	 Extremely fast parser for "1 double per line" files.
	*/
	template<typename T> class input_file {

		const std::string& filename;

	public:

		input_file(const std::string& filename) : filename(filename) {}

		void read(hist_builder<T>& hist) {
			std::ifstream in(filename, std::ios::in);

			double parsed_double;
			struct {
				double* value;
				void operator()(double const& d, boost::spirit::qi::unused_type, boost::spirit::qi::unused_type) const {
					*value = d;
				}
			} double_parser{ &parsed_double };

			using boost::spirit::qi::double_;
			using boost::spirit::qi::char_;
			using boost::spirit::qi::parse;

			while(in.good()) {
				char buffer[1024];
				in.getline(buffer, 1023);

				auto first = std::begin(buffer);
				auto last = first + in.gcount() - 1;

				if(parse(first, last, *char_(' ') >> double_[double_parser] >> *char_(' ')) && first == last)
					hist.add(parsed_double);
			}
		}

	};

}

