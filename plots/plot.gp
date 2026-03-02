# Настройка терминала SVG
# size 600,400 — это внутренние единицы, которые хорошо масштабируются
set terminal svg size 600,400 dynamic font "Arial,12"

set grid
set key left top

# Функции для аппроксимации
f(x) = a * log(x) + b
g(x) = c * log(x) + d

# --- График 1: Fusion Tree ---
set output 'fusion_time.svg'
set title "Performance of Fusion Tree (Lookup)"
set xlabel "Number of Keys (n)"
set ylabel "Time (ns)"
fit f(x) 'data.txt' using 1:2 via a, b
plot 'data.txt' using 1:2 with points pt 7 lc rgb "blue" title "Fusion (Exp.)", \
     f(x) with lines lw 2 lc rgb "navy" title sprintf("Fit: %.2f*ln(x) + %.2f", a, b)

# --- График 2: Сравнение ---
set output 'comparison_time.svg'
set title "Lookup Time Comparison: Fusion vs RB-Tree"
fit f(x) 'data.txt' using 1:2 via a, b
fit g(x) 'data.txt' using 1:3 via c, d
plot 'data.txt' using 1:2 with points pt 7 lc rgb "blue" title "Fusion", \
     f(x) with lines lw 2 lc rgb "blue" title "Fusion Fit", \
     'data.txt' using 1:3 with points pt 5 lc rgb "red" title "RB-Tree", \
     g(x) with lines lw 2 lc rgb "red" title "RB-Tree Fit"

# --- График 3: Высоты ---
set output 'heights_comparison.svg'
set title "Tree Height Comparison"
set ylabel "Height (levels)"
set yrange [0:45]
plot 'data.txt' using 1:4 with linespoints pt 7 lw 2 lc rgb "blue" title "Fusion Height", \
     'data.txt' using 1:5 with linespoints pt 5 lw 2 lc rgb "red" title "RB-Tree Height"

unset output