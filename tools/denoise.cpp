#include <cstdio>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <cmath>
extern "C" {
  void* oidnNewDevice(int);
  void oidnSetDeviceInt(void*,const char*,int);
  void oidnCommitDevice(void*);
  int oidnGetDeviceError(void*,const char**);
  void* oidnNewFilter(void*,const char*);
  void oidnSetSharedFilterImage(void*,const char*,void*,int,size_t,size_t,size_t,size_t,size_t);
  void oidnSetFilterBool(void*,const char*,bool);
  void oidnCommitFilter(void*);
  void oidnExecuteFilter(void*);
  void oidnReleaseFilter(void*);
  void oidnReleaseDevice(void*);
}
int main(int argc,char**argv){
  if(argc!=3)return 1;
  const int w=1600,h=900;const size_t count=w*h*3;
  std::vector<float> rgb(count),out(count);std::vector<unsigned char> bytes(count);
  void* d=oidnNewDevice(1);
  auto check=[&](){const char* msg=nullptr;if(oidnGetDeviceError(d,&msg)){fprintf(stderr,"%s\n",msg);exit(1);}};
  check();oidnSetDeviceInt(d,"numThreads",4);oidnCommitDevice(d);check();
  void* f=oidnNewFilter(d,"RT");check();
  oidnSetSharedFilterImage(f,"color",rgb.data(),3,w,h,0,0,0);
  oidnSetSharedFilterImage(f,"output",out.data(),3,w,h,0,0,0);
  oidnSetFilterBool(f,"srgb",true);oidnCommitFilter(f);check();
  char path[1024];while(fgets(path,sizeof(path),stdin)){
    int frame=atoi(path);snprintf(path,sizeof(path),"%s/f_%05d.ppm",argv[1],frame);
    FILE* in=fopen(path,"rb");if(!in)return 2;
    char line[128];for(int k=0;k<3;k++)if(!fgets(line,sizeof(line),in))return 3;
    if(fread(bytes.data(),1,count,in)!=count)return 4;fclose(in);
    for(size_t j=0;j<count;j++)rgb[j]=bytes[j]/255.f;
    oidnExecuteFilter(f);check();
    for(size_t j=0;j<count;j++)bytes[j]=(unsigned char)std::clamp(lroundf((.95f*out[j]+.05f*rgb[j])*255),0L,255L);
    snprintf(path,sizeof(path),"%s/f_%05d.ppm",argv[2],frame);
    FILE* dest=fopen(path,"wb");if(!dest)return 5;fprintf(dest,"P6\n%d %d\n255\n",w,h);
    if(fwrite(bytes.data(),1,count,dest)!=count)return 6;fclose(dest);
  }
  oidnReleaseFilter(f);oidnReleaseDevice(d);
}
