set model [load_charts_model -csv data/dot/dot_bazel.csv -first_line_header]

set plot [create_charts_plot -model $model -type dot \
  -columns {{from 0} {to 1} {group 3}} -title "dot bazel"]
