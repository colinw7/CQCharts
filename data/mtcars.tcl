set model [load_model -csv data/mtcars.csv -first_line_header]

set plot [create_plot -model $model -type barchart -columns "name=model,value=mpg,group=cyl"]
