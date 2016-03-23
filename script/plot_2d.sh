# !/bash/sh

cd ~/Projects/finance_analyzer
octave --silent ./script/plot_2d.m -f 00_05_00_00_01_03_00.csv
octave --silent ./script/plot_2d.m -f 00_05_00_00_01_06_00.csv
octave --silent ./script/plot_2d.m -f 00_05_00_00_01_09_00.csv
octave --silent ./script/plot_2d.m -f 00_05_00_00_01_12_00.csv

