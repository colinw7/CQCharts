set model [load_charts_model -csv data/summary_simple.csv -first_line_header]

set plot [create_charts_plot -type summary -model $model -columns {{columns {0 1}}}]
