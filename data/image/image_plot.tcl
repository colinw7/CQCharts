set model [load_charts_model -csv data/housetasks.csv -first_line_header -first_column_header]

set plot [create_charts_plot -model $model -type image -title "Household Tasks"]
