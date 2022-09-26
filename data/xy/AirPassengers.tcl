set model [load_charts_model -csv data/AirPassengers.csv -first_line_header]

set plot [create_charts_plot -model $model -type xy -columns {{x time} {y value}}]
