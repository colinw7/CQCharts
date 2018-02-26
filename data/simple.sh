CQChartsTest \
 -expr -num_rows 50 \
 -process "+map(-10,10)" \
 -process "+sin(x)=sin(@1)" \
 -process "+atan(x)=atan(@1)" \
 -process "+cos(atan(x))=cos(atan(@1))" \
 -type xy -columns "x=1,y=2 3 4" \
 -plot_title "Simple Plots" \
 -properties "points.visible=0,dataStyle.stroke.visible=1" \
 -properties "xaxis.ticks.inside=1,xaxis.line.visible=0,xaxis.ticks.mirror=1" \
 -properties "yaxis.ticks.inside=1,yaxis.line.visible=0,yaxis.ticks.mirror=1" \
 -properties "key.location=tl,key.flipped=1" \
 -properties "title.text.font=+8" \
 -close_app

CQChartsTest \
 -expr -num_rows 100 \
 -process "+map(-pi/2,pi)" \
 -process "+cos(x)=cos(@1)" \
 -process "+-(sin(x)>sin(x+1)?sin(x):sin(x+1))=-(sin(@1)>sin(@1+1)?sin(@1):sin(@1+1))" \
 -type xy -columns "x=1,y=2 3" \
 -plot_title "Simple Plots" \
 -properties "points.visible=0,dataStyle.stroke.visible=1" \
 -properties "xaxis.ticks.inside=1,xaxis.line.visible=0,xaxis.ticks.mirror=1" \
 -properties "yaxis.ticks.inside=1,yaxis.line.visible=0,yaxis.ticks.mirror=1" \
 -properties "key.flipped=1,key.border.visible=0" \
 -properties "title.text.font=+8" \
 -close_app

CQChartsTest \
 -expr -num_rows 200 \
 -process "+map(-3,5)" \
 -process "+asin(x)=asin(@1)" \
 -process "+acos(x)=acos(@1)" \
 -type xy -columns "x=1,y=2 3" \
 -xmin -3 -xmax 5 \
 -plot_title "Simple Plots" \
 -properties "points.visible=0,dataStyle.stroke.visible=1" \
 -properties "xaxis.ticks.inside=1,xaxis.line.visible=0,xaxis.ticks.mirror=1" \
 -properties "yaxis.ticks.inside=1,yaxis.line.visible=0,yaxis.ticks.mirror=1" \
 -properties "key.location=tl,key.flipped=1" \
 -properties "title.text.font=+8" \
 -close_app

CQChartsTest \
 -expr -num_rows 500 \
 -process "+map(-5*pi,5*pi)" \
 -process "+tan(x)/atan(x)=tan(@1)/atan(@1)" \
 -process "+1/x=1/@1" \
 -type xy -columns "x=1,y=2 3" \
 -plot_title "Simple Plots" \
 -xmin -16 -xmax 16 -ymin -5 -ymax 5 \
 -properties "points.visible=0" \
 -properties "key.location=bc,key.insideY=0" \
 -properties "title.text.font=+8" \
 -close_app

CQChartsTest \
 -expr -num_rows 800 \
 -process "+map(-30,20)" \
 -process "+sin(x*20)*atan(x)=sin(@1*20)*atan(@1)" \
 -type xy -columns "x=1,y=2" \
 -plot_title "Simple Plots" \
 -properties "points.visible=0" \
 -properties "key.location=bl,key.insideY=0,key.flipped=1" \
 -properties "title.text.font=+8" \
 -close_app
