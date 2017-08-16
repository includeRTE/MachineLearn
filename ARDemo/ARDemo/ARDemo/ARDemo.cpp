// ARDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include<Windows.h>
#include<GL/GL.h>
#include<GL/glut.h>
#include <AR/ar.h>
#include <AR/gsub.h>
#include <AR/video.h>

#include <stdio.h>
#include<stdlib.h>
#include<string.h>

ARHandle           *arHandle;
ARPattHandle       *arPattHandle;
AR3DHandle         *ar3DHandle;
ARGViewportHandle  *vp;
int                 xsize, ysize;
int                 flipMode = 0;
int                 patt_id;
double              patt_width = 80.0;
int                 count = 0;
char                fps[256];
char                errValue[256];
int                 distF = 0;
int                 contF = 0;
ARParamLT          *gCparamLT = NULL;

#define             CPARA_NAME       "Data/camera_para.dat"
#define             VPARA_NAME       "Data/cameraSetting-%08x%08x.dat"
#define             PATT_NAME        "Data/hiro.patt"

static void init(int argc, char* argv[]);
static void mainLoop();
static void draw(ARdouble trans[3][4]);
static void keyFunc(unsigned char key, int x, int y);
static void cleanup();
int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	init(argc, argv);

	argSetDispFunc(mainLoop, 1);
	argSetKeyFunc(keyFunc);
	count = 0;
	fps[0] = '\0';
	arUtilTimerReset();
	argMainLoop();
    return 0;
}

static void init(int argc, char* argv[])
{
	ARParam           cparam;
	ARGViewport       viewport;
	char              vconf[512];
	AR_PIXEL_FORMAT   pixFormat;
	ARUint32          id0, id1;
	int               i;
	if (argc == 1) vconf[0] = '\0';
	else {
		strcpy(vconf, argv[1]);
		for (i = 2; i < argc; i++)
		{
			strcat(vconf, " ");
			strcat(vconf, argv[i]);
		}
	}

	if (arVideoOpen(vconf) < 0) exit(0);
	if (arVideoGetSize(&xsize, &ysize) < 0) exit(0);
	if ((pixFormat = arVideoGetPixelFormat()) < 0) exit(0);
	if (arVideoGetId(&id0, &id1) == 0)
	{
		sprintf(vconf, VPARA_NAME, id1, id0);
		if (arVideoLoadParam(vconf) < 0) {
			ARLOGe("No camera setting data!!\n");
		}
	}

	if (arParamLoad(CPARA_NAME, 1, &cparam) < 0)
	{
		exit(0);
	}

	arParamChangeSize(&cparam, xsize, ysize, &cparam);
	arParamDisp(&cparam);
	if ((gCparamLT = arParamLTCreate(&cparam, AR_PARAM_LT_DEFAULT_OFFSET)) == NULL)
	{
		exit(-1);
	}

	if ((arHandle = arCreateHandle(gCparamLT)) == NULL)
	{
		exit(0);
	}

	if (arSetPixelFormat(arHandle,pixFormat) < 0)
	{
		exit(0);
	}

	if ((ar3DHandle = ar3DCreateHandle(&cparam)) == NULL)
	{
		exit(0);
	}
	if ((arPattHandle = arPattCreateHandle()) == 0)
	{
		exit(0);
	}

	if ((patt_id = arPattLoad(arPattHandle, PATT_NAME)) < 0) {
		ARLOGe("pattern load error !!\n");
		exit(0);
	}

	arPattAttach(arHandle, arPattHandle);
	viewport.sx = 0;
	viewport.sy = 0;
	viewport.xsize = xsize;
	viewport.ysize = ysize;
	if ((vp = argCreateViewport(&viewport)) == NULL)
	{
		exit(0);
	}

	argViewportSetCparam(vp, &cparam);
	argViewportSetPixFormat(vp, pixFormat);
	argViewportSetDistortionMode(vp, AR_GL_DISTORTION_COMPENSATE_DISABLE);
	if (arVideoCapStart() != 0)
	{
		exit(0);
	}
}

