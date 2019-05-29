set model [load_charts_model -csv data/boxplot.csv -first_line_header]

set plot [create_charts_plot -type barchart -columns {{group 0} {value 2}} -title "barchart"]
