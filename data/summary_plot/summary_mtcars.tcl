set model [load_charts_model -csv data/mtcars.csv -first_line_header]
set plot [create_charts_plot -type summary -model $model -columns {{columns {1 2 3 4 5 6 7 8 9 10 11}}}]
