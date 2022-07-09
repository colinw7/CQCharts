set model [load_charts_model -csv data/word_cloud.csv -first_line_header]

set nr [get_charts_data -model $model -name num_rows   ]
set nc [get_charts_data -model $model -name num_columns]

set nameCount {}

for {set r 0} {$r < $nr} {incr r} {
  set name [get_charts_data -model $model -column 0 -row $r -name value -role display]

  if {! [dict exists $nameCount $name]} {
    dict set nameCount $name 1
  } else {
    dict set nameCount $name [expr {[dict get $nameCount $name] + 1}]
  }
}

set plot [create_charts_plot -type empty -xmin 0 -xmax 1 -ymin 0 -ymax 1]

set group [create_charts_annotation_group -plot $plot -id group]

dict for { name value } $nameCount {
  set id  $name
  set tip $name

  set text [create_charts_text_annotation -plot $plot -group $group -id $id -tip $tip \
              -position [list 0 0] -text $name]

  set_charts_property -annotation $text -name value -value $value
}

set_charts_property -annotation $group -name layout.type -value TEXT_CLOUD

set_charts_property -plot $plot -name xaxis.visible -value 0
set_charts_property -plot $plot -name yaxis.visible -value 0
