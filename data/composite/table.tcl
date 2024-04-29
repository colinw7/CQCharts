set pie_model [load_charts_model -csv data/ages.csv -first_line_header \
  -column_type {{{1 integer}}}]

set scatter_model [load_charts_model -tsv data/multi_series.tsv -comment_header \
  -column_type {{{time} {format %Y%m%d} {oformat %F}}}]

set bubble_model [load_charts_model -csv data/flare1.csv -comment_header \
  -column_type {{{2 real}}}]

set plots {}

set nr 10
set nc 3

set dx [expr {100.0/$nc}]
set dy 25.0

set ymax [expr {$nr*$dy}]

for {set r 0} {$r < $nr} {incr r} {
  for {set c 0} {$c < $nc} {incr c} {
    set plot ""

    if       {$c == 0} {
      set plot [create_charts_plot -model $pie_model -type pie \
        -columns {{label 0} {values 1}} -title "Pie $r : $c"]

    } elseif {$c == 1} {
      set plot [create_charts_plot -type scatter -model $scatter_model \
        -columns {{x 0} {y 1}} -title "Scatter $r : $c"]
    } else {
      set plot [create_charts_plot -model $bubble_model -type bubble \
        -columns {{name 1} {value 2}} -title "Bubble $r : $c"]
    }

    set vx1 [expr {$c*$dx}]
    set vx2 [expr {$vx1 + $dx}]
    set vy2 [expr {$ymax - $r*$dy}]
    set vy1 [expr {$vy2 - $dy}]

    set_charts_property -plot $plot -name range.view -value [list [list $vx1 $vy1] [list $vx2 $vy2]]

    lappend plots $plot
  }
}

set view [get_charts_data -plot $plot -name view]

set_charts_property -view $view -name scroll.plots -value 1
