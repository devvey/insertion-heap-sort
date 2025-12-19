#!/usr/bin/env python3
"""
Simple plot generator for sorting algorithms.
Run after the C++ program generates 'random_results.csv'
"""

import pandas as pd
import matplotlib.pyplot as plt
import os

print("=== Sorting Algorithms Plot Generator ===")

# Check for data file
if not os.path.exists('../random_results.csv'):
    print("ERROR: 'random_results.csv' not found!")
    print("Please run the C++ program first.")
    exit(1)

# Read data
df = pd.read_csv('../random_results.csv')
print(f"Loaded data for sizes: {list(df['Size'])}")

# Create figure
fig, axes = plt.subplots(1, 2, figsize=(14, 6))

# Plot 1: Linear scale
ax1 = axes[0]
if all(df['InsertionSort_us'] > 0):
    ax1.plot(df['Size'], df['InsertionSort_us'], 'r-o', label='Insertion Sort', linewidth=2)
ax1.plot(df['Size'], df['HeapSort_us'], 'b-s', label='Heap Sort', linewidth=2)
ax1.plot(df['Size'], df['StdSort_us'], 'g-^', label='std::sort', linewidth=2)
ax1.set_xlabel('Array Size')
ax1.set_ylabel('Time (microseconds)')
ax1.set_title('Performance Comparison (Linear)')
ax1.legend()
ax1.grid(True, alpha=0.3)

# Plot 2: Log scale
ax2 = axes[1]
if all(df['InsertionSort_us'] > 0):
    ax2.loglog(df['Size'], df['InsertionSort_us'], 'r-o', label='Insertion Sort', linewidth=2)
ax2.loglog(df['Size'], df['HeapSort_us'], 'b-s', label='Heap Sort', linewidth=2)
ax2.loglog(df['Size'], df['StdSort_us'], 'g-^', label='std::sort', linewidth=2)
ax2.set_xlabel('Array Size (log scale)')
ax2.set_ylabel('Time (microseconds, log scale)')
ax2.set_title('Performance Comparison (Log-Log)')
ax2.legend()
ax2.grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig('../performance_graph.png', dpi=300)
print("Graph saved as 'performance_graph.png'")

# Show summary
print("\n=== Summary ===")
for _, row in df.iterrows():
    print(f"Size {row['Size']}: Insertion={row['InsertionSort_us']:.1f}us, "
          f"Heap={row['HeapSort_us']:.1f}us, std::sort={row['StdSort_us']:.1f}us")

plt.show()