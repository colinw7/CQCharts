#set model [load_model -tsv data/adjacency.tsv -comment_header -column_type "3#connection_list"]
#set model [load_model -csv data/USArrests.csv -first_line_header -first_column_header]
#set model [load_model -tsv data/multi_series.tsv -comment_header -column_type "time:format=%Y%m%d"]
#set model [load_model -tsv data/scatter.tsv -first_line_header]
#set model [load_model -csv data/sankey_energy.csv -comment_header -column_type "0#name_pair"]
#set model [load_model -tsv data/states.tsv -comment_header -column_type "1#polygon_list"]
set model [load_model -csv data/boxplot.csv -first_line_header]

set tvars [process_model -model $model -analyze]

set plots {}

foreach tvar $tvars {
  set typeName    [lindex $tvar 0]
  set nameColumns [lindex $tvar 1]

  echo "$typeName : $nameColumns"

  if {[llength $nameColumns] > 0} {
    set i 0

    set columns ""

    foreach nameColumn $nameColumns {
      set name   [lindex $nameColumn 0]
      set column [lindex $nameColumn 1]

      if {$i > 0} {
        append columns ","
      }

      append columns "$name=$column"

      incr i
    }

    set plot [create_plot -type $typeName -model $model -columns $columns -title $typeName]

    lappend plots $plot
  }
}

set np [llength $plots]

set nr [expr {int(sqrt($np))}]

if {$nr > 1} {
  place_plots -rows $nr $plots
} else {
  place_plots -horizontal $plots
}
