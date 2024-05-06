proc annotationSlot { viewId plotId id } {
  puts "annotationSlot: $viewId, $plotId, $id"

  set id [get_charts_property -annotation $id -name id]
  puts "id: $id"
}

set plot [create_charts_plot -type empty -xmin -100 -ymin -100 -xmax 100 -ymax 100]

set group [create_charts_annotation_group -plot $plot -id group]

set path1 [create_charts_path_annotation -plot $plot -id one -group $group \
  -path "M14,85l3,9h72c0,0,5-9,4-10c-2-2-79,0-79,1" -properties {{fill.color #7C4E32} {stroke.visible 0}}]
set path2 [create_charts_path_annotation -plot $plot -id two -group $group \
  -path "M19,47c0,0-9,7-13,14c-5,6,3,7,3,7l1,14c0,0,10,8,23,8c14,0,26,1,28,0c2-1,9-2,9-4c1-1,27,1,27-9c0-10,7-20-11-29c-17-9-67-1-67-1" -properties {{fill.color #E30000} {stroke.visible 0}}]
set path3 [create_charts_path_annotation -plot $plot -id three -group $group \
  -path "M17,32c-3,48,80,43,71-3 l-35-15" -properties {{fill.color #FFE1C4} {stroke.visible 0}}]
set path4 [create_charts_path_annotation -plot $plot -id four -group $group \
  -path "M17,32c9-36,61-32,71-3c-20-9-40-9-71,3" -properties {{fill.color #8ED8F8} {stroke.visible 0}}]
set path5 [create_charts_path_annotation -plot $plot -id five -group $group \
  -path "M54,35a10 8 60 1 1 0,0.1zM37,38a10 8 -60 1 1 0,0.1z" -properties {{fill.color #FFF} {stroke.visible 0}}]
set path6 [create_charts_path_annotation -plot $plot -id six -group $group \
  -path "M41,6c1-1,4-3,8-3c3-0,9-1,14,3l-1,2h-2h-2c0,0-3,1-5,0c-2-1-1-1-1-1l-3,1l-2-1h-1c0,0-1,2-3,2c0,0-2-1-2-3M17,34l0-2c0,0,35-20,71-3v2c0,0-35-17-71,3M5,62c3-2,5-2,8,0c3,2,13,6,8,11c-2,2-6,0-8,0c-1,1-4,2-6,1c-4-3-6-8-2-12M99,59c0,0-9-2-11,4l-3,5c0,1-2,3,3,3c5,0,5,2,7,2c3,0,7-1,7-4c0-4-1-11-3-10" -properties {{fill.color #FFF200} {stroke.visible 0}}]
set path7 [create_charts_path_annotation -plot $plot -id seven -group $group \
  -path "M56,78v1M55,69v1M55,87v1" -properties {{stroke.color #000} {stroke.width 1.0} {stroke.cap round}}]
set path8 [create_charts_path_annotation -plot $plot -id eight -group $group \
  -path "M60,36a1 1 0 1 1 0-0.1M49,36a1 1 0 1 1 0-0.1M57,55a2 3 0 1 1 0-0.1M12,94c0,0,20-4,42,0c0,0,27-4,39,0z" -properties {{fill.color #000} {stroke.visible 0}}]
set path9 [create_charts_path_annotation -plot $plot -id nine -group $group \
  -path "M50,59c0,0,4,3,10,0M56,66l2,12l-2,12M25,50c0,0,10,12,23,12c13,0,24,0,35-15" -properties {{fill.visible 0} {stroke.color #000} {stroke.width 0.5}}]

connect_charts_signal -plot $plot -from annotationPressed -to annotationSlot
