#!/usr/bin/python3
from  matplotlib import pyplot as plt
import csv,sys
import scipy.signal as sig
import scipy
import pprint
from numpy import trapezoid
from scipy.signal import savgol_filter
import numpy as np
import pandas as pd
from statistics import median,mean
import time
import math

np.set_printoptions(suppress=True)
sys.path.append('/root/projects/ref/kosmic/python-bindings')
import kosmic

plts=plt.subplots(2,3)
print(plts)
#quit()
 
f=open(sys.argv[1])
c=csv.reader(f)
x=[]
for d in c:
  #print(d)
  if(len(d)>0):
    try:
      float_number = float(d[0])
      x=x+[float_number]
    except ValueError as e:
      pass

hist_result=plts[1][0,0].hist(x,bins=np.linspace(min(x),max(x),num=201),histtype='step')
hist_result_cum=plts[1][1,0].hist(x,bins=np.linspace(min(x),max(x),num=201),histtype='step',cumulative=True)
h_mu=mean(hist_result[0])


result = kosmic.kosmic(x, decimals=1)
lr=kosmic.percentile(result, 0.025)
middle=kosmic.percentile(result, 0.50)
ur=kosmic.percentile(result, 0.975)

plts[1][0,0].plot([ur,ur],[0,1400])
plts[1][0,0].plot([lr,lr],[0,1400])

plts[1][1,1].plot([ur,ur],[0,0.5])
plts[1][1,1].plot([lr,lr],[0,0.5])

plts[1][1,0].plot([ur,ur],[0,10000])
plts[1][1,0].plot([lr,lr],[0,10000])


mu    = result["mu"]
sigma = result["sigma"]
  
curve_data = np.linspace(mu - 3*sigma, mu + 3*sigma, 100)

plts[1][0,1].plot(curve_data, scipy.stats.norm.pdf(curve_data, mu, sigma))

pprint.pprint(hist_result)
pprint.pprint(result)

plts[1][0,2].axis('off')
plts[1][1,2].axis('off')
plts[1][0,2].bottom=0.2

plts[1][1,2].bottom=0.2



t1=plts[1][0,2].table(
  [
    ['parameter','value'],
    ['mu',result["mu"]],
    ['sigma',result["sigma"]],
    ['t1(low cut-off)',result["t1"]],
    ['t2(low cut-off)',result["t2"]],
    ['lambda',result["lambda"]],
    ['ks',result["ks"]],
    ['RI (2.5%-97.5%)','{}-{}'.format(lr,ur)],
    ['mean (50%)',middle]
  ],loc="center")

t2=plts[1][1,2].table(
  [
    ['parameter','value'],
    ['mu',result["mu"]],
    ['sigma',result["sigma"]],
    ['t1(low cut-off)',result["t1"]],
    ['t2(low cut-off)',result["t2"]],
    ['lambda',result["lambda"]],
    ['ks',result["ks"]],
    ['RI (2.5%-97.5%)','{}-{}'.format(lr,ur)],
    ['mean (50%)',middle]
  ],loc="center")


t1.auto_set_font_size(False)
t1.set_fontsize(12)
  
basic_x=np.linspace(0,1,101)
basic_y=[]
for i in basic_x:
    basic_y=basic_y+[kosmic.percentile(result, i)]

plts[1][1,1].plot(basic_y,basic_x)

plt.show()

plt.close()



'''
{'bootstrap': [],
 'ks': 0.0022613108952529936,
 'lambda': 0.07,
 'mu': 3.1508196560827586,
 'sigma': 0.7163059461821749,
 't1': 11.5,
 't2': 46.8}

'''
