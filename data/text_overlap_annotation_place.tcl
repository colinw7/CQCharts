proc rand_in { min max } {
  set d [expr {$max - $min}]

  return [expr {rand()*$d + $min}]
}

set model [load_charts_model -csv data/sankey_energy.csv -comment_header \
 -column_type {{{0 name_pair}}}]

set plot [create_charts_plot -type empty -xmin 0 -xmax 100 -ymin 0 -ymax 100]

set group [create_charts_annotation_group -plot $plot -id group]

set nr [get_charts_data -model $model -name num_rows   ]
set nc [get_charts_data -model $model -name num_columns]

for {set r 0} {$r < $nr} {incr r} {
  set path  [get_charts_data -model $model -column 0 -row $r -name value -role display]
  set value [get_charts_data -model $model -column 1 -row $r -name value -role display]

  set x [rand_in 10 90]
  set y [rand_in 10 90]

  set id  $r
  set tip $r

  set text [create_charts_text_annotation -plot $plot -group $group -id $id -tip $tip \
              -position [list $x $y] -text $path]
}

set_charts_property -annotation $group -name layout.type -value TEXT_OVERLAP
