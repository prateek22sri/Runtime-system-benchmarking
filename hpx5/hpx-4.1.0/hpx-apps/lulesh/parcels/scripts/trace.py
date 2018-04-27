import pandas as pd
import numpy as np
import os.path

events=pd.read_csv(os.path.join(os.path.dirname(__file__), 'events.csv'))

def width(n):
    pd.set_option('display.width', n)

def load(data):
    samples=pd.read_csv(data, comment='#')
    samples=samples[(samples.iteration != 0) & (samples.iteration != 1)]
    samples=samples.join(events, on='event')
    return samples

def summarize(samples, values):
    summary=pd.pivot_table(samples, index='event', values=values,
                           aggfunc=[np.mean,np.std,max,min])
    summary=summary.stack()
    summary=summary.stack()
    summary=summary.unstack(1)
    summary=summary.unstack(1)
    return summary

def extract(table,rank,iteration):
    return table.loc[(table['rank'] == rank) &
                     (table['iteration'] == iteration)]
