set ::env(CQ_CHARTS_GRAPHVIZ_INPUT_FILE) /tmp/dot_in.gv
set ::env(CQ_CHARTS_GRAPHVIZ_OUTPUT_FILE) /tmp/dot_out.dot

set model [load_charts_model -csv data/self_connect.csv -first_line_header]

set plot [create_charts_plot -model $model -type graphviz \
  -columns {{from 0} {to 1}} -title "self connect"]
