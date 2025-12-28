#!/usr/bin/python3
import sys
sys.path.append('/root/projects/kosmic/kosmic/python-bindings')
import kosmic
import csv
f=open('1.csv')
c=csv.reader(f)
x=[]
for d in c:
  print(d)
  if(len(d)>0):
    try:
      float_number = float(d[0])
      if(float_number<150):
        x=x+[float_number]
    except ValueError as e:
      pass
      #print(f"Error: {e}")
print(x)
#quit()
result = kosmic.kosmic(x, decimals=1)
lr=kosmic.percentile(result, 0.025)
ur=kosmic.percentile(result, 0.975)
print(lr,ur)
