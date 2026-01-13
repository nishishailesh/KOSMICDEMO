#!/usr/bin/python3
from  matplotlib import pyplot as plt
import csv,sys
import scipy.signal as sig
import scipy
import pprint
#from numpy import trapz
from numpy import trapezoid
from scipy.signal import savgol_filter
import numpy as np
import pandas as pd
from statistics import median,mean
import time
import math

sys.path.append('/root/projects/ref/kosmic/python-bindings')
import kosmic

f=open(sys.argv[1])
c=csv.reader(f)
x=[]
for d in c:
  #print(d)
  if(len(d)>0):
    try:
      float_number = float(d[0])
      #if(float_number<150):
      x=x+[float_number]
    except ValueError as e:
      pass
      #print(f"Error: {e}")
#print(x)
#quit()

hist_result=plt.hist(x,bins=np.linspace(min(x),max(x),num=201),histtype='step')
plt.title(sys.argv[1])


result = kosmic.kosmic(x, decimals=1)
lr=kosmic.percentile(result, 0.025)
middle=kosmic.percentile(result, 0.50)
ur=kosmic.percentile(result, 0.975)
print(lr,ur,middle)
pprint.pprint(result)

basic_x=*np.linspace(0,1,101)
basic_y=[]
factor=middle/50
max_observation=max(hist_result[0])/50
ideal_y=[]
ideal_x=[]
mn=mean(x)

for i in basic_x:
    basic_y=basic_y+[kosmic.percentile(result, i)]



plt.plot(basic_x,basic_y)

#pprint.pprint([max(hist_result[1])/3,max(hist_result[0])/2])

plt.annotate(
              result, 
              (
                max(hist_result[1])/3,
                max(hist_result[0])/2
              ) 
            )
              
pprint.pprint([max(hist_result[1])/3,max(hist_result[0])/2])

#mu = result["mu"]
#sigma =result["sigma"]
#curve_data = np.linspace(mu - 6*sigma, mu + 6*sigma, 100)


#mu = 5.196881971671105
#sigma =0.7163059461821749
#curve_data = np.linspace(mu - 6*sigma, mu + 6*sigma, 100)
#plt.plot(curve_data, scipy.stats.norm.pdf(curve_data, mu, sigma))
#plt.plot(result)
plt.show()

plt.close()

'''

5.196881971671105 52.11899431980769
{'bootstrap': [],
 'ks': 0.0022613108952529936,
 'lambda': 0.07,
 'mu': 3.1508196560827586,
 'sigma': 0.7163059461821749,
 't1': 11.5,
 't2': 46.8}


'''
