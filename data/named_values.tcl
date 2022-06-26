set x { X 2 3 4 5 6 7 }
set y { Y 1 3 5 5 3 1 }

set model [load_charts_model -tcl [list $x $y $y] -first_line_header]

set_charts_data -model $model -column 2 -name column_type \
    -value {{color} {named_values {
             {{1} {#FF0000}}
             {{2} {#00FF00}}
             {{5} {#0000FF}}
            }}}

set plot [create_charts_plot -type scatter -columns {{x X} {y Y} {color 2}} -title "X/Y"]
