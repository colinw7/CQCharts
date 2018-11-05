set model [load_model -csv data/boxplot.csv -first_line_header \
 -column_type "integer;integer;integer"]

set plot1 [create_plot -model $model -type distribution -parameter "valueType=RANGE" -columns "value=0,data=2"]
