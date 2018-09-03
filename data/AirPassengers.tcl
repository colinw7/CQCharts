set model [load_model -csv data/AirPassengers.csv -first_line_header]

set plot [create_plot -model $model -type xy -columns "x=time,y=value"]
