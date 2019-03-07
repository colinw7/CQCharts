set model [load_charts_model -csv data/multi_bar.csv -first_line_header]

set vis [process_charts_model -model $model -add -expr "1" -header "Visible" -type boolean]

set plot [create_charts_plot -model $model -type barchart -columns "name=0,value=1,visible=$vis"]
