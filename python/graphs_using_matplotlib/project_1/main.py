import numpy as np
import matplotlib.pyplot as plt
from mpmath import *

def graf1():
    frequency = np.logspace(0, 8, 400)
    neg = np.logspace(-8, 0, 400)
    ll = np.logspace(-8,8,800)
    one = np.sin(ll)*0+1
    cutoff_frequency = 100000
    gain = 1 / (1 + (frequency / cutoff_frequency)**2)

    plt.figure(figsize=(8, 6))
    plt.semilogx(frequency, gain, color='r', linewidth=2)
    plt.semilogx(neg, np.flip(gain), color='r', linewidth=2)

    plt.plot(ll,one,color='b')
    plt.annotate("30 dB",xy=[10**6,0.92],fontsize=20)
    one = np.sin(ll)*0+0.7
    plt.plot(ll,one,color='b')
    plt.annotate("27 dB",xy=[10**6,0.62],fontsize=20)
    plt.annotate("0 dB",xy=[10**7,0.02],fontsize=20)

    plt.vlines(x=1,ymin=0,ymax=1,color='b',ls="--")
    plt.vlines(x=10**5-4*10**4,ymin=0,ymax=0.7,color='b',ls="--")
    plt.vlines(x=10**-5+5*10**-6,ymin=0,ymax=0.7,color='b',ls="--")

    Bline = np.linspace(10**-5+5*10**-6,10**5-4*10**4,100000)
    one = np.sin(Bline)*0+0.3
    plt.plot(Bline,one)
    plt.arrow(10**5-10*10**4,0.3,4*10**4,0, head_width=0.05, head_length=3*10**4, length_includes_head=True, head_starts_at_zero=True)
    plt.arrow(10**-5+17*10**-6,0.3,-(10**-6),0,head_width=0.05, head_length=20*10**-6, length_includes_head=True, head_starts_at_zero=True)

    frame1 = plt.gca()
    frame1.axes.get_xaxis().set_ticks([])
    frame1.axes.get_yaxis().set_ticks([])

    plt.annotate('$\it{B}$',xy=[1.5,0.32],fontsize=20)
    plt.annotate('$\it{f}$',xy=[10**-5+10**-5,0],fontsize=20)
    plt.annotate('$\it{f}$',xy=[1.3,0],fontsize=20)
    plt.annotate('$\it{f}$',xy=[10**5-3*10**4,0],fontsize=20)
    plt.annotate('$\it{d}$',xy=[10**-5+9**-5,-0.01],fontsize=12)
    plt.annotate('$\it{0}$',xy=[1.8,-0.01],fontsize=12)
    plt.annotate('$\it{h}$',xy=[10**5,-0.01],fontsize=12)

    fig = plt.gcf()
    fig.canvas.manager.set_window_title('') 	

    plt.xlabel('Frekvence [Hz]')
    plt.ylabel('Zisk [dB]')
    plt.title('Šířka pásma')
    plt.tight_layout()
    plt.show()

