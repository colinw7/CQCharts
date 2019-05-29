set model [load_charts_model -csv data/xy_1000.csv -first_line_header]

set plot1 [create_charts_plot -model $model -type barchart -columns {{value 1} {color "(color(100,100,255))"}}]

set plot2 [create_charts_plot -model $model -type boxplot -columns {{group "(0)"} {value 1}} \
  -properties {{options.horizontal 1}}]

place_charts_plots -vertical $plot1 $plot2
