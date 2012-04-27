// OpenCV_Capture.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
void TemlateBit(IplImage* dest,IplImage* src){

}
void TemlateBit(IplImage* dest,const char *filename,CvPoint *ptDest,CvPoint *ptSrc){
	//IplImage *img;
	IplImage *tpl;
	IplImage *res;
	CvPoint minloc,maxloc;
	double minval,maxval;
	int img_width,img_height;
	int tpl_width,tpl_height;
	int res_width,res_height;
	if (0==dest)
	{
		return ;
	}
	tpl=cvLoadImage(filename,CV_LOAD_IMAGE_COLOR);
	if (0==tpl)
	{
		return ;
	}
	img_width=dest->width;
	img_height=dest->height;
	tpl_width=tpl->width;
	tpl_height=tpl->height;
	res_width=img_width-tpl_width+1;
	res_height=img_height-tpl_height+1;

	res=cvCreateImage(cvSize(res_width,res_height),IPL_DEPTH_32F,1);
	cvMatchTemplate(dest,tpl,res,CV_TM_SQDIFF);
	cvMinMaxLoc(res,&minval,&maxval,&minloc,&maxloc,0);
	//cvRectangle(dest,cvPoint(minloc.x/2,minloc.y/2),cvPoint(minloc.x/2+tpl_width/2,minloc.y/2+tpl_height/2),cvScalar(0,0,255,0),1,0,0);
	//ptDest=cvPoint(minloc.x,minloc.y);
	//ptSrc=cvPoint(minloc.x+tpl_width,minloc.y+tpl_height);
	ptDest->x=minloc.x;
	ptDest->y=minloc.y;
	ptSrc->x=minloc.x+tpl_width;
	ptSrc->y=minloc.y+tpl_height;
    //printf("ptDest.x=%d ptDest.y=%d \n ptSrc.x=%d ptSrc.y=%d \n",ptDest.x,ptDest.y,ptSrc.x,ptSrc.y);
	//cvNamedWindow("reference",CV_WINDOW_AUTOSIZE);
	//cvNamedWindow("template",CV_WINDOW_AUTOSIZE);
	//cvShowImage("reference",dest);
	//cvShowImage("template",tpl);

	//cvWaitKey(0);

	//cvDestroyWindow("reference");
	//cvDestroyWindow("template");
	//cvReleaseImage(&dest);
	cvReleaseImage(&tpl);
	cvReleaseImage(&res);

}
void CopyData(char *dest, const char *src, int dataByteSize,bool isConvert, int height)
{
	char * p = dest;
	if(!isConvert) {   
		memcpy(dest, src, dataByteSize);
		return;
	}
	if(height<=0) return;
	//int height = dataByteSize/rowByteSize;
	int rowByteSize = dataByteSize / height;
	src = src + dataByteSize - rowByteSize ;
	for(int i=0; i<height; i++) {
		memcpy(dest, src, rowByteSize);
		dest += rowByteSize;
		src  -= rowByteSize;
	}   
}

IplImage* GetDIBitsFormScreen(LPRECT lpRect)
{
	int pLeft=lpRect->left ,pWidth=lpRect->right-lpRect->left ,pHeight=lpRect->bottom-lpRect->top ,pTop=lpRect->top ;
	char * buff = NULL;
	HDC hDC = ::GetDC(NULL);
	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDC,pWidth,pHeight);
	SelectObject(hMemDC,hBitmap);
	BitBlt(hMemDC,0,0,pWidth,pHeight,hDC,pLeft,pTop,SRCCOPY);
	BITMAP bmp;
	GetObject(hBitmap,sizeof(BITMAP),&(bmp));
	BITMAPINFO bi;
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = pWidth;
	bi.bmiHeader.biHeight = pHeight;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = 0;
	DWORD dwSize = bmp.bmHeight* bmp.bmWidthBytes;
	buff = new char[dwSize]; 
	GetDIBits(hDC,hBitmap,0,pHeight ,buff,&bi,DIB_RGB_COLORS);
	//SetDIBits(hDC,hBitmap,0,pHeight,buff,&bi,DIB_RGB_COLORS);
	//转IplImage
	IplImage* iplImg;
	int height;
	bool isLowerLeft=bi.bmiHeader.biHeight >0 ;
	height=(bi.bmiHeader.biHeight > 0) ? bi.bmiHeader.biHeight : -bi.bmiHeader.biHeight ;
	iplImg = cvCreateImage( cvSize(bi.bmiHeader.biWidth, pHeight), IPL_DEPTH_8U, bi.bmiHeader.biBitCount / 8);
	//iplImg->imageData = buff;  //这样的图像是倒着的,可以把bi.bmiHeader.biHeight设成负数,然后就可以不要下面的CopyData部分了
	CopyData( iplImg->imageData, (char*)buff, bi.bmiHeader.biSizeImage,isLowerLeft, height);
	::ReleaseDC(NULL,hDC);
	DeleteDC(hMemDC);
	free(buff);
	return iplImg;
}
int image_width;
int image_height;
int image_depth;
int image_nchannels;

