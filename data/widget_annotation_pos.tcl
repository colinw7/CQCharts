set modelId [load_charts_model -csv data/p2.csv]

set plot [create_charts_plot -type empty -xmin -1 -ymin -1 -xmax 1 -ymax 1]

set viewId [get_charts_property -plot $plot -name viewId]

set w [qt_create_widget -type QPushButton -name Button]

qt_set_property -object $w -property text -value "Button"

set ann [create_charts_widget_annotation -plot $plot -id Button -widget $w]

set_charts_property -annotation $ann -name position -value {99 99 V}
set_charts_property -annotation $ann -name align -value {AlignRight|AlignTop}
