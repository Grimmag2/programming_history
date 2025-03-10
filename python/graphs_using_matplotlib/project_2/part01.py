#!/usr/bin/env python3
"""
Autor: xkalin16
"""

from bs4 import BeautifulSoup
import re
import requests
import numpy as np
from numpy.typing import NDArray
import matplotlib.pyplot as plt
from typing import List, Callable, Dict, Any

def generate_graph(a: List[float], show_figure: bool = False, save_path: str | None = None):    
    # Funkce pro prevod cislic na cisla ve spodnim indexu
    def subtext(x): 
        normal = "0123456789"
        sub_s = "₀₁₂₃₄₅₆₇₈₉"
        res = x.maketrans(''.join(normal), ''.join(sub_s)) 
        return x.translate(res)

    x = np.arange(-3, 3, 0.01)
    x_mesh, a_mesh = np.meshgrid(x, a)
    f = a_mesh**2 * x_mesh**3 * np.sin(x_mesh)
    
    fig = plt.figure(figsize=(8, 5))

    # leng  = delka vstupniho listu a
    # f_max = maximalni hodnota napric vsemi funkcemi
    leng = f_max = 0
    nums = "0123456789"
    for i, a in enumerate(a):
        txt_str = ""
        for j in str(a):
            if j in nums:
                txt_str = txt_str + subtext(j)
            else:
                txt_str = txt_str + '.'

        plt.plot(x, f[i], label="y"+txt_str+"(x)")
        plt.fill_between(x, f[i], alpha=0.1)
        leng += 1
        plt.annotate("∫f"+txt_str+"(x)dx = " + str("{:.2f}".format(np.trapz(f[i],dx=0.01))), xy=(3,f[i][-1]))
        
        temp = max(f[i])
        if temp>f_max:
            f_max=temp

    # Popsani a velikost os
    plt.xlabel('x')
    plt.ylabel('fₐ(x)')
    plt.margins(x=0)
    plt.ylim(0,f_max-(f_max % -10))
    plt.xlim(-3,5)

    # Smazani urcitych bodu na osach
    ax = plt.gca()
    xticks = ax.xaxis.get_major_ticks()
    xticks[-1].set_visible(False)
    xticks[-2].set_visible(False)

    # Legenda nad grafem
    plt.legend(loc = 'upper center', bbox_to_anchor=(0.5, 1.1), ncol = leng)        

    if (save_path != None):
        plt.savefig(save_path)
    if (show_figure):
        plt.show()


def generate_sinus(show_figure: bool = False, save_path: str | None = None):
    t = np.arange(0,101,0.01)
    f_1 = 0.5 * np.cos(np.pi*t/50)
    f_2 = 0.25 * (np.sin(np.pi*t) + np.sin(1.5*np.pi*t))
    

    figure, axis = plt.subplots(3,1)
    
    # Vykresleni prvniho grafu
    axis[0].plot(t,f_1)
    axis[0].set_xlabel('t')
    axis[0].set_ylabel('f₁(t)')
    axis[0].figure.set_figwidth(10)
    axis[0].figure.set_figheight(9)
    axis[0].margins(x=0)
    axis[0].margins(y=0)
    axis[0].set_xlim(0,100)
    axis[0].set_ylim(-0.8,0.8)
    
    # Vykresleni druheho grafu
    axis[1].plot(t,f_2)
    axis[1].set_xlabel('t')
    axis[1].set_ylabel('f₂(t)')
    axis[1].margins(x=0)
    axis[1].margins(y=0)
    axis[1].set_xlim(0,100)
    axis[1].set_ylim(-0.8,0.8)

    # Vytvoreni masky a spravnych funkci treti graf
    f_3 = f_1+f_2
    f_4 = f_1+f_2
    mask = f_3>f_1
    for i in enumerate(f_3):
        if mask[i[0]] == False:
            f_3[i[0]] = np.NaN
        else:
            f_4[i[0]] = np.NaN

    # Vykresleni tretiho grafu
    axis[2].plot(t,f_3,color="green")
    axis[2].plot(t,f_4,color="red")
    axis[2].set_xlabel('t')
    axis[2].set_ylabel('f₁(t)+f₂(t)')
    axis[2].margins(x=0)
    axis[2].margins(y=0)
    axis[2].set_xlim(0,100)
    axis[2].set_ylim(-0.8,0.8)

    plt.subplots_adjust(wspace=0.4,hspace=0.4)
    
    if (save_path != None):
        plt.savefig(save_path)
    if (show_figure):
        plt.show()

generate_graph([1., 1.5, 2.], show_figure=False,save_path="tmp_fn.png")
generate_sinus(show_figure=False, save_path="tmp_sin.png")
