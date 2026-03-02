import matplotlib.pyplot as plt
import numpy as np

# Данные для X (количество ключей)
keys = np.arange(200000, 3000001, 200000)

# Данные для Fusion Tree
fusion_times = np.array([
    1848.485, 2020.247, 2094.979, 2264.754, 2312.380, 2380.716, 2404.751, 
    2438.978, 2469.101, 2489.862, 2511.948, 2512.412, 2534.389, 2561.277, 2577.709
])
fusion_heights = np.array([8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9])

# Данные для RB-Tree
rb_times = np.array([
    493.371, 742.225, 859.522, 1011.589, 1064.665, 1150.452, 1231.228, 
    1255.776, 1298.451, 1327.219, 1356.977, 1385.383, 1431.527, 1491.215, 1506.315
])
rb_heights = np.array([33, 35, 36, 37, 37, 38, 38, 39, 39, 39, 40, 40, 40, 40, 40])

def get_log_params(x, y):
    """Вычисляет коэффициенты логарифмической регрессии y = a*ln(x) + b"""
    coeffs = np.polyfit(np.log(x), y, 1)
    return coeffs # [a, b]

# --- График 1: Только Fusion Tree Время ---
plt.figure(figsize=(10, 6))
c = get_log_params(keys, fusion_times)
plt.scatter(keys, fusion_times, color='blue', label='Экспериментальные данные')
plt.plot(keys, c[0] * np.log(keys) + c[1], '--', color='navy', 
         label=f'Тренд: {c[0]:.2f} ln(x) + ({c[1]:.2f})')
plt.title('Производительность Fusion Tree (Lookup)')
plt.xlabel('Количество ключей (n)')
plt.ylabel('Время поиска (нс)')
plt.legend()
plt.grid(True, alpha=0.3)
plt.show()

# --- График 2: Сравнение Fusion и RB с уравнениями ---
plt.figure(figsize=(10, 6))
# Расчет для Fusion
cf = get_log_params(keys, fusion_times)
# Расчет для RB
crb = get_log_params(keys, rb_times)

plt.scatter(keys, fusion_times, color='blue', alpha=0.5)
plt.plot(keys, cf[0] * np.log(keys) + cf[1], color='blue', linewidth=2,
         label=f'Fusion Tree: $y = {cf[0]:.2f} \ln(x) + ({cf[1]:.2f})$')

plt.scatter(keys, rb_times, color='red', alpha=0.5)
plt.plot(keys, crb[0] * np.log(keys) + crb[1], color='red', linewidth=2,
         label=f'RB-Tree: $y = {crb[0]:.2f} \ln(x) + ({crb[1]:.2f})$')

plt.title('Сравнение Lookup: Fusion Tree vs RB-Tree')
plt.xlabel('Количество ключей (n)')
plt.ylabel('Время поиска (нс)')
plt.legend(loc='upper left', fontsize='small')
plt.grid(True, alpha=0.3)
plt.show()

# --- График 3: Высоты (линейный вид) ---
plt.figure(figsize=(10, 6))
plt.plot(keys, fusion_heights, 'o-', color='blue', label='Fusion Tree Height', markersize=4)
plt.plot(keys, rb_heights, 's-', color='red', label='RB-Tree Height', markersize=4)
plt.title('Динамика изменения высоты деревьев')
plt.xlabel('Количество ключей (n)')
plt.ylabel('Высота (уровни)')
plt.yticks(np.arange(0, 45, 5))
plt.legend()
plt.grid(True, alpha=0.3)
plt.show()