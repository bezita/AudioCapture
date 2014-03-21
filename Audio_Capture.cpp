#include<windows.h>
#include<stdio.h>
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")

#define BUFFER_SIZE (44100*16*2/8*30)
#define FRAGMENT_SIZE 1024
#define FRAGMENT_NUM  4 

static unsigned char buffer[BUFFER_SIZE] = {0};
static int buf_count = 0;

void CALLBACK waveInProc(HWAVEIN hwi,
			UINT uMsg,
			DWORD_PTR dwInstance,
			DWORD_PTR dwParam1,
			DWORD_PTR dwParam2);
void CALLBACK waveOutPro(HWAVEOUT hwo,
			UINT uMsg,
			DWORD_PTR dwInstance,
			DWORD_PTR dwParam1,
			DWORD_PTR dwParam2);

FILE *fp;

int main()
{
	if(fp = fopen("123.pcm","ab+"));
		puts("open the file success.\n");
	else
		puts("open the file failure.\n");
	//Device
	int nReturn = waveInGetNumDevs();
	printf("Device Num:%d\n",nReturn);
	for(int i = 0;i < nReturn;i++)
	{
		WAVEINCAPS wic;
		waveinGetDevCaps(i,&wic,sizeof(WAVEINCAPS));
		printf("%d\t Device Name\n",wic.szPname);
	}
	
	//Open
	HWAVEIN hWaveIn;
        WAVEFORMATEX wavform;  
    	wavform.wFormatTag = WAVE_FORMAT_PCM;  //采样格式只支持PCM 
    	wavform.nChannels = 2;                 //通道数目
    	wavform.nSamplesPerSec = 44100;        //采样速率
   	wavform.nAvgBytesPerSec = 44100*16*2/8;  //每秒采样得到的数据
    	wavform.nBlockAlign = 4;              //记录区块对齐的单位
    	wavform.wBitsPerSample = 16;         
    	wavform.cbSize = 0;  
  
    	waveInOpen(&hWaveIn, WAVE_MAPPER, &wavform, (DWORD_PTR)waveInProc, 0, CALLBACK_FUNCTION);  //这里是回调函数
  
    	WAVEINCAPS wic;  
    	waveInGetDevCaps((UINT_PTR)hWaveIn, &wic, sizeof(WAVEINCAPS));  
    	printf("打开的输入设备：%s\n", wic.szPname);  
  
    	// prepare buffer   
    	static WAVEHDR wh[FRAGMENT_NUM];   //FRAGMENT_NUM 录制声音长度 (44100*16*2/8*30)   WAVEHDR 采集音频时包含数据缓存的结构体
    	for (int i=0; i<FRAGMENT_NUM; i++)  
    	{  
        	wh[i].lpData = new char[FRAGMENT_SIZE];  
        	wh[i].dwBufferLength = FRAGMENT_SIZE;  
        	wh[i].dwBytesRecorded = 0;  
        	wh[i].dwUser = NULL;  
        	wh[i].dwFlags = 0;  
        	wh[i].dwLoops = 1;  
        	wh[i].lpNext = NULL;  
        	wh[i].reserved = 0;  
        	//将建立好的wHdr1做为备用
        	waveInPrepareHeader(hWaveIn, &wh[i], sizeof(WAVEHDR));  
        	//将两个wHdr添加到waveIn中去
		waveInAddBuffer(hWaveIn, &wh[i], sizeof(WAVEHDR));  
    	}  
  
    // record   
    printf("Start to Record...\n");  
  
    buf_count = 0; 
    //开始音频采集
    waveInStart(hWaveIn);  
  
    while (buf_count < BUFFER_SIZE)  
    {  
        Sleep(1);  
    }  
  
    printf("Record Over!\n\n");  
  
    // clean   
    waveInStop(hWaveIn);  
    waveInReset(hWaveIn);  
    for (int i=0; i<FRAGMENT_NUM; i++)  
    {  
        waveInUnprepareHeader(hWaveIn, &wh[i], sizeof(WAVEHDR));  
        delete wh[i].lpData;  
    }  
    waveInClose(hWaveIn);  
  
    system("pause");  // 暂停直到任意键按下去
    printf("\n");  
  
/*--------------------- 放音--------------------------- */  
      
    // Device   
    nReturn = waveOutGetNumDevs();  
    printf("\n输出设备数目：%d\n", nReturn);  
    for (int i=0; i<nReturn; i++)  
    {  
        WAVEOUTCAPS woc;  
        waveOutGetDevCaps(i, &woc, sizeof(WAVEOUTCAPS));  
        printf("#%d\t设备名：%s\n", i, wic.szPname);  
    }  
  
    // open   
    HWAVEOUT hWaveOut;  //输出设备
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wavform, (DWORD_PTR)waveOutProc, 0, CALLBACK_FUNCTION);  //回调函数
  
    WAVEOUTCAPS woc;  
    waveOutGetDevCaps((UINT_PTR)hWaveOut, &woc, sizeof(WAVEOUTCAPS));  
    printf("打开的输出设备：%s\n", wic.szPname);  
  
    // prepare buffer   
    WAVEHDR wavhdr;  //输出缓冲
    wavhdr.lpData = (LPSTR)buffer;  //将buffer 作为中介全局变量啊 
    wavhdr.dwBufferLength = BUFFER_SIZE;  
    wavhdr.dwFlags = 0;  
    wavhdr.dwLoops = 0;  
  
    //将建立好的WAVEHDR 作为备用
    waveOutPrepareHeader(hWaveOut, &wavhdr, sizeof(WAVEHDR));  
  
    // play   
    printf("Start to Play...\n");  
  
    buf_count = 0; 

    //开始播放
    waveOutWrite(hWaveOut, &wavhdr, sizeof(WAVEHDR));  
    while (buf_count < BUFFER_SIZE)  
    {  
        Sleep(1);  
    }  
  
    //clean   
    waveOutReset(hWaveOut); 

    waveOutUnprepareHeader(hWaveOut, &wavhdr, sizeof(WAVEHDR));  
    waveOutClose(hWaveOut);  
  
    printf("Play Over!\n\n");  
  
    return 0;  
}  
  
