all: picture_lib concurrent_picture_lib blur_opt_exprmt picture_compare

picture_lib: SeqMain.o Utils.o Picture.o PicProcess.o
	gcc sod_118/sod.c SeqMain.o Utils.o Picture.o PicProcess.o -I sod_118 -lm -o picture_lib

concurrent_picture_lib: ConcMain.o Utils.o Picture.o PicProcess.o PicStore.o
	gcc sod_118/sod.c ConcMain.o Utils.o Picture.o PicProcess.o PicStore.o -I sod_118 -lm -lpthread -o concurrent_picture_lib	

blur_opt_exprmt: BlurExprmt.o Utils.o Picture.o PicProcess.o
	gcc sod_118/sod.c BlurExprmt.o Utils.o Picture.o PicProcess.o -I sod_118 -lm -lpthread -o blur_opt_exprmt

picture_compare: Compare.o Utils.o Picture.o
	gcc sod_118/sod.c Compare.o Utils.o Picture.o -I sod_118 -lm -o picture_compare

Utils.o: Utils.h Utils.c

Picture.o: Utils.h Picture.h Picture.c

PicProcess.o: Utils.h Picture.h PicProcess.h PicProcess.c

SeqMain.o: SeqMain.c Utils.h Picture.h PicProcess.h

PicStore.o: Utils.h Picture.h PicStore.h PicStore.c

ConcMain.o: ConcMain.c Utils.h Picture.h PicProcess.h PicStore.h 

BlurExprmt.o: BlurExprmt.c Utils.h Picture.h PicProcess.h

Compare.o: Compare.c Utils.h Picture.h

%.o: %.c
	gcc -c -I sod_118 -lm -lpthread $<

clean:
	rm -rf picture_lib concurrent_picture_lib blur_opt_exprmt picture_compare *.o

.PHONY: all clean

