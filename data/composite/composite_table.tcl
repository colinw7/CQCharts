set plots {}

for {set r 0} {$r < 10} {incr r} {
  for {set c 0} {$c < 3} {incr c} {
    if       {$c == 0} {
      set pie_model [load_charts_model -csv data/ages.csv -first_line_header \
        -column_type {{{1 integer}}}]

      set pie_plot [create_charts_plot -model $pie_model -type pie \
        -columns {{label 0} {values 1}} -title "Pie Chart"]

      lappend plots $pie_plot
    } elseif {$c == 1} {
      set scatter_model [load_charts_model -tsv data/multi_series.tsv -comment_header \
        -column_type {{{time} {format %Y%m%d} {oformat %F}}}]

      set scatter_plot [create_charts_plot -type scatter -model $scatter_model \
        -columns {{x 0} {y 1}} -title "New York Temperatures"]

      lappend plots $scatter_plot
    } else {
      set bubble_model [load_charts_model -csv data/flare1.csv -comment_header \
        -column_type {{{2 real}}}]

      set bubble_plot [create_charts_plot -model $bubble_model -type bubble \
        -columns {{name 1} {value 2}} \
        -title "bubble"]

      lappend plots $bubble_plot
    }
  }
}

set cplot [group_charts_plots -composite -tabbed $plots]

set_charts_property -plot $cplot -name composite.numColumns   -value 3
set_charts_property -plot $cplot -name composite.rowHeight    -value 25V
set_charts_property -plot $cplot -name composite.headerHeight -value 5V
set_charts_property -plot $cplot -name composite.type         -value TABLE

set view [get_charts_data -plot $cplot -name view]

set_charts_property -view $view -name scroll.plots -value 1
