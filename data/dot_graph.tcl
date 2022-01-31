# Dot From/To
#set model [load_charts_model -csv data/dot_data/abstract.csv -first_line_header]
#set model [load_charts_model -csv data/dot_data/biological.csv -first_line_header]
set model [load_charts_model -csv data/dot_data/dfa.csv -first_line_header]
#set model [load_charts_model -csv data/dot_data/grammar.csv -first_line_header]
#set model [load_charts_model -csv data/dot_data/NaN.csv -first_line_header]
#set model [load_charts_model -csv data/dot_data/philo.csv -first_line_header]
#set model [load_charts_model -csv data/dot_data/simple.csv -first_line_header]

set plot [create_charts_plot -model $model -type graphviz \
  -columns {{from 0} {to 1} {attributes 2}} -title "dot from/to"]