static void mainLoop()
{
	static int         contF2 = 0;
	static ARdouble    patt_trans[3][4];
	static ARUint8*    dataPtr = NULL;
	ARMarkerInfo*      markerInfo;
	int                markerNum;
	ARdouble           err;
	int                imageProcMode;
	int                debugMode;
	int                j, k;

	//dataPtr = arVideoGetImage();

	if ((dataPtr = (ARUint8 *)arVideoGetImage()) == NULL) {
		arUtilSleep(2);
		return;
	}
	argDrawMode2D(vp);
	if (arGetDebugMode(arHandle, &debugMode) < 0)
	{
		exit(0);
	}
	if (debugMode == 0) {
		argDrawImage(dataPtr);
	}
	else {
		arGetImageProcMode(arHandle, &imageProcMode);
		if (imageProcMode == AR_IMAGE_PROC_FRAME_IMAGE) {
			argDrawImage(arHandle->labelInfo.bwImage);
		}
		else {
			argDrawImageHalf(arHandle->labelInfo.bwImage);
		}
	}

	if (arDetectMarker(arHandle, dataPtr) < 0)
	{
		exit(0);
	}

	if (count % 60 == 0)
	{
		sprintf(fps, "%f[fps]", 60.0 / arUtilTimer());
		arUtilTimerReset();
	}

	count++;
	glColor3f(0.0f, 1.0f, 0.0f);
	argDrawStringsByIdealPos(fps, 10, ysize - 30);
	markerNum = arGetMarkerNum(arHandle);
	if (markerNum == 0)
	{
		argSwapBuffers();
		return;
	}

	markerInfo = arGetMarker(arHandle);
	k = -1;
	for (j = 0; j < markerNum;j++)
	{
		if (patt_id == markerInfo[j].id)
		{
			if (k == -1)
			{
				if (markerInfo[j].cf >= 0.7)
				{
					k = j;
				}
			}
			else if (markerInfo[j].cf > markerInfo[k].cf)
			{
				k = j;
			}
		}
	}

	if (k == -1)
	{
		contF2 = 0;
		argSwapBuffers();
		return;
	}
	if (contF && contF2)
	{
		err = arGetTransMatSquareCont(ar3DHandle, &(markerInfo[k]), patt_trans, patt_width, patt_trans);
	}
	else
	{
		err = arGetTransMatSquare(ar3DHandle, &(markerInfo[k]), patt_width, patt_trans);
	}

	sprintf(errValue, "err = %f", err);
	glColor3f(0.0f, 1.0f, 0.0f);
	argDrawStringsByIdealPos(fps, 10, ysize - 30);
	argDrawStringsByIdealPos(errValue, 10, ysize - 60);

	contF2 = 1;
	draw(patt_trans);
	argSwapBuffers();
}

static void   draw(ARdouble trans[3][4])
{
	ARdouble  gl_para[16];
	GLfloat   mat_diffuse[] = { 0.0f, 0.0f, 1.0f, 0.0f };
	GLfloat   mat_flash[] = { 1.0f, 1.0f, 1.0f, 0.0f };
	GLfloat   mat_flash_shiny[] = { 50.0f };
	GLfloat   light_position[] = { 100.0f,-200.0f,200.0f,0.0f };
	GLfloat   light_ambi[] = { 0.1f, 0.1f, 0.1f, 0.0f };
	GLfloat   light_color[] = { 1.0f, 1.0f, 1.0f, 0.0f };

	argDrawMode3D(vp);
	glClearDepth(1.0);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	argConvGlpara(trans, gl_para);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(gl_para);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambi);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_color);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_diffuse);

#if 1
	glTranslatef(0.0f, 0.0f, 40.0f);
	glutSolidCube(80.0);
#else
	glTranslatef(0.0f, 0.0f, 20.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glutSolidTeapot(40.0);
#endif
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);

	glDisable(GL_DEPTH_TEST);
}

static void cleanup()
{
	arVideoCapStop();
	argCleanup();
	arPattDetach(arHandle);
	arPattDeleteHandle(arPattHandle);
	ar3DDeleteHandle(&ar3DHandle);
	arDeleteHandle(arHandle);
	arParamLTFree(&gCparamLT);
	arVideoClose();
}

static void keyFunc(unsigned char key, int x, int y)
{
	int value;

	switch (key)
	{
	case 0x1b:
		cleanup();
		exit(0);
		break;
	case '1':
		arGetLabelingThresh(arHandle, &value);
		value -= 5;
		if (value < 0) value = 0;
		arSetLabelingThresh(arHandle, value);
		break;
	case '2':
	case '+':
		arGetLabelingThresh(arHandle, &value);
		value += 5;
		if (value > 255) value = 255;
		arSetLabelingThresh(arHandle, value);
		break;
	case 'd':
	case 'D':
		arGetDebugMode(arHandle, &value);
		value = 1 - value;
		arSetDebugMode(arHandle, value);
		break;
	case 'h':
	case 'H':
		if (flipMode & AR_GL_FLIP_H)
			flipMode = flipMode & AR_GL_FLIP_V;
		else
			flipMode = flipMode & AR_GL_FLIP_H;
		argViewportSetFlipMode(vp, flipMode);
	case 'v':
	case 'V':
		if (flipMode & AR_GL_FLIP_V)
			flipMode = flipMode & AR_GL_FLIP_H;
		else
			flipMode = flipMode & AR_GL_FLIP_V;
		argViewportSetFlipMode(vp, flipMode);
		break;
	case 'c':
		contF = 1 - contF;
		break;
	case '?':
	case '/':
		arVideoDispOption();
		break;
	default:
		break;
	}
}