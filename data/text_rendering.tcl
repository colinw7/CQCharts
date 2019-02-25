set model [load_model -csv data/one.csv]

set plot [create_plot -model $model -type xy -columns "x=0,y=1"]

set annotation [create_text_annotation -plot $plot -position {1 1} -text "The quick brown fox jumps\nover the lazy dog"]
