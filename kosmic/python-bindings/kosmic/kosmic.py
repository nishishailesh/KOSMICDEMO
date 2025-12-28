from math import exp, log
import ctypes
from ctypes import c_int, c_double, c_char_p, POINTER, ARRAY, Structure, pointer, byref
from multiprocessing import cpu_count
from scipy.stats import norm
import numpy as np
import os
from platform import architecture

def box_cox_transform(x, l):
	if l == 0:
		return log(x)
	else:
		return (x ** l - 1) / l

def box_cox_inverse(x, l):
	if l == 0:
		return exp(x)
	else:
		return (x * l + 1) ** (1 / l)

def percentile(result, p):
    return box_cox_inverse(norm.ppf(p, loc=result["mu"], scale=result["sigma"]), result["lambda"])

def percentile_cis(result, p):
	results = [percentile(r, p) for r in [result] + result["bootstrap"]]
	return np.percentile(results, 5), np.percentile(results, 50), np.percentile(results, 95)

class _best_box_cox_normal(Structure):
	_fields_ = [
		("l", c_double),
		("mu", c_double),
		("sigma", c_double),
		("ks", c_double),
		("t1", c_double),
		("t2", c_double)
	]

arch = architecture()
if arch[1] == "ELF":
	lib = ctypes = ctypes.CDLL(os.path.dirname(os.path.abspath(__file__)) + "/kosmic-lib.so")
else:
	if arch[0] == "32bit":
		lib = ctypes.CDLL(os.path.dirname(os.path.abspath(__file__)) + "\\..\\..\\builds\\win32\\kosmic.dll")
	elif arch[0] == "64bit":
		lib = ctypes.CDLL(os.path.dirname(os.path.abspath(__file__)) + "\\..\\..\\builds\\win-x64\\kosmic.dll")
	else:
		raise RuntimeError("Unknown architecture, can't load appropriate binary kosmic-library.")
lib.kosmic_lib.argtypes = [POINTER(c_double), c_int, c_int, c_int, c_int, c_int, c_double, c_double, c_double, c_double, c_double, c_double, c_char_p,
	POINTER(_best_box_cox_normal), POINTER(_best_box_cox_normal)]
lib.kosmic_lib.restype = c_int

def kosmic(samples, decimals, bootstrap=0, bootstrap_seed=0, threads=cpu_count(), t1=[0.05, 0.30], t2=[0.70, 0.95], sd=0.80, tol=1e-7, cost="classic"):
	data = (c_double * len(samples))(*samples)
	result = _best_box_cox_normal()
	if bootstrap > 0:
		bootstrap_result = (bootstrap * _best_box_cox_normal)()
	else:
		bootstrap_result = None
	if lib.kosmic_lib(data, len(samples), decimals, bootstrap, bootstrap_seed, threads, t1[0], t1[1], t2[0], t2[1], sd, tol, c_char_p(cost.encode("ASCII")), pointer(result), bootstrap_result) != 0:
		return { "error": True }
	if bootstrap == 0:
		bootstrap_result = []
	return {
		"lambda": result.l,
		"mu": result.mu,
		"sigma": result.sigma,
		"ks": result.ks,
		"t1": result.t1,
		"t2": result.t2,
		"bootstrap": [{
			"lambda": result.l,
			"mu": result.mu,
			"sigma": result.sigma,
			"ks": result.ks,
			"t1": result.t1,
			"t2": result.t2
		} for result in bootstrap_result]
	}
