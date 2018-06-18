proc describe_model { model } {
  set nr [get_charts_data -model $model -name num_rows]
  set nc [get_charts_data -model $model -name num_columns]

  puts "($nr, $nc)"

  for {set c 0} {$c < $nc} {incr c} {
    set name [get_charts_data -model $model -column $c -header -name value]

    set num_null [get_charts_data -model $model -column $c -name num_null]

    set num_non_null [expr {$nr - $num_null}]

    set type [get_charts_data -model $model -column $c -name type]

    puts "$name\t$num_non_null non-null $type"
  }
}
