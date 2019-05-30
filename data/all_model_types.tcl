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

show_charts_manage_models_dlg
