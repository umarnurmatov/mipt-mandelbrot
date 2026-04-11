import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

def plot_cpu_metrics(title, csv_file, cycles_file, save_filename):
    df = pd.read_csv(csv_file, delimiter='\t')
    cycles = np.loadtxt(cycles_file)
    
    cycles_frames = np.arange(len(cycles))
    time_seconds = np.arange(0, len(df) * 0.5, 0.5)
    time_edges = np.append(time_seconds, time_seconds[-1] + 0.5)
    
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(14, 10))
    
    ax1.step(cycles_frames, cycles, 'g-', linewidth=1.5, where='mid')
    ax1.set_xlabel('Номер кадра')
    ax1.set_ylabel('Количество тиков TSC')
    ax1.grid(True, alpha=0.3)
    
    color1 = '#1f77b4'
    ax2.stairs(df['Bzy_MHz'].values, time_edges, color=color1, fill=True, label='Bzy_MHz',baseline=df['Bzy_MHz'].min()-50,zorder=1)
    ax2.set_xlabel('Время, с')
    ax2.set_ylabel('Частота, МГц', color=color1)
    ax2.tick_params(axis='y', labelcolor=color1)
    ax2.grid(True, alpha=0.3)
    
    ax2_twin = ax2.twinx()
    color2 = '#d62728'
    ax2_twin.stairs(df['CoreTmp'].values, time_edges, color=color2, fill=True, label='CoreTemp', baseline=df['CoreTmp'].min()-5,zorder=2)
    ax2_twin.set_ylabel('Температура ядра, ⁰C', color=color2)
    ax2_twin.set_ylim(df['CoreTmp'].min()-5, 90)
    ax2_twin.tick_params(axis='y', labelcolor=color2)
    
    cycles_mean = cycles.mean()
    cycles_std = cycles.std()
    tsc_mean = df['Bzy_MHz'].mean()
    tsc_std = df['Bzy_MHz'].std()
    temp_mean = df['CoreTmp'].mean()
    temp_std = df['CoreTmp'].std()
    
    stats = f'тики [{cycles_mean:.2E}, 3σ = {3*cycles_std:.2E}] тиков/кадр | частота [{tsc_mean:.0f}, 3σ = {3*tsc_std:.0f}] МГц'
    fig.suptitle(title + ' | ' + stats, fontsize=11, y=0.99)
    
    plt.tight_layout()
    plt.savefig(save_filename, dpi=300)

class bconf:
    def __init__(self, dname, txt):
        self.dname = dname
        self.txt = txt

class bversion:
    def __init__(self, fname, txt):
        self.fname = fname
        self.txt = txt

stat_dir = [
            bconf('stats_O2', 'оптимизация -O2'),
            bconf('stats_O3', 'оптимизация -O3')
           ]

benches = [
            bversion('bench_naive',     'простая версия'),
            bversion('bench_intrinsic', 'intrinsic-версия'),
            bversion('bench_arrs',      'версия с ручным loop-unroll')
          ]

for sd in stat_dir:
    for bn in benches:
        plot_cpu_metrics(f'{sd.txt}, {bn.txt}', f'{sd.dname}/{bn.fname}.stat', f'{sd.dname}/{bn.fname}.clk', f'img/plots/{sd.dname}_{bn.fname}.png')