def graf2():
    x = np.linspace(0,5*np.pi,1000)
    f_1 = -np.cos(x)
    f_2 = 1.5*np.cos(x)
    
    one = np.sin(x)*0
    two = np.sin(x)*0

    fig = plt.figure(figsize=(12, 5))
    
    plt.subplot(1,2,1)
    plt.plot(x,f_1)
    plt.xlabel('t [s]')
    plt.margins(x=0)
    plt.ylim(-2,2)
    plt.plot(x,one)
    frame1 = plt.gca()
    frame1.axes.get_xaxis().set_ticks([])
    frame1.axes.get_yaxis().set_ticks([])
    dps = fig.dpi_scale_trans.inverted()
    bbox = frame1.get_window_extent().transformed(dps)
    width, height = bbox.width, bbox.height
    xmin, xmax = frame1.get_xlim() 
    ymin, ymax = frame1.get_ylim()  
    hw = 1./20.*(ymax-ymin) 
    hl = 1./20.*(xmax-xmin)
    lw = 1.
    ohg = 0.3
    
    yhw = hw/(ymax-ymin)*(xmax-xmin)* height/width 
    yhl = hl/(xmax-xmin)*(ymax-ymin)* width/height
    
    frame1.arrow(xmin, -2, xmax-xmin, 0., fc='k', ec='k', lw = lw, 
            head_width=hw, head_length=hl, overhang = ohg, 
            length_includes_head= True, clip_on = False)
    frame1.arrow(0, ymin, 0., ymax-ymin, fc='k', ec='k', lw = lw, 
            head_width=yhw, head_length=yhl, overhang = ohg, 
            length_includes_head= True, clip_on = False) 
    frame1.spines['top'].set_visible(False)
    frame1.spines['right'].set_visible(False)
    frame1.spines['bottom'].set_visible(False)
    frame1.spines['left'].set_visible(False)

    plt.subplot(1,2,2)
    plt.plot(x,f_2)
    plt.xlabel('t [s]')
    plt.margins(x=0)
    plt.ylim(-2,2)
    plt.plot(x,two)
    frame1 = plt.gca()
    frame1.axes.get_xaxis().set_ticks([])
    frame1.axes.get_yaxis().set_ticks([])

    dps = fig.dpi_scale_trans.inverted()
    bbox = frame1.get_window_extent().transformed(dps)
    width, height = bbox.width, bbox.height
    xmin, xmax = frame1.get_xlim() 
    ymin, ymax = frame1.get_ylim()  
    hw = 1./20.*(ymax-ymin) 
    hl = 1./20.*(xmax-xmin)
    lw = 1.
    ohg = 0.3
    
    yhw = hw/(ymax-ymin)*(xmax-xmin)* height/width 
    yhl = hl/(xmax-xmin)*(ymax-ymin)* width/height
    
    frame1.arrow(xmin, -2, xmax-xmin, 0., fc='k', ec='k', lw = lw, 
            head_width=hw, head_length=hl, overhang = ohg, 
            length_includes_head= True, clip_on = False) 
    frame1.arrow(0, ymin, 0., ymax-ymin, fc='k', ec='k', lw = lw, 
            head_width=yhw, head_length=yhl, overhang = ohg, 
            length_includes_head= True, clip_on = False) 
    plt.tight_layout(pad=6.0)   
    frame1.spines['top'].set_visible(False)
    frame1.spines['right'].set_visible(False)
    frame1.spines['bottom'].set_visible(False)
    frame1.spines['left'].set_visible(False)
    plt.show()

def graf3():
    x = np.arange(-5,15,0.01)
    y = []
    for i in x:
        y.append(819*sech(i-5)+180)
    y = np.array(y)
    plt.plot(x,y)
    
    ax = plt.gca()
    ax.set_ylim([0,1200])
    ax.set_xlim([0,10])
    
    plt.xticks(list(plt.xticks()[0]) + [5])
    labels = [item.get_text() for item in ax.get_xticklabels()]
    labels[0] = "0.75"
    labels[-1] = "1"
    labels[5] = "1.25"
    ax.set_xticklabels(labels)
    xticks = ax.xaxis.get_major_ticks()
    for i in range(1,5):
        xticks[i].set_visible(False)

    plt.yticks(list(plt.yticks()[0])+[250, 500, 750])
    yticks = ax.yaxis.get_major_ticks()
    for i in range(1,5):
        yticks[i].set_visible(False)

    y250 = []
    y500 = []
    y750 = []
    y1000 = []
    for i in x:
        y250.append(250)
        y500.append(500)
        y750.append(750)
        y1000.append(1000)
        
    plt.plot(x,y250,color='k',linewidth=0.5)
    plt.plot(x,y500,color='k',linewidth=0.5)
    plt.plot(x,y750,color='k',linewidth=0.5)
    plt.plot(x,y1000,color='k',linewidth=0.5)
    plt.axvline(x=5, color='k', linewidth=0.5)
    plt.show()

def graf4():
    x = np.arange(-5,15,0.01)
    y = []
    for i in x:
        y.append((819*sech(i-5)+180))
    y = np.array(y)
    plt.plot(x,y, '--')
    
    ax = plt.gca()
    ax.set_ylim([0,1200])
    ax.set_xlim([0,10])
    
    plt.xticks(list(plt.xticks()[0]) + [5])
    labels = [item.get_text() for item in ax.get_xticklabels()]
    labels[0] = "0.75"
    labels[-1] = "1"
    labels[5] = "1.25"
    ax.set_xticklabels(labels)
    xticks = ax.xaxis.get_major_ticks()
    for i in range(1,7):
        xticks[i].set_visible(False)

    plt.yticks(list(plt.yticks()[0])+[250, 500, 750])
    yticks = ax.yaxis.get_major_ticks()
    for i in range(1,10):
        yticks[i].set_visible(False)
    
    x2 = np.arange(-5,6,1)
    y600 = []
    y2 = []
    for i in x:
        y2.append((950*sech(1.5*i-7.5)+180))

    for i in x2:
        y600.append(1130)
        
    plt.plot(x2,y600,'--',color='k',linewidth=0.85)
    plt.axvline(x=5, linestyle='--',color='k', linewidth=0.85)
    plt.plot(x,y2)
    plt.show()


graf1()
graf2()
graf3()
graf4()