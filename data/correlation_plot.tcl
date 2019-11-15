set model [load_charts_model -csv data/winequality-white.csv -first_line_header -separator {;}]

set plot [create_charts_plot -type correlation -model $model]
