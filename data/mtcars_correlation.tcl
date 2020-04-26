set model [load_charts_model -csv data/mtcars.csv -first_line_header]

set plot [create_charts_plot -type correlation -model $model -title "MT Cars Correlation"]
