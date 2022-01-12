'''
Should be able to generate different time evolution plots.
    - choose folder (with comparison capability)
    - choose data to output and axis
'''
import sys
import argparse
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import rcParams

from tools import Parameters, loadfile

rcParams.update({'font.size': 20, 'axes.axisbelow': True})

PARSER = argparse.ArgumentParser()
PARSER.add_argument(dest='fnames', nargs='*', help='file to use for plotting')
PARSER.add_argument('-c', '--columns', action='append', default=[], help='column(s) to plot')
PARSER.add_argument('-l', '--legend', action='append', default=[], help='legend labels')
PARSER.add_argument('-o', '--outname', default='output.png', help='output filename')
PARSER.add_argument('-e', '--earth', action='store_true', help='plot earth val')
PARSER.add_argument('-n', '--notation', help='plot description, i.e., (a), (b)...')

ARGS = PARSER.parse_args()
PARAMS = Parameters().params
print(ARGS)


COLORS = ['black', 'red', 'orange', 'blue', 'lawngreen', 'cyan', 'grey']
#COLORS = ['black', 'lawngreen', 'red']
#COLORS = ['orange', 'blue', 'lawngreen']
STYLES = ['-', ':', '-.', '--']

FIG = plt.figure(figsize=(8, 8))
handles = []



def plot_time_evolution(ARGS):
    Final_Values = []
    islog = False
    print(ARGS.fnames)
    if len(ARGS.fnames) < 5:
        for j, fname in enumerate(sorted(ARGS.fnames)):
            xaxis, yaxis = loadfile(fname, ARGS.columns, PARAMS)
            #print(xaxis, yaxis)
            if xaxis is None:
                continue
            print("Enumerated_ ARGS collumns = ", ARGS.columns)
            for i, col in enumerate(ARGS.columns):
                h, = plt.plot(xaxis, yaxis[i], COLORS[j], ls=STYLES[j], lw=2, label = str(col))
                print(col)
                handles.append(h)
                if ARGS.earth:
                    plt.axhline(y=PARAMS[col].earthscale, color='k', ls='--', lw=2)
    else: # fill_between plot
        islog = False
        print("Enumerated_ ARGS collumns = ", ARGS.legend)
        counts_reservoirs = 0
        for i, col in enumerate(ARGS.columns):
            min_vals = None
            max_vals = None
            for fname in ARGS.fnames:
                print("fname -> ", fname)
                xaxis, yaxis = loadfile(fname, col, PARAMS)
                if xaxis is None:
                    continue

                if min_vals is None:
                    min_vals = yaxis[0]
                    max_vals = yaxis[0]
                    continue
                    
                elif min_vals.size == 0:
                    min_vals = yaxis[0]
                    max_vals = yaxis[0]
    
                if len(yaxis[0]) > 0:
                    x_vals = xaxis
                    min_vals = np.minimum(min_vals, yaxis[0])
                    max_vals = np.maximum(max_vals, yaxis[0])
            Final_Values.append([str(col), min_vals[-1], max_vals[-1]])   
    	    
           # print('min ----->', min_vals)  
           # print('max ----->', max_vals)
            
            reservoir = ARGS.legend[counts_reservoirs]
            h = plt.fill_between(x_vals, min_vals, max_vals,
                                 lw=None, edgecolor=COLORS[i], facecolor=COLORS[i], label = str(reservoir), alpha=0.65)
            handles.append(h)
            counts_reservoirs += 1
            
            print(np.max(max_vals[0]), np.min(min_vals[0]))
            if np.max(max_vals[0])/np.min(min_vals[0]) > 10 and np.min(min_vals[0]) > 0:
                islog = True
            
            #islog = True
            
	    
            if ARGS.earth:
                plt.axhline(y=PARAMS[col].earthscale, color=COLORS[i], ls='--', lw=2, label = str(reservoir)+str( " (PEL)"))
                print(reservoir)
            
            if ARGS.notation:
               plt.title(ARGS.notation, x=0.9, y=0.15)    
        
    plt.ylabel(PARAMS[ARGS.columns[0]].ylabel)
    plt.xlabel('Time [Ma]')
    plt.legend(bbox_to_anchor=(0.68, 1), loc=2 ,fontsize = 12)
    plt.xlim(-20, 4520)
    plt.xticks(np.arange(500, 5500, 1000))
    
    print("Final Values = ", Final_Values)
    
    if islog:
        plt.yscale('log')

    plt.grid(ls='dashed')
    plt.savefig(ARGS.outname, format='png', bbox_inches='tight')

if __name__ == "__main__":
    plot_time_evolution(ARGS)
