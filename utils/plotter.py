import matplotlib.pyplot as plt

x_axes = [1, 5, 10]
times = {
    '1 thread': [7304, 43162, 78944],
    '8 threads': [1387, 10483, 22712]
}
speedups = {
    '8 threads': [5.26604, 4.11733, 3.47587]
}
colors = ['red', 'blue', 'green', 'yellow', 'orange', 'purple']

plt.figure(figsize=(10, 8))

for key, speedup, index in zip(speedups.keys(), speedups.values(), range(len(speedups))):
    plt.plot(x_axes, speedup, label=key, color=colors[index])

plt.title('Speedups')
plt.gcf().canvas.set_window_title('speedups')
plt.xlabel('Number of words')
plt.xticks(x_axes)
plt.ylabel('speedup')
plt.legend()

plt.show()


plt.figure(figsize=(10, 8))

for key, time, index in zip(times.keys(), times.values(), range(len(times))):
    plt.plot(x_axes, time, label=key, color=colors[index])

plt.title('Execution times')
plt.gcf().canvas.set_window_title('times')
plt.xlabel('Number of words')
plt.xticks(x_axes)
plt.ylabel('time (ms)')
plt.legend()

plt.show()
