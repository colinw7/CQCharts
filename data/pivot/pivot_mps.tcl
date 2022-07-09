# Name,Party,Province,Age,Gender
set model [load_charts_model -csv data/mps.csv -first_line_header]

set plot [create_charts_plot -type image -title "Canadian MPS"]
