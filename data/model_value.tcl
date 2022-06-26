set model [load_charts_model -csv data/type_data.csv -first_line_header \
 -column_type {{{0  boolean}} \
               {{1  integer}} \
               {{2  real}} \
               {{3  string}} \ 
               {{4  time} {format %m/%d/%Y}} \
               {{5  rect}} \
               {{6  color}} \
               {{7  polygon}} \
               {{8  path}} \
               {{9  style}} \
               {{10 polygon_list}} \
               {{11 connection_list}} \
               {{12 name_pair}}}]

set nr [get_charts_data -model $model -name num_rows   ]
set nc [get_charts_data -model $model -name num_columns]

# boolean,integer,real,string,time,rect,color,polygon,path,style,polygon_list,connection_list,name_pair

set columns {}

# DATA
lappend columns 0
# EXPR
lappend columns {#ONE (1)}
# ROW number
lappend columns @ROW
# ROW number + 1
lappend columns @ROW1
# COLUMN
lappend columns @COL
# CELL
lappend columns @CELL
# HHEADER
lappend columns @HHEADER
# VHEADER
lappend columns @VHEADER}
# GROUP
lappend columns @GROUP
# REF
lappend columns {@REF 0}
# DATA and ROLE
lappend columns {1@2}
# DATA and INDEX
lappend columns {4@2[%M]}

echo "--- Columns ---"

foreach column $columns {
  #echo $column
  print_charts_variant -type column -value $column
}

echo "--- Data ---"

for {set r 0} {$r < $nr} {incr r} {
  echo " --- Row $r ---"

  foreach column $columns {
    set v [get_charts_data -quiet -model $model -column $column -row $r -name value]

    echo "  $column = $v"
  }
}

#show_charts_manage_models_dlg
exit
