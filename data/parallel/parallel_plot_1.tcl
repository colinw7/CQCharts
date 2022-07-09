# Parallel Plot

set model1 [load_charts_model -tsv data/scatter.tsv -first_line_header]

set plot1 [create_charts_plot -model $model1 -type parallel \
  -columns {{x species} {y {0 1 2 3}}} -title "parallel"]
