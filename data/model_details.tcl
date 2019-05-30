proc modelDetails { modelId } {
  set nr [get_charts_data -model $modelId -name num_rows]
  set nc [get_charts_data -model $modelId -name num_columns]

  set hier [get_charts_data -model $modelId -name hierarchical]

  puts $nr,$nc,$hier

  for {set c 0} {$c < $nc} {incr c} {
    puts "Column: $c"

    set type [get_charts_data -model $modelId -column $c -name type]
    set minv [get_charts_data -model $modelId -column $c -name min]
    set maxv [get_charts_data -model $modelId -column $c -name max]

    puts "  Type: $type"
    puts "  Range: $minv, $maxv"

    set monotonic  [get_charts_data -model $modelId -column $c -name monotonic]
    set increasing [get_charts_data -model $modelId -column $c -name increasing]

    puts "  Monotonic: $monotonic"
    puts "  Increasing: $increasing"

    set num_unique [get_charts_data -model $modelId -column $c -name num_unique]

    puts "  Num Unique: $num_unique"

    set num_null [get_charts_data -model $modelId -column $c -name num_null]

    puts "  Num Null: $num_null"
  }
}

proc modelCells { modelId } {
  puts "Model Cells"

  set nr [get_charts_data -model $modelId -name num_rows]
  set nc [get_charts_data -model $modelId -name num_columns]

# if {$nr > 100} {
#   set nr 100
# }

  for {set r 0} {$r < $nr} {incr r} {
    for {set c 0} {$c < $nc} {incr c} {
      set value [get_charts_data -model $modelId -column $c -row $r -name value -role display]

      if {$c == 0} {
        puts -nonewline "  $value"
      } else {
        puts -nonewline ",$value"
      }
    }

    puts ""
  }
}

proc mapValues { modelId } {
  puts "Map Values"

  set nr [get_charts_data -model $modelId -name num_rows]
  set nc [get_charts_data -model $modelId -name num_columns]

  for {set r 0} {$r < $nr} {incr r} {
    for {set c 0} {$c < $nc} {incr c} {
      set value [get_charts_data -model $modelId -column $c -row $r -name value]

      set map [get_charts_data -model $modelId -column $c -row $r -name map]

      puts "  Map: $r,$c,$value,$map"
    }
  }
}

#set modelId [load_charts_model -tsv data/digits.tsv -first_line_header]
#set modelId [load_charts_model -csv data/aster_data.csv -first_line_header]
#set modelId [load_charts_model -csv data/cities.csv -first_line_header \
#  -column_type {{{3 real} {min 0} {max 1}} {{4 color}}}]
set modelId [load_charts_model -tsv data/multi_series.tsv -comment_header \
  -column_type {{{0 time} {format %Y%m%d}}}]
#set modelId [load_charts_model -tsv data/cities1.dat -comment_header \
#  -column_type {{{2 real} {min 0}}}]

modelDetails $modelId

process_charts_model -model $modelId -add -expr "(column(1)+column(2)+column(3))/3" -header "Average"

modelDetails $modelId
