set model [load_charts_model -tsv data/multi_series.tsv -comment_header \
  -column_type {{time {format %Y%m%d}}}]

set model1 [group_charts_model -model $model -columns {1 2 3}]

set plot [create_charts_plot -model $model1 -type scatter3d \
  -columns {{x 0} {z 2} {y @GROUP} {group 1}} \
  -title "multiple xy plot"]
