import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

def main():
    # Determine project root (assumes this script is in plots/)
    project_root = Path(__file__).resolve().parent.parent

    # Read CSVs with your renamed column format
    results_dir = project_root / 'results'
    cpp = pd.read_csv(results_dir / 'benchmark_results_cpp.csv')
    java = pd.read_csv(results_dir / 'benchmark_results_java.csv')
    python = pd.read_csv(results_dir / 'benchmark_results_python.csv')

    # Add language labels for plotting
    cpp['language'] = 'C++'
    java['language'] = 'Java'
    python['language'] = 'Python'

    # Combine into one DataFrame (for plotting)
    df = pd.concat([cpp, java, python], ignore_index=True)

    # Compute CPU usage % = (avg_cpu_s / avg_wall_s) * 100
    df['cpu_usage_pct'] = (df['avg_cpu_s'] / df['avg_time_s']) * 100

    # Ensure plots directory exists
    plots_dir = project_root / 'plots'
    plots_dir.mkdir(parents=True, exist_ok=True)

    # Plot metrics (excluding avg_cpu_s, replaced by cpu_usage_pct)
    metrics = {
        'avg_time_s': 'Time (s)',
        'cpu_usage_pct': 'CPU Usage (%)',
        'avg_mem_mb': 'Memory Usage (MB)',
    }

    for metric, label in metrics.items():
        plt.figure()
        for lang in df['language'].unique():
            sub = df[df['language'] == lang].sort_values('size')
            plt.plot(sub['size'], sub[metric], marker='o', label=lang)
        plt.xlabel('Matrix Size (n × n)')
        plt.ylabel(label)
        plt.title(f'{label} by Matrix Size and Language')
        plt.legend()
        plt.grid(True)
        plt.tight_layout()
        plt.savefig(plots_dir / f'{metric}.png')
        plt.close()

    print("✅ Plots saved in:", plots_dir)
    df['avg_cpu_s'] = df['avg_cpu_s'].round(1)
    df['avg_mem_mb'] = df['avg_mem_mb'].round(1)
    df['cpu_usage_pct'] = df['cpu_usage_pct'].round(1)
    
    combined_csv_path = results_dir / 'benchmark_combined_clean.csv'
    df[['size', 'avg_cpu_s', 'avg_mem_mb', 'cpu_usage_pct', 'language']].to_csv(combined_csv_path, index=False)
    print("📄 Combined CSV saved to:", combined_csv_path)

if __name__ == '__main__':
    main()
