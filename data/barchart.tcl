set model [load_model -csv data/multi_bar.csv -first_line_header]

set plot [create_plot -model $model -type barchart -columns "name=0,value=1"]