IplImage* screemImage;
int flag=0;

//截屏函数
void CopyScreenToBitmap()
{
	int right=GetSystemMetrics(SM_CXSCREEN),left=0,top=0,bottom=GetSystemMetrics(SM_CYSCREEN);//定义截屏范围此处设为全屏

	int nWidth, nHeight;

	HDC hSrcDC = NULL, hMemDC = NULL;
	HBITMAP hBitmap = NULL, hOldBitmap = NULL;

	hSrcDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);
	hMemDC = CreateCompatibleDC(hSrcDC);

	nWidth = right - left;
	nHeight = bottom - top;

	hBitmap = CreateCompatibleBitmap(hSrcDC, nWidth, nHeight);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hSrcDC, left, top , SRCCOPY);
	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);

	BITMAP bmp;
	int nChannels,depth;
	BYTE *pBuffer;

	GetObject(hBitmap,sizeof(BITMAP),&bmp);

	image_nchannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel/8 ;
	image_depth = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;
	image_width=bmp.bmWidth;
	image_height=bmp.bmHeight;

	if(flag==0)
	{
		screemImage=cvCreateImage(cvSize(image_width,image_height),image_depth,image_nchannels);
		flag=1;
	}

	pBuffer = new BYTE[image_width*image_height*image_nchannels];
	GetBitmapBits(hBitmap,image_height*image_width*image_nchannels,pBuffer);
	memcpy(screemImage->imageData,pBuffer,image_height*image_width*image_nchannels);

	delete pBuffer;

	SelectObject(hMemDC,hOldBitmap);
	DeleteObject(hOldBitmap);
	DeleteDC(hMemDC);

	SelectObject(hSrcDC,hBitmap);
	DeleteDC(hMemDC);
	DeleteObject(hBitmap);
}
int _tmain(int argc, _TCHAR* argv[])
{
/*
	CvCapture *capture=0;
	IplImage *frame=0;
	
	int right=GetSystemMetrics(SM_CXSCREEN);
	int left=0;
	int top=0;
	int bottom=GetSystemMetrics(SM_CYSCREEN);
	RECT rc={left,top,right,bottom};
    frame=GetDIBitsFormScreen(&rc);
	cvNamedWindow("frame",CV_WINDOW_AUTOSIZE);
    cvShowImage("frame",frame);

	cvWaitKey(0);

	cvDestroyWindow("frame");
	cvReleaseImage(&frame);
	//_CrtDumpMemoryLeaks();
	return 0;*/
	IplImage* screenRGB=0;
	IplImage* screen_resize=0;
	time_t begin,end; 

	while(1)
	{
		begin=clock();
		CopyScreenToBitmap(); //得到的图片为RGBA格式,即4通道。

		if(!screen_resize)
			screen_resize=cvCreateImage(cvSize(1600,900),image_depth,image_nchannels);
		cvResize(screemImage,screen_resize,CV_INTER_LINEAR);

		if(!screenRGB)
			screenRGB=cvCreateImage(cvSize(1600,900),IPL_DEPTH_8U,3);
		cvCvtColor(screen_resize,screenRGB,CV_RGBA2RGB);

		//cvShowImage("s_laplace",screenRGB);
		CvPoint ptDest;
		CvPoint ptSrc;
		ptDest.x=0;ptDest.y=0;
		ptSrc.x=0;ptSrc.y=0;
		TemlateBit(screenRGB,"temimage.jpg",&ptDest,&ptSrc);
		end=clock();
		printf("ptDest.x=%d ptDest.y=%d \n ptSrc.x=%d ptSrc.y=%d \n",ptDest.x,ptDest.y,ptSrc.x,ptSrc.y);
		printf("runtime:%1.3f \n",double(end-begin)/CLOCKS_PER_SEC);
		HWND h=GetDesktopWindow();
		HDC hdc=GetWindowDC(h);
		
		
		//Rectangle(hdc,ptDest.x-1,ptDest.y-1,ptSrc.x+1,ptSrc.y+1);
		MoveToEx(hdc,ptDest.x-1,ptDest.y-1,NULL);
		LineTo(hdc,ptSrc.x+1,ptDest.y-1);
		LineTo(hdc,ptSrc.x+1,ptSrc.y+1);
		LineTo(hdc,ptDest.x-1,ptSrc.y+1);
		LineTo(hdc,ptDest.x-1,ptDest.y-1);

		ReleaseDC(h,hdc);

		cvWaitKey(10);
	}
	cvDestroyAllWindows();
	return 0;
}

