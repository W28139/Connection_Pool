import matplotlib.pyplot as plt
import pandas as pd
import os
import matplotlib

# 强制使用 Agg 后端，防止在无显示器的服务器上报错
matplotlib.use('Agg') 

def generate_plot():
    csv_file = 'test_results.csv'
    if not os.path.exists(csv_file):
        print("Error: test_results.csv not found!")
        return

    # 读取 CSV
    df = pd.read_csv(csv_file, names=['Threads', 'Type', 'TimeMS'])
    # 转换格式：Index 为 Threads, Columns 为 Type
    pivot_df = df.pivot_table(index='Threads', columns='Type', values='TimeMS')

    # 绘图
    ax = pivot_df.plot(kind='bar', figsize=(10, 6), color=['#66b3ff', '#ff9999'])
    
    plt.title('Database Performance: Pool vs No Pool', fontsize=14)
    plt.xlabel('Thread Count', fontsize=12)
    plt.ylabel('Total Time (ms)', fontsize=12)
    plt.xticks(rotation=0)
    plt.grid(axis='y', linestyle='--', alpha=0.6)

    # 在柱子上标数值
    for p in ax.patches:
        ax.annotate(f"{int(p.get_height())}", 
                    (p.get_x() + p.get_width() / 2., p.get_height()), 
                    ha='center', va='center', xytext=(0, 8), 
                    textcoords='offset points', fontsize=10)

    plt.tight_layout()
    plt.savefig('performance_report.png')
    print(">>> 可视化图表已生成: performance_report.png")

if __name__ == "__main__":
    generate_plot()