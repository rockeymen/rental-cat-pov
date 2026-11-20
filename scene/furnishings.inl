// Distances in metres. The mattress is draped in its own continuous quilt surface.
void makeBed(int timber,int piping,int ivory){
  const float x=-1.77f,z=.07f;
  int duvet=mat(Weave,{.245,.33,.365}), stitch=mat(Weave,{.21,.28,.31});
  for(float dx:{-.59f,.59f})for(float dz:{-.91f,.91f})
    box({x+dx,.14f,z+dz},{.067f,.28f,.067f},timber);
  box({x,.246f,z},{1.35f,.11f,2.04f},timber);
  soft(room,{x,.345f,z},{.666f,.10f,1.009f},ivory,.17f);
  for(float dx:{-.62f,.62f})box({x+dx,.51f,z-.98f},{.065f,.64f,.061f},timber);
  caneBed(x,z-.98f,timber);
  for(int i=0;i<2;i++){
    V3 c(x-.307f+i*.608f,.493f,z-.728f);
    M34 tilt=mRotY(.078f-i*.135f)*mRotX(-.105f);
    soft(room,c,{.295,.074,.181},ivory,.57,tilt);
    std::vector<V3> hem;
    for(int j=0;j<=90;j++){float a=2*PI*j/90;hem.push_back(c+tilt.dir({.294f*sp(cosf(a),.57f),0,.18f*sp(sinf(a),.57f)}));}
    cord(room,hem,.003f,piping);
  }
  auto quilt=[=](float u,float v){
    float end=std::max(0.f,(v-.89f)/.11f),side=std::max(0.f,(fabsf(u)-.84f)/.16f);
    float fold=.018f*sinf(14*u+6*v+.9f*sinf(v*8))+.010f*cosf(u*33-v*8)*(.3f+.7f*v);
    float line=std::min(fabsf(sinf((u+.03f)*11.8f)),fabsf(sinf(v*18.f)));
    return V3(x+.67f*clampf(u/.84f,-1,1)+sp(u,1)*.044f*side+.01f*sinf(v*20)*side,
      .467f+fold+.016f*line-.185f*side-.135f*end-.036f*expf(-sq((v-.13f)*28)),
      z-.52f+1.56f*std::min(v/.89f,1.f)+.032f*end+.018f*sinf(u*6+v*2));};
  addGrid(room,152,136,-1,1,0,1,quilt,duvet);
  for(int j=-3;j<=3;j++){std::vector<V3> p;for(int i=0;i<=90;i++)p.push_back(quilt(j*.25f,i/90.f)+V3(0,.001,0));cord(room,p,.0008f,stitch,4);}
}
#include "goldfish.inl"
void makeTank(int timber,int dark){
  const float x=1.55f,z=-.85f,b=.34f,wid=1.04f,dep=.43f,top=.847f;
  int pane=s.addMat(49,{.96,.99,.99},.008,{0,0,0},0,{0,0,0},0,0,0,0,0,0,.98);
  int water=s.addMat(50,{.82,.93,.90},.018,{0,0,0},0,{0,0,0},0,0,0,0,0,0,.97);
  int back=mat(60,{.019,.034,.026}),edge=mat(Paint,{.14,.30,.245},.2);
  for(float d:{-.46f,.46f})for(float e:{-.16f,.16f})box({x+d,.166f,z+e},{.042f,.332f,.042f},timber);
  box({x,b-.019f,z},{1.09f,.037f,.47f},timber);
  box({x,b+.011f,z},{wid,.021f,dep},dark);box({x,.59f,z-dep/2},{wid,.50f,.007f},back);
  addGrid(room,1,1,-wid/2,wid/2,b,top,[=](float u,float v){return V3(x+u,v,z+dep/2);},pane);
  for(int d:{-1,1}){
    addGrid(room,1,1,-dep/2,dep/2,b,top,[=](float u,float v){return V3(x+d*wid/2,v,z+u);},pane);
    for(int e:{-1,1})rod({x+d*wid/2,b,z+e*dep/2},{x+d*wid/2,top,z+e*dep/2},.0022f,edge);
    rod({x+d*wid/2,top,z-dep/2},{x+d*wid/2,top,z+dep/2},.0026f,edge);
  }
  for(int e:{-1,1})rod({x-wid/2,top,z+e*dep/2},{x+wid/2,top,z+e*dep/2},.0025f,edge);
  addGrid(room,75,42,-.517f,.517f,-.211f,.211f,[=](float u,float v){return V3(x+u,top-.025f+.0018f*sinf(41*u+18*v)*cosf(v*33-u*7),z+v);},water);
  for(float dx:{-.42f,.42f})rod({x+dx,top,z-.199f},{x+dx,.88f,z-.04f},.005f,dark);
  box({x,.884f,z-.04f},{.889f,.019f,.052f},dark);
  int gravel=mat(60,{.17,.124,.071});
  box({x,b+.030f,z},{1.03f,.040f,.42f},gravel);
  int stones[7];for(int j=0;j<7;j++)stones[j]=mat(60,V3(.24,.173,.112)*rng.r(.37,1.32),.44);
  for(int i=0;i<4500;i++){
    float r=rng.r(.0029,.010),xx=rng.r(-.50,.50),zz=rng.r(-.201,.201);
    addBlob(room,{x+xx,b+.049f+r*.44f+.009f*sinf(xx*12)*cosf(zz*8),z+zz},{r,r*.6f,r*.84f},6,4,.16f,3,i*.71f,stones[i%7]);}
  for(int i=0;i<15;i++){float xx=i<9?-.30f:.32f,r=rng.r(.018,.052);addBlob(room,{x+xx+rng.r(-.11,.10),.411f,z+rng.r(-.13,.05)},{r,r*.78f,r*.84f},12,8,.21,3,i,stones[i%7]);}
  tankGarden(x,z);
  int bark=MT.barkDark;
  for(int j=0;j<7;j++){V3 p(x-.41f,.395f,z-.03f),e(x-.12f+j*.037f,.46f+.14f*fabsf(sinf(j*8)),z-.13f+.065f*sinf(j));
    auto branch=curvePts(p,lerp(p,e,.42f)+V3(.005,.06,0),lerp(p,e,.72f)-V3(.025,.04,.01),e,18);
    addTube(room,branch,[=](float t){return (.009f+j*.0008f)*(.04f+.96f*(1-t));},9,bark);}
  int plant[6];for(int i=0;i<6;i++)plant[i]=mat(54,V3(.104,.245,.046)*rng.r(.56,1.24));
  for(int j=0;j<14;j++){
    float dx=rng.r(-.49,.49),zz=rng.r(-.18,-.07),h=rng.r(.13,.38);
    if(fabsf(dx)<.19f)h*=.50f;
    for(int k=0;k<8;k++){float a=rng.r(0,6.28),l=h*rng.r(.52,1),rr=rng.r(.024,.077),ww=rng.r(.005,.013);
      addGrid(room,5,18,-1,1,0,1,[=](float u,float v){float r=rr*v*v,ang=a+v*.89f,width=ww*powf(std::max(.00001f,sinf(PI*clampf(v,0,1))),.63f);
        return V3(x+dx+r*cosf(ang)+width*u*sinf(ang),.397f+l*v-.002f*u*u,z+zz+r*sinf(ang)-width*u*cosf(ang));},MT.irisLeaf);}}
  // Broad-leaved Anubias: individual petioles branch from horizontal rhizomes.
  for(int j=0;j<9;j++){
    float bx=x+(j%2?.36f:-.34f)+rng.r(-.105,.10), bz=z+rng.r(-.008,.10);
    for(int k=0;k<7;k++){
      float a=k*2.4f+j*.69f,h=rng.r(.029,.091), l=rng.r(.045,.096), ww=l*rng.r(.22,.37);
      V3 root(bx,.408f,bz),axis(cosf(a),0,sinf(a)),lat(-sinf(a),0,cosf(a));
      V3 base=root+axis*(h*.45f)+V3(0,h,0);
      cord(room,{root,base},.0012f,plant[2]);
      auto leaf=[=](float u,float v){float width=ww*powf(std::max(.00001f,sinf(PI*clampf(v,0,1))),.72f);
        return base+axis*(l*v)+lat*(width*u)+V3(0,l*(.42f*v+.33f*sinf(PI*v))-.23f*width*fabsf(u),0);};
      addGrid(room,10,14,-1,1,0,1,leaf,MT.hosta);
      std::vector<V3> vein;for(int n=0;n<=9;n++)vein.push_back(leaf(0,n/9.f)+V3(0,.0005,0));cord(room,vein,.00055f,plant[1],4);
    }
  }
  box({x+.43f,.69f,z-.17f},{.067,.13,.051},dark);
  rod({x+.43f,.68f,z-.17f},{x+.43f,.47f,z-.17f},.012f,dark);
  for(int i=0;i<9;i++)rod({x+.43f,.48f+i*.005f,z-.17f},{x+.43f,.482f+i*.005f,z-.17f},.016f,back);
  makeFish();
}
void addFern(V3 p,float scale,int pot){
  lathe(p,{{.083,0},{.092,.009},{.118,.16},{.124,.17},{.124,.182},{.11,.183}},pot);
  int soil=mat(Paint,{.064,.043,.023}),stem=mat(Paint,{.23,.27,.094});
  soft(room,p+V3(0,.168,0),{.112,.013,.112},soil,1);
  for(int i=0;i<24;i++){
    int leaf=MT.fern;
    Mesh f=fernFrond(rng,rng.r(.20,.42)*scale,18,leaf,stem,.21f);
    s.put(s.addProto(f),mTrans(p+V3(0,.172,0))*mRotY(i*2.399f)*mRotX(rng.r(-.90,-.13)));}
}
