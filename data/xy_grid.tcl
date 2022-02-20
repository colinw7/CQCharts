set model [load_charts_model -tsv data/multi_series.tsv -comment_header \
  -column_type {{{time} {format %Y%m%d} {oformat %F}}}]

set plot [create_charts_plot -type xy -columns {{x 0} {y 1}} -title "XY Plot"]

set_charts_property -plot $plot -name xaxis.grid.fill -value {MAJOR}
set_charts_property -plot $plot -name xaxis.grid.lines -value {MAJOR_AND_MINOR}
set_charts_property -plot $plot -name xaxis.grid.major.fill.alpha -value {0.1}
set_charts_property -plot $plot -name xaxis.grid.major.stroke.color -value {#000000}
set_charts_property -plot $plot -name xaxis.grid.minor.stroke.color -value {#0000ff}

set_charts_property -plot $plot -name yaxis.grid.fill -value {MAJOR}
set_charts_property -plot $plot -name yaxis.grid.lines -value {MAJOR_AND_MINOR}
set_charts_property -plot $plot -name yaxis.grid.major.fill.alpha -value {0.1}
set_charts_property -plot $plot -name yaxis.grid.major.stroke.color -value {#000000}
set_charts_property -plot $plot -name yaxis.grid.minor.stroke.color -value {#0000ff}
