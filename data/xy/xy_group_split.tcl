set model [load_charts_model -csv data/xy_group_split.csv -first_line_header]

set plot [create_charts_plot -type xy -columns {{x 0} {y 1} {group 2}} -title "Group Split"]

set_charts_property -plot $plot -name dataGrouping.splitGroups.enabled -value 1
set_charts_property -plot $plot -name points.visible -value 1
