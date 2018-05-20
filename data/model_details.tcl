#set modelId [load_model -tsv data/digits.tsv -first_line_header]
#set modelId [load_model -csv data/aster_data.csv -first_line_header]
#set modelId [load_model -csv data/cities.csv -first_line_header -column_type "3#real:min=0,max=1;4#color"]
set modelId [load_model -tsv data/multi_series.tsv -comment_header -column_type "0#time:format=%Y%m%d"]
#set modelId [load_model -tsv data/cities1.dat -comment_header -column_type "2#real:min=0"]

puts $modelId

set nr [get_property -model $modelId -name num_rows]
set nc [get_property -model $modelId -name num_columns]

puts $nr,$nc

for {set c 0} {$c < $nc} {incr c} {
  puts $c
}

for {set c 0} {$c < $nc} {incr c} {
  puts "Column: $c"

  set type [get_model -ind $modelId -column $c -name type]
  set minv [get_model -ind $modelId -column $c -name min]
  set maxv [get_model -ind $modelId -column $c -name max]

  puts "  Type: $type"
  puts "  Range: $minv, $maxv"

  set monotonic  [get_model -ind $modelId -column $c -name monotonic]
  set increasing [get_model -ind $modelId -column $c -name increasing]

  puts "  Monotonic: $monotonic"
  puts "  Increasing: $increasing"

  set num_unique [get_model -ind $modelId -column $c -name num_unique]

  puts "  Num Unique: $num_unique"
}

if {$nr > 100} {
  set nr 100
}

puts "Map Values"

for {set r 0} {$r < $nr} {incr r} {
  for {set c 0} {$c < $nc} {incr c} {
    set value [get_model -ind $modelId -column $c -row $r -name value]

    set map [get_model -ind $modelId -column $c -row $r -name map]

    puts "  Map: $r,$c,$value,$map"
  }
}
