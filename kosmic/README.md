# Reference Interval Estimation from Mixed Distributions using Truncation Points and the Kolmogorov-Smirnov Distance (kosmic)

(c) 2020 Jakob Zierk (jakobtobiaszierk@gmail.com or jakob.zierk@uk-erlangen.de).

Based on ideas and prior implementations by Farhad Arzideh (farhad.arzideh@gmail.com or farhad.arzideh@uk-koeln.de).

Available at https://gitlab.miracum.org/kosmic

## Options/command line parameters:
    -f, --file                Input file name. (One floating point number per line, decimal separator: ".")
    -d, --decimals            Number of decimal digits.
    -b, --bootstrap           Number of bootstrap iterations, default=0.
    -s, --bootstrap-seed      Bootstrap seed, default=0.
    --bootstrap-results       Output bootstrapping results.
    --threads                 Number of threads for bootstrapping, default=#cores.
    --t1min                   T1 search range start (quantile), default=0.05.
    --t1max                   T1 search range end (quantile), default=0.30.
    --t2min                   T2 search range start (quantile), default=0.70.
    --t2max                   T2 search range end (quantile), default=0.95.
    --sd                      Quantile to use for initial sd guess (mode - sd), default=0.80.
    -t, --tolerance           Optimizer tolerance for conversion. Optimization stops if < |dObjective|, default=1e-7.
    --cost                    Cost function that is optimized (minimized) when estimating reference intervals, default=classic. Slightly optimized variant: ks_log
    --json                    Output JSON instead of text output.
    --runtime                 Output time stats.
    -h, --help                Show this help message.
    -l, --license             Show license information.

## Installation:
### Linux:
```shell
$ git clone https://gitlab.miracum.org/kosmic/kosmic  
$ cd kosmic  
$ make  
```
### Windows:
Use the `.exe` and `.dll` files under `/builds`.
### Command line usage:
```shell
$ ./kosmic -h
```
Input files need to have a very simple format: one test result per line, decimal separator `.` (dot), text file
### Python binding usage:
> **Note:** Python 3 is required.
```shell
$ PYTHONPATH=python-bindings/ python
```
```python
import kosmic
import random
# Random "hemoglobin", n=1000, 900 "normal" samples (12.0-16.0), 100 "pathological" samples (10.0-14.0):
X = [round(random.normalvariate(14, 1/0.979982), 1) for i in range(900)] + [round(random.normalvariate(10, 1/0.979982), 1) for i in range(100)]
result = kosmic.kosmic(X, decimals=1)
# Estimated reference interval:
kosmic.percentile(result, 0.025)
kosmic.percentile(result, 0.975)
```

## License:
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
[GNU General Public License](GPL_3.0.md) for more details.
