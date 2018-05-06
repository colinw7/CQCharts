CQChartsTest -ceil -exec coverage.cl -close_app

if (0) then
CQChartsTest -vertical -y1y2 \
 -csv data/coverage.csv -first_line_header \
 -column_type "0#time:format=%m/%d/%Y,oformat=%F" \
  -type xy -impulse -columns "id=0,x=0,y=1" \
  -properties "impulse.color=palette,impulse.alpha=0.5,impulse.width=20px" \
-and \
  -type xy -impulse -columns "id=0,x=0,y=2" \
  -properties "invertY=1" \
  -properties "impulse.color=palette#1,impulse.alpha=0.5,impulse.width=20px" \

CQChartsTest -vertical -overlay -y1y2 \
 -csv data/coverage.csv -first_line_header \
  -type barchart -columns "name=0,value=1" \
-and \
  -type barchart -columns "name=0,value=2"

CQChartsTest -overlay \
 -csv data/coverage.csv -first_line_header \
  -type xy -columns "name=0,value=1"
-and \
  -type xy -columns "name=0,value=2"
endif