// 录音回调函数  
// 这个是callback函数，对于采集到的音频数据都在这个函数中处理
void CALLBACK waveInProc(HWAVEIN hwi,  //输入设备      
                         UINT uMsg,    //消息      
                         DWORD_PTR dwInstance,    
                         DWORD_PTR dwParam1,    
                         DWORD_PTR dwParam2     )  
{  
    LPWAVEHDR pwh = (LPWAVEHDR)dwParam1;  
     
//这里对采集到的数据做处理 
    if ((WIM_DATA==uMsg) && (buf_count < BUFFER_SIZE))  
    {

        int temp = BUFFER_SIZE - buf_count;  
        // pwh->dwBytesRecorded 这就是采集到的数据长度

        temp = (temp>pwh->dwBytesRecorded) ? pwh->dwBytesRecorded : temp;    
        memcpy(buffer+buf_count, pwh->lpData, temp);  
        buf_count += temp;  
        fwrite(pwh->lpData,sizeof(char),temp,fp);
        // pwh->lpData 是数据 temp是数据长度


//printf("length:%d\n",temp);
        //处理完之后还要再把这个缓冲数组加回去
// pWnd->win 代表是否继续采集因为当停止采集的时候，只有当所有的
// 缓冲数组都腾出来之后才能close 所以需要停止采集时就不要waveInAddBuffer了
        waveInAddBuffer(hwi, pwh, sizeof(WAVEHDR)); 

    }  

}  
  
// 放音回调函数  
void CALLBACK waveOutProc(  HWAVEOUT hwo,     
                          UINT uMsg,           
                          DWORD_PTR dwInstance,     
                          DWORD_PTR dwParam1,      
                          DWORD_PTR dwParam2     )  
{  
    fclose(fp);
    printf("close file\n");
    if (WOM_DONE == uMsg)  
    {  
        buf_count = BUFFER_SIZE;  
    }  
	
}
