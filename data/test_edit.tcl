#namespace import charts::*

interp alias {} test_edit {} charts::test_edit

set model [load_charts_model -tsv data/multi_series.tsv -comment_header \
  -column_type "time:format=%Y%m%d,oformat=%F"]

set plot [create_charts_plot -type xy -columns {{x 0} {y 1}} -title "XY Plot"]

#charts::test_edit -type alpha

#charts::test_edit -type arrow_data
#charts::test_edit -type arrow_data_line

#charts::test_edit -type box_data
#charts::test_edit -type box_data_line

#charts::test_edit -type color
#charts::test_edit -type color_line

#charts::test_edit -type column
#charts::test_edit -type column_line

#charts::test_edit -type columns
#charts::test_edit -type columns_line

#charts::test_edit -type fill_data
#charts::test_edit -type fill_data_line

#charts::test_edit -type fill_pattern

#charts::test_edit -type fill_under_pos
#charts::test_edit -type fill_under_pos_line

#charts::test_edit -type fill_under_side

#charts::test_edit -type filter

#charts::test_edit -type key_location

#charts::test_edit -type length

#charts::test_edit -type line_dash

#charts::test_edit -type line_data
#charts::test_edit -type line_data_line

#charts::test_edit -type polygon
#charts::test_edit -type polygon_line

#charts::test_edit -type position

#charts::test_edit -type rect

#charts::test_edit -type shape_data
#charts::test_edit -type shape_data_line

#charts::test_edit -type sides

#charts::test_edit -type stroke_data
#charts::test_edit -type stroke_data_line

charts::test_edit -type symbol_data
charts::test_edit -type symbol_data_line

#charts::test_edit -type symbol_type

#charts::test_edit -type text_box_data
#charts::test_edit -type text_box_data_line

#charts::test_edit -type text_data
#charts::test_edit -type text_data_line

if {0} {
charts::test_edit -plot $plot -editable 0 -type [list \
 arrow_data_line \
 box_data_line \
 color_line \
 column_line \
 columns_line \
 fill_data_line \
 fill_under_pos_line \
 line_data_line \
 polygon_line \
 shape_data_line \
 symbol_data_line \
 stroke_data_line \
 text_box_data_line \
 text_data_line]
}
