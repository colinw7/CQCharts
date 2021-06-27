# load model
set model [load_charts_model -csv data/who_suicide_statistics.csv -first_line_header -spreadsheet]

set view [create_charts_view]

set plot [create_charts_plot -model $model -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]
