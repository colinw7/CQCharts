# Dot From/To
#set model [load_charts_model -csv data/dot_data/csv/abstract.csv -first_line_header]
#set model [load_charts_model -csv data/dot_data/csv/biological.csv -first_line_header]
set model [load_charts_model -csv data/dot_data/csv/dfa.csv -first_line_header]
#set model [load_charts_model -csv data/dot_data/csv/grammar.csv -first_line_header]
#set model [load_charts_model -csv data/dot_data/csv/NaN.csv -first_line_header]
#set model [load_charts_model -csv data/dot_data/csv/philo.csv -first_line_header]
#set model [load_charts_model -csv data/dot_data/csv/simple.csv -first_line_header]

set plot [create_charts_plot -model $model -type graphviz \
  -columns {{from 0} {to 1} {attributes 2}} -title "dot from/to"]
