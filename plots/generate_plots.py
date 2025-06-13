import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

def main():
    # Determine project root (assumes this script is in plots/)
    project_root = Path(__file__).resolve().parent.parent

    # Read CSVs
    results_dir = project_root / 'results'
    cpp = pd.read_csv(results_dir / 'benchmark_results_cpp.csv')
    java = pd.read_csv(results_dir / 'benchmark_results_java.csv')
    python = pd.read_csv(results_dir / 'benchmark_results_python.csv')

    # Label languages
    cpp['language'] = 'C++'
    java['language'] = 'Java'
    python['language'] = 'Python'

    # Combine into one DataFrame
    df = pd.concat([cpp, java, python], ignore_index=True)

    # Metrics to plot
    metrics = {
        'avg_time_s': 'Wall-Clock Time (s)',
        'avg_cpu_s': 'CPU Time (s)',
        'avg_mem_mb': 'Memory Usage (MB)',
    }

    # Ensure plots directory exists
    plots_dir = project_root / 'plots'
    plots_dir.mkdir(parents=True, exist_ok=True)

    # Create plots
    for metric, label in metrics.items():
        plt.figure()
        for lang in df['language'].unique():
            sub = df[df['language'] == lang].sort_values('size')
            plt.plot(sub['size'], sub[metric], marker='o', label=lang)
        plt.xlabel('Matrix Size (n x n)')
        plt.ylabel(label)
        plt.title(f'{label} by Matrix Size and Language')
        plt.legend()
        plt.grid(True)
        plt.tight_layout()
        plt.savefig(plots_dir / f'{metric}.png')
        plt.close()

if __name__ == '__main__':
    main()
