# !/bash/sh

cd ~/Projects/finance_analyzer
./finance_analyzer --output_daily
octave --silent ./script/multiple_plot_2d_ex.m
#octave --silent ./script/multiple_plot_2d.m 00_05_00_00_01_03_00.csv 00_05_00_00_01_06_00.csv 00_05_00_00_01_09_00.csv 00_05_00_00_01_12_00.csv
#octave --silent ./script/plot_2d.m -f 00_05_00_00_01_03_00.csv
#octave --silent ./script/plot_2d.m -f 00_05_00_00_01_06_00.csv
#octave --silent ./script/plot_2d.m -f 00_05_00_00_01_09_00.csv
#octave --silent ./script/plot_2d.m -f 00_05_00_00_01_12_00.csv

