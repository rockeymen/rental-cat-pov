// Apartment in metres; the same room, lens and daylight for all three shots.
#pragma once
#include "cat.h"
namespace Flat {
enum { Paint=40, Oak, Weave, Kilim, Fur, Print, Wool, Linen, Throw };
static inline float sp(float x,float e){return copysignf(powf(fabsf(x),e),x);}
static void soft(Mesh& m,V3 center,V3 radii,int mat,float power=.33f,M34 rot=M34()){
  addGrid(m,64,32,-PI,PI,-PI*.4999f,PI*.4999f,[=](float u,float v){
    return V3(sp(cosf(v),power)*sp(cosf(u),power),sp(sinf(v),power),sp(cosf(v),power)*sp(sinf(u),power))*radii;
  },mat,mTrans(center)*rot);
}
static void cord(Mesh& m,const std::vector<V3>& points,float radius,int mat,int sides=6){
  addTube(m,points,[=](float){return radius;},sides,mat);
}
struct Set {
  Scene& s; World& w; Mesh room,ball,paw,feelers; Rng rng{5288};
  int footID,toyID,yarnID,hairID,fishBody[2],fishTail[2];
  int mat(int k,V3 a,float rough=.8f){return s.addMat(k,a,rough,a,0,a);}
  void box(V3 p,V3 size,int m,M34 rot=M34()){addBox(room,size*-.5f,size*.5f,m,mTrans(p)*rot);}
  void rod(V3 a,V3 b,float r,int m){cord(room,{a,b},r,m,12);}
  void lathe(V3 p,const std::vector<V2>& profile,int m){addRevolve(room,profile,56,25,m,mTrans(p));}
#include "handcraft.inl"
#include "furnishings.inl"
  void build(){
    makeMats(s);
    w.mapW=w.mapH=1; w.map.assign(4,0); w.wet=0;w.fogIn=w.fogOut=0;w.sunDir=norm(V3(-1,.54,-.26));
    w.sunCol=V3(4.2,3.20,2.33);w.skyGain=1.05;w.sunRadius=.027;w.hfov=83*PI/180;
    const int plaster=mat(Paint,{.71,.68,.62}),white=mat(Paint,{.81,.77,.66}),dark=mat(Paint,{.032,.034,.030}),
      wood=mat(Oak,{.30,.158,.074}),chair=mat(Oak,{.19,.08,.038}),canvas=mat(Weave,{.245,.255,.204}),
      seam=mat(Weave,{.164,.173,.134}),mustard=mat(Weave,{.47,.25,.154}),blue=mat(Weave,{.045,.145,.16}),
      carpet=mat(Kilim,{.48,.205,.097}),linen=mat(Linen,{.82,.78,.69}),clay=mat(Paint,{.47,.19,.105}),
      enamel=mat(Paint,{.145,.31,.30}),brass=mat(Paint,{.42,.265,.105},.28),knit=mat(Throw,{.72,.564,.37});
    // Parquet battens with a staggered end joint and per-board finish.
    box({0,-.055,0},{5.65,.10,6.9},dark);
    for(int k=0;k<33;k++){float x=-2.8f+.087f+k*.174f;
      for(float z=-4.93f-(k%3)*.42f;z<3.48f;z+=1.43f){
        int oak=mat(Oak,V3(.385,.255,.160)*rng.r(.82,1.18));box({x,-.011f,z+.713f},{.173f,.022f,1.427f},oak);}}
    box({0,1.4,-3.46},{5.7,2.8,.12},plaster);box({2.86,1.4,0},{.12,2.8,7.0},plaster);
    box({0,2.84,0},{5.7,.1,7.0},white);box({0,1.4,3.47},{5.7,2.8,.1},plaster);
    box({-2.86,.36,0},{.12,.72,7},plaster);box({-2.86,2.64,0},{.12,.32,7},plaster);
    box({-2.86,1.61,-2.9},{.12,1.78,1.16},plaster);box({-2.86,1.61,2.55},{.12,1.78,1.90},plaster);
    box({0,.059,-3.375},{5.65,.118,.043},white);box({2.778,.059,0},{.045,.118,6.85},white);box({-2.77,.059,0},{.045,.118,6.85},white);
    box({-2.74,.73,-.36},{.24,.062,4.02},white);
    for(float z:{-2.3f,-1.f,.30f,1.6f}) box({-2.81,1.62,z},{.10,1.85,.042},white);
    box({-2.81,1.69,-.36},{.09,.044,3.92},white);box({-2.81,2.49,-.36},{.10,.060,3.94},white);
    courtyard();libraryWall(wood,linen,dark);
    // Opposite tenement courtyard: plaster reveals, sash trim and intermittent balconies.
    int facade=MT.brick, glass=mat(Paint,{.12,.21,.245},.18), balcony=mat(Paint,{.17,.19,.17});
    for(int house=0;house<3;house++){
      float face=-14.f-house*.31f,zc=-14.6f+house*11.3f;
      float roof=5.4f+house*.87f;
      box({face-.8f,roof-5.2f,zc},{1.6f,10.4f,11.12f},facade);
      box({face+.02f,roof,zc},{.22f,.15f,11.25f},white);
      for(int floor=0;floor<4;floor++)for(int bay=0;bay<6;bay++){
        float wy=roof-.99f-2.50f*floor,wz=zc-4.66f+1.81f*bay;
        box({face+.025f,wy,wz},{.049f,1.44f,1.04f},white);box({face+.051f,wy,wz},{.029f,1.31f,.89f},glass);
        box({face+.074f,wy,wz},{.031f,1.32f,.045f},white);box({face+.074f,wy+.29f,wz},{.031f,.04f,.9f},white);
        if((bay+floor)%3==1){box({face+.25f,wy-.77f,wz},{.56f,.087f,1.48f},balcony);
          rod({face+.50f,wy-.22f,wz-.74f},{face+.50f,wy-.22f,wz+.74f},.017f,balcony);
          for(int bar=0;bar<8;bar++){float bz=wz-.71f+bar*.203f;rod({face+.5f,wy-.73f,bz},{face+.5f,wy-.22f,bz},.009f,balcony);}}
      }}
    for(float z:{-1.f,.30f})box({-2.748f,1.45f,z},{.07f,.078f,.017f},brass);
    // Gathered linen panels on either side of the opening.
    for(float start:{-2.38f,1.09f}) addGrid(room,96,30,0,1,0,1,[=](float u,float v){
      return V3(-2.64f+.065f*cosf(u*39)+.014f*sinf(v*9+u*21),.14f+2.40f*v+.025f*cosf(u*39)*(1-v),start+u*(.59f-.09f*v));},linen);
    // Small imperfections in the join, mitre, and seam catch low raking light.
    for(float y:{.735f,.757f}){
      box({0,y,-3.387f},{5.60f,.016f,.031f},white);
      box({2.787f,y,0},{.025f,.016f,6.74f},white);
      box({0,y,3.393f},{5.63f,.016f,.024f},white);}
    // Sofa with individual seat welts, an imperfect overfilled back and tapered timber feet.
    for(float x:{-.99f,1.0f})for(float z:{-2.94f,-2.12f}) rod({x,.02f,z+.025f},{x*.97f,.22f,z},.039f,chair);
    soft(room,{0,.255,-2.56},{1.23,.126,.53},canvas,.22);
    for(float x:{-.544f,.544f}){
      soft(room,{x,.405,-2.49},{.541,.104,.444},canvas,.31);
      std::vector<V3> loop;for(int i=0;i<=180;i++){float a=2*PI*i/180;loop.push_back({x+.534f*sp(cosf(a),.28f),.437f,-2.49f+.438f*sp(sinf(a),.28f)});}cord(room,loop,.0045,seam);
      soft(room,{x,.695,-2.878},{.555,.279,.111},canvas,.35,mRotX(-.16));}
    for(float x:{-1.16f,1.16f})soft(room,{x,.447,-2.55},{.110,.236,.55},canvas,.35);
    textileCushion({-.77,.695,-2.65},{.248,.212,.091},mustard,mRotZ(.26)*mRotX(-.18),2);
    textileCushion({.70,.674,-2.63},{.242,.213,.091},blue,mRotZ(-.29)*mRotX(-.20),4);
    // Woven throw conforms to the seat and creases over its front lip.
    auto blanket=[](float u,float v){float fold=.021f*sinf(16*u+5*v+1.7f*sinf(6*v))+.012f*cosf(31*u-7*v)*(.4f+.6f*v)+.016f*expf(-sq((u-.25f-v*.31f)*7));
      float bend=.68f+.038f*sinf(8*u), drop=std::max(0.f,(v-bend)/(1-bend));
      return V3(-.80f+.89f*u+.029f*sinf(v*6+u*4),
        .549f+fold-.29f*drop,-2.75f+.691f*std::min(v/bend,1.f)+.047f*sinf(drop*PI)+.017f*drop);};
    addGrid(room,152,109,0,1,0,1,blanket,knit);
    // Connected knit courses follow the blanket around the seat lip. Each eye of yarn
    // rises above its neighbour and returns under it on the next row.
    for(int row=0;row<148;row++){std::vector<V3> yarn;
      for(int j=0;j<=1404;j++){float th=TAU*j/12.f,u=(j/12.f+.30f*sinf(th))/117.f;
        float v=(row+.55f+.67f*cosf(th)+.10f*sinf(2*th))/149.f;
        V3 q=blanket(u,v),du=blanket(u+.001f,v)-q,dv=blanket(u,v+.001f)-q;
        V3 normal=norm(cross(dv,du));q+=normal*(.0017f+.0011f*sinf(th));yarn.push_back(q);}
      cord(room,yarn,.00134f,knit,5);}
    for(int i=0;i<90;i++){V3 p=blanket(i/89.f,1);cord(room,{p,p+V3(.003,-.021,.003),p+V3(rng.r(-.009,.009),-.045,.002)},.0015,linen,4);}
    // Woven runner and folded end fringes.
    box({-.02,.012,-.80},{2.64,.024,2.10},carpet);
    for(int i=0;i<255;i++){float x=-1.32f+i*.0102f;for(int side:{-1,1}){
      float z=-.80f+side*1.05f;cord(room,{{x,.014f,z},{x+.003f,.011f,z+.030f*side},{x+rng.r(-.009,.009),.005f,z+side*rng.r(.055,.079)}},.0021,linen,4);}}
    // Side table, shelves, mugs, stacked paperbacks and an articulated reading light.
    for(float x:{1.47f,2.05f})for(float z:{-2.86f,-2.37f})box({x,.26f,z},{.038f,.52f,.038f},chair);
    box({1.76,.529,-2.61},{.68,.045,.59},wood);box({1.76,.196,-2.61},{.64,.021,.53},wood);
    auto book=[&](V3 p,float width,float length,float h,float yaw,int cover){
      for(int k:{-1,1})box(p+V3(0,k*h*.48f,0),{width,.005f,length},cover,mRotY(yaw));
      box(p,{width-.009f,h-.005f,length-.008f},white,mRotY(yaw));};
    book({1.76,.227,-2.57},.39,.25,.039,.05,blue);book({1.78,.269,-2.56},.345,.232,.041,-.12,clay);
    book({1.69,.57,-2.61},.29,.213,.032,.12,clay);
    lathe({1.73,.588,-2.60},{{.038,0},{.043,.002},{.051,.09},{.046,.092},{.043,.080},{.035,.012}},enamel);
    std::vector<V3> handle;for(int i=0;i<=20;i++){float a=i*PI/20;handle.push_back({1.775f+.033f*sinf(a),.637f+.027f*cosf(a),-2.60f});}cord(room,handle,.006,enamel);
    lathe({-1.72,0,-2.68},{{.15,0},{.15,.018},{.10,.034},{.021,.045}},dark);
    rod({-1.72,.033,-2.68},{-1.72,1.40,-2.68},.013,brass);
    rod({-1.72,1.40,-2.68},{-1.52,1.53,-2.66},.012,brass);
    lathe({-1.52,1.21,-2.66},{{.196,0},{.201,.009},{.130,.28},{.115,.303}},mat(61,{.79,.56,.32}));
    // Two framed graphic prints, cut from mathematical colour fields in the shader.
    int art=mat(Paint,{.67,.629,.53});
    for(int i=0;i<2;i++){float x=-.46f+i*.81f; box({x,1.65f,-3.367f},{.68f,.87f,.047f},chair);
      box({x,1.65f,-3.339f},{.65f,.836f,.007f},white);box({x,1.65f,-3.332f},{.541f,.704f,.008f},art);}
    // Pressed herbarium specimens mounted under each frame.
    for(int f=0;f<2;f++){
      float x=-.46f+f*.81f;
      int sepia=mat(Paint,f?V3(.27,.23,.12):V3(.17,.18,.112));
      for(int k=0;k<4;k++){
        float len=.29f+.074f*sinf(k*3.8f+f), lean=(k-1.5f)*.072f, y=1.45f+k*.008f;
        auto axis=[=](float t){return V3(x-.054f+lean*t+.036f*sinf(t*2.8f+k)*t,y+len*t,-3.324f);};
        std::vector<V3> pts;for(int j=0;j<20;j++)pts.push_back(axis(j/19.f));cord(room,pts,.0016f,sepia);
        for(int j=3;j<19;j++)for(int d:{-1,1}){
          float t=(j+.3f*d)/20,wide=(.030f+.015f*f)*sinf(t*PI);
          addGrid(room,5,8,-1,1,0,1,[=](float u,float v){
            return axis(t)+V3(d*wide*v,.028f*v+u*.006f*sinf(v*PI),.0007f);},sepia);}
      }
    }
    // Wall hardware and a cable clipped along the skirting board.
    box({1.41,.28,-3.337},{.149,.088,.028},white);
    for(float x:{1.37f,1.446f})for(int k:{-1,1})box({x+k*.013f,.281f,-3.319f},{.005f,.018f,.004f},dark,mRotZ(k*.32f));
    cord(room,curvePts({-1.72,.042,-2.67},{-1.46,.006,-2.61},{-1.53,.008,-3.32},{-1.26,.015,-3.32},28),.0036f,dark);
    // A simple paper dust jacket tipped open on a stack of books.
    book({1.55,.032,-.78},.255,.195,.043,.23,blue);
    book({1.55,.072,-.78},.23,.175,.031,.10,clay);
    // Cardboard move-in crate, tape and finger slots, under a low window plant stand.
    for(int i=0;i<2;i++){float x=2.26f-i*.12f,y=.19f+i*.335f,z=-1.87f;float sx=.62f-i*.09f,hh=.37f-i*.07f;
      box({x,y,z},{sx,hh,.45},mat(Paint,{.44,.275,.13}));box({x,y+hh/2+.001f,z},{.073,.004,.45},mustard);
      soft(room,{x,y+.08f,z+.226f},{.07,.014,.002},dark,.53);}
    // A pothos, including stems with leaves at different pitches and sizes.
    V3 pot(-2.07,0,-2.21);lathe(pot,{{.126,0},{.14,.015},{.176,.30},{.184,.319},{.157,.32},{.155,.30}},clay);
    lathe(pot+V3(0,.294,0),{{0,0},{.151,0},{.152,.004}},dark);
    for(int j=0;j<43;j++){float a=rng.r(0,6.28),h=rng.r(.29,.91),rr=rng.r(.09,.32);
      V3 tip=pot+V3(rr*cosf(a),.28+h,rr*sinf(a));V3 root=pot+V3(.02*cosf(a),.30f,.02*sinf(a));V3 mid=(root+tip)*.5f+V3(0,.09,0);
      cord(room,{root,mid,tip},.0028,canvas);LeafP leaf;leaf.len=rng.r(.12,.26);leaf.wid=leaf.len*rng.r(.31,.45);leaf.pitch=.20;leaf.droop=.78;leaf.fold=.29;leaf.nu=9;leaf.nv=15;leaf.prof=profEll(.72,.74);
      addLeaf(room,leaf,mTrans(tip)*mRotY(a),mat(Paint,V3(.096,.22,.077)*rng.r(.65,1.28)));}
    // Cotton ball with actual overlapping yarn loops and a short knotted tail.
    int rust=mat(Wool,{.82,.258,.092}),cream=mat(Wool,{.89,.67,.38}), teal=mat(Wool,{.044,.22,.23});
    int oatmeal=mat(Wool,{.62,.435,.272}),ochre=mat(Wool,{.565,.244,.113});
    soft(ball,{0,0,0},{.068,.070,.068},ochre,1);
    // Bands of adjacent courses wind around changing axes. Tiny flyaway fibres
    // leave the silhouette at the same locations as their parent strand.
    for(int layer=0;layer<13;layer++){
      M34 turn=mRotY(layer*2.3999f+.27f)*mRotX(1.02f*sinf(layer*2.12f))*mRotZ(.64f+layer*.36f);
      for(int i=0;i<19;i++){
        float lat=(i-9)*.042f,r=.0681f+layer*.00044f;
        int col=(layer%4==0 && i%9<4)?oatmeal:(i%7==0 || layer%4==1?ochre:rust);
        std::vector<V3> strand;
        for(int j=0;j<=150;j++){float a=TAU*j/150.f,phi=lat+.065f*sinf(a*2+layer)+.017f*sinf(a*4+i*.06f);
          V3 n=turn.dir({cosf(a)*cosf(phi),sinf(phi),sinf(a)*cosf(phi)}),q=n*r;
          strand.push_back(q);
          if(j<150 && rng.f()<.20f){V3 tip=q+n*rng.r(.0019,.0039)+V3(rng.r(-.0017,.0017),.0008,.0004);
            addTube(ball,{q,(q+tip)*.5f+n*.00031f,tip},[](float t){return .00012f*(1-.93f*t);},3,col);}
        }
        cord(ball,strand,.00164f,col,7);}}
    toyID=s.addProto(ball);
    Mesh loose;
    auto line=curvePts({.036f,.014f,0},{.070f,-.003f,.067f},{.115f,.001f,-.029f},{.178f,.001f,.014f},51);
    auto end=curvePts(line.back(),{.211f,.002f,.043f},{.237f,.002f,.012f},{.251f,.002f,.008f},22);
    line.insert(line.end(),end.begin()+1,end.end());cord(loose,line,.0021f,ochre);
    yarnID=s.addProto(loose);
    // Slender radiator and copper valves underneath the open window.
    for(int j=0;j<25;j++)soft(room,{-2.69,.368f,-.97f+.076f*j},{.061,.246,.031},white,.39f);
    for(float h:{.19f,.53f})rod({-2.69f,h,-1.01f},{-2.69f,h,.91f},.019f,white);
    rod({-2.67f,.19f,.94f},{-2.67f,.015f,.94f},.011f,brass);
    makeBed(wood,seam,linen); makeTank(wood,dark);
    addFern({1.72f,.555f,-2.78f},.91f,clay);
    // A glazed planter on a plant stool catches the sidelight beside the bed.
    for(float dx:{-.105f,.105f})for(float dz:{-.075f,.075f})rod({-2.38f+dx,.015f,1.48f+dz},{-2.38f+dx*.84f,.59f,1.48f+dz*.84f},.014f,chair);
    box({-2.38f,.60f,1.48f},{.33f,.026f,.273f},wood);
    addFern({-2.38f,.615f,1.48f},1.38f,enamel);
    // Loose paperback open face-down at an angle on the duvet.
    book({-1.52f,.512f,.33f},.145f,.207f,.025f,.38f,blue);
    // Bedside cabinet, drawers, handles and a water carafe.
    for(float dx:{-.15f,.15f})for(float dz:{-.125f,.125f})box({-1.81f+dx,.13f,-1.35f+dz},{.037f,.26f,.035f},chair);
    box({-1.81f,.36f,-1.35f},{.42f,.28f,.35f},wood);
    for(int j=0;j<2;j++){box({-1.81f,.292f+j*.132f,-1.169f},{.40f,.123f,.014f},wood);rod({-1.81f,.292f+j*.132f,-1.165f},{-1.81f,.292f+j*.132f,-1.143f},.013f,brass);}
    book({-1.83f,.521f,-1.38f},.228f,.168f,.032f,.22f,mustard);
    lathe({-1.67f,.504f,-1.29f},{{.042,0},{.044,.012},{.045,.095},{.041,.118},{.026,.137},{.021,.161},{.024,.177}},enamel);
    // A panelled entrance door, old brass latch and a cloth tote on the peg rail.
    for(float dx:{-.46f,.46f})box({.29f+dx,1.032f,3.374f},{.073f,2.064f,.061f},white);
    box({.29f,2.034f,3.374f},{.99f,.079f,.061f},white);
    box({.29f,1.00f,3.376f},{.854f,1.984f,.042f},mat(Paint,{.49,.485,.421}));
    for(float yy:{.51f,1.46f})for(float dx:{-.194f,.194f}){
      box({.29f+dx,yy,3.342f},{.332f,.77f,.025f},white);
      box({.29f+dx,yy,3.325f},{.301f,.739f,.013f},mat(Paint,{.55,.536,.463}));}
    box({.625f,.995f,3.307f},{.054f,.144f,.020f},brass);
    rod({.624f,1.015f,3.303f},{.624f,1.015f,3.252f},.015f,brass);rod({.624f,1.015f,3.252f},{.529f,1.015f,3.252f},.012f,brass);
    box({-.82f,1.58f,3.374f},{.705f,.067f,.045f},wood);
    for(int j=0;j<3;j++)rod({-1.08f+j*.25f,1.58f,3.36f},{-1.08f+j*.25f,1.609f,3.286f},.011f,chair);
    soft(room,{-.85f,1.136f,3.276f},{.172f,.196f,.038f},knit,.39f,mRotZ(.075));
    cord(room,curvePts({-.97f,1.29f,3.258f},{-.92f,1.51f,3.277f},{-.82f,1.98f,3.281f},{-.73f,1.288f,3.26f},26),.011f,linen);
    // Prayer plant at the foot of the bed, translucent herringbone leaf blades.
    V3 pp(-1.34f,.008f,1.18f);
    lathe(pp,{{.099,0},{.105,.009},{.135,.202},{.137,.217},{.127,.22},{.124,.205}},clay);
    soft(room,pp+V3(0,.204,0),{.126,.007,.126},dark,1);
    int foliage=MT.calathea,vein=mat(Paint,{.231,.296,.119});
    for(int j=0;j<33;j++){
      float a=j*2.399f,b=rng.r(.055,.235),h=rng.r(.13,.38),l=rng.r(.13,.245),ww=l*rng.r(.24,.36);
      V3 root=pp+V3(0,.206f,0),axis(cosf(a),0,sinf(a)),lat(-sinf(a),0,cosf(a));
      V3 base=root+axis*b+V3(0,h,0);
      cord(room,curvePts(root,root+V3(0,h*.38f,0),base-axis*.061f,base,12),.0023f,vein);
      auto blade=[=](float u,float v){float width=ww*powf(std::max(.00001f,sinf(PI*clampf(v,0,1))),.74f);
        return base+axis*(l*v)+lat*(width*u)+V3(0,l*(.29f*sinf(v*PI)-.31f*v*v)-width*.33f*fabsf(u),0);};
      addGrid(room,16,24,-1,1,0,1,blade,foliage);
    }
    // Cut flowers and a low branch by the window break up the bed silhouette.
    V3 jar(-1.75f,.536f,-1.36f);
    lathe(jar,{{.038,0},{.046,.015},{.055,.084},{.042,.13},{.024,.141},{.023,.16}},MT.terracotta);
    for(int k=0;k<12;k++){
      Mesh m=(k%3==0?poppy(rng):k%3==1?lavender(rng):tallDaisy(rng,0));
      float a=k*2.399f;
      s.put(s.addProto(m),mTrans(jar+V3(.013f*cosf(a),.125f,.013f*sinf(a)))*mRotY(a)*mRotX(.20f+.26f*sinf(k))*mScale(rng.r(.41,.67)));}
    // An indoor hanging pothos. Foliage grows at staggered nodes down the vines.
    V3 basket(-2.39f,1.91f,-.85f);
    lathe(basket,{{.069,0},{.091,.019},{.119,.132},{.118,.151},{.108,.153}},MT.terracotta);
    for(int i=0;i<3;i++){float a=i*TAU/3;cord(room,{basket+V3(.111f*cosf(a),.080f,.111f*sinf(a)),basket+V3(.018f*cosf(a),.62f,.018f*sinf(a)),basket+V3(0,.83f,0)},.0024f,linen,5);}
    for(int j=0;j<13;j++){
      float angle=j*2.399f,len=rng.r(.31,.85);std::vector<V3> stalk;
      for(int k=0;k<15;k++){float t=k/14.f;
        V3 q=basket+V3((.135f+.061f*t)*cosf(angle+.45f*t)+.019f*sinf(9*t+j),.145f-len*t,(.135f+.061f*t)*sinf(angle+.45f*t));
        stalk.push_back(q);LeafP l;l.len=rng.r(.046,.098);l.wid=l.len*.49f;l.prof=profEll(.60,.61,.42);
        l.nu=8;l.nv=14;l.pitch=.44f;l.droop=1.16f;l.fold=.29f;l.twist=.42f*sinf(j+k);l.w=rng.f();l.phase=rng.f()*TAU;
        addLeaf(room,l,mTrans(q)*mRotY(angle+k*2.25f),MT.hostaVar);}
      cord(room,stalk,.0016f,MT.stemGreen,5);}
    // A linen overshirt on a hanger: overlapping fronts, yoke, collar, loose sleeves.
    int shirt=mat(Weave,{.145,.198,.22}), buttons=mat(Paint,{.65,.59,.47});
    V3 hook(-1.08f,1.605f,3.25f);
    auto shirtFace=[=](float u,float v){float w=.12f+.067f*sinf(v*2.1f)+.013f*v;
      return hook+V3(u*w,-.051f-.61f*v-.046f*u*u*(1-v)+.009f*sinf(u*7)*v,
        -.020f-.022f*sinf(PI*v)+.018f*sinf(17*u+v*3.3f)*(.45f+.55f*v)-.024f*expf(-sq((u+.24f)*5)));};
    addGrid(room,62,57,-1,1,0,1,shirtFace,shirt);
    for(int d:{-1,1}){
      addGrid(room,24,36,-1,1,0,1,[=](float u,float v){
        return shirtFace(d,.09f)+V3(d*(.056f*v+.042f*u),-.37f*v+.012f*u,-.039f*sinf(PI*v)-.012f*cosf(u*5+v*8));},shirt);
      addGrid(room,12,12,0,1,0,1,[=](float u,float v){return shirtFace(d*u*.55f,v*.15f-.015f)+V3(0,0,-.019f*v);},shirt);}
    std::vector<V3> seamPts;for(int k=0;k<40;k++)seamPts.push_back(shirtFace(-.077f,k/39.f)+V3(0,0,-.002));
    cord(room,seamPts,.0024,shirt);
    for(int j=1;j<7;j++)soft(room,shirtFace(-.065f,j/7.5f)+V3(0,0,-.003f),{.004f,.004f,.0014f},buttons,1);
    int whisk=mat(Paint,{.49,.46,.38});
    for(int k=0;k<2;k++)for(int j=0;j<5;j++){float d=k?1:-1;auto pts=curvePts({d*.05f,-.040f,-.024f},{d*.071f,-.047f+j*.003f,-.08f},
      {d*.125f,-.058f+j*.004f,-.114f},{d*(.156f+.006f*j),-.045f+j*.005f,-.135f},14);
      addTube(feelers,pts,[](float t){return .00016f*(1-.97f*t);},4,whisk);}
    hairID=s.addProto(feelers);
    // Only the paw tip crosses the bottom of the lens on each swat.
    int coat=mat(Fur,{.61,.42,.27}),creamToe=mat(Fur,{.83,.73,.60});
    auto tuft=[&](V3 c,V3 r,int m,int count){
      soft(paw,c,r,m,.88f);
      for(int j=0;j<count;j++){
        float a=rng.r(0,TAU),y=rng.r(-.96f,1.f),xz=sqrtf(1-y*y);
        V3 n(xz*cosf(a),y,xz*sinf(a)),p=c+r*n;
        addTube(paw,{p,p+n*.0019f+V3(0,0,.0018f),p+n*.003f+V3(0,0,.0042f)},
          [](float u){return .00022f*(1-.92f*u);},3,m);}};
    tuft({0,.002f,.142f},{.029f,.024f,.147f},coat,1200);
    tuft({0,0,.04f},{.033f,.023f,.042f},coat,850);
    for(int j=0;j<4;j++)tuft({(j-1.5f)*.014f,-.001f,.016f+.005f*fabsf(j-1.5f)},
       {.010f,.015f,.020f},creamToe,190);
    footID=s.addProto(paw);
    s.put(s.addProto(room),M34());
  }
};
struct Beat {float t; V3 eye,look,toy;float roll,focus;};
static const Beat beats[]={
 {0,{-.10,.28,3.14},{-.30,.37,-.61},{-.51,.076,1.50},0,1.55},
 {2,{.22,.264,2.48},{-.34,.352,-.31},{.14,.076,1.64},.013,1.02},
 {4,{-.35,.22,1.72},{.13,.19,.42},{.07,.076,.68},-.028,1.00},
 {6,{.36,.21,.74},{1.32,.33,-.67},{1.16,.076,-.38},.026,1.02},
 {7,{1.00,.33,.06},{1.52,.56,-.82},{1.26,.076,-.44},-.016,.92},
 {8,{1.587,.559,-.348},{1.560,.607,-.817},{1.27,.076,-.45},-.019,.363},
 {10,{1.55,.58,-.27},{1.70,.57,-.89},{1.27,.076,-.45},.016,.52},
 {11,{.68,.20,-1.39},{-.15,.64,-2.5},{1.27,.076,-.45},.03,1.14},
 {11.7,{.02,.92,-2.04},{-.33,.56,-2.50},{1.27,.076,-.45},.048,.76},
 {13,{-.24,.83,-2.55},{.16,.54,-1.22},{1.27,.076,-.45},-.034,1.32},
 {15,{-.41,.686,-2.66},{-1.35,.63,-.35},{1.27,.076,-.45},-.098,1.18},
 {18,{-.41,.662,-2.66},{-1.37,.61,-.35},{1.27,.076,-.45},-.112,1.18}};
static Beat beat(float t){
  int i=0,N=sizeof(beats)/sizeof(*beats);while(i+2<N && beats[i+1].t<t)++i;
  auto a=beats[i],b=beats[i+1];float f=smooth(a.t,b.t,t);
  return {t,lerp(a.eye,b.eye,f),lerp(a.look,b.look,f),lerp(a.toy,b.toy,f),lerpf(a.roll,b.roll,f),lerpf(a.focus,b.focus,f)};
}
static CamPose pose(float t){
  auto b=beat(t); V3 f=norm(b.look-b.eye),right=norm(cross(f,{0,1,0})),up=cross(right,f);
  return {b.eye,right*cosf(b.roll)+up*sinf(b.roll),up*cosf(b.roll)-right*sinf(b.roll),f,b.focus,t<5?.0055f:(t<11?.0024f:.0045f)};
}
// One escape circuit rejoins the original orbit after the cat turns away.
static float panicPulse(float t,float duration,float decay){
  float u=clampf(t/duration,0,1);
  // Integrated squared attack with an exponential release; zero speed at onset.
  auto integral=[&](float x){return 1-(1+decay*x)*expf(-decay*x);};
  return integral(u)/integral(1);
}
static V3 escapePosition(float t,int j){
  float dt=t-(8.22f+j*.125f);
  float panic=smooth(0,.16f,dt)*(1-smooth(1.60f,2.65f,dt));
  float phase=(t-8)*(.44f+j*.08f)+j*3.35f;
  // Dash to cover, bend around the end of the tank, then cross it again.
  phase+=2*PI*(.52f*panicPulse(dt,1.30f,4.0f)+.48f*panicPulse(dt-.67f-j*.08f,1.89f,4.1f));
  float dive=smooth(0,.23f,dt)*(1-smooth(.36f,1.31f,dt));
  return {1.54f+j*.228f+(.225f+(.079f-j*.042f)*panic)*sinf(phase),
          .604f+.034f*sinf(2*phase+.6f)-j*.079f+(.021f-j*.051f)*dive+.016f*panic*sinf(phase*1.5f+j),
          -.795f+j*.036f+.071f*cosf(phase)-(.055f+j*.009f)*dive};
}
static void moving(const Set& room,float t,const CamPose& c,std::vector<InstCPU>& out,V3 toy,bool film){
  auto place=[&](int id,M34 tr){out.push_back({id,tr,0,.4f,TINT1,0,{}});};
  place(room.toyID,mTrans(toy)*mRotX(toy.z/.073f)*mRotZ(-toy.x/.073f));
  place(room.yarnID,mTrans({toy.x,.002f,toy.z})*mRotY(.18f*sinf(toy.x*3.5f-toy.z*2.4f)));
  place(room.hairID,mBasis(c.R,c.U,-c.F,c.pos));
  for(int j=0;j<2;j++){
    float a=(t-8)*(.44f+j*.08f)+j*3.35f;
    float dx=.225f*cosf(a),dz=-.071f*sinf(a);
    V3 p(1.54f+.225f*sinf(a)+j*.228f,.604f+.034f*sinf(a*2+.6f)-j*.079f,-.795f+.071f*cosf(a)+j*.036f);
    float yaw=atan2f(-dz,dx)-.35f*cosf(a),pitch=.088f*sinf(a+.7f),bank=0;
    float tail=.26f*sinf(t*7+j);
    float dt=t-(8.22f+j*.125f);
    if(film && dt>0 && dt<2.75f){
      p=escapePosition(t,j);
      V3 velocity=(escapePosition(t+.004f,j)-escapePosition(t-.004f,j))/.008f;
      float active=smooth(0,.085f,dt)*(1-smooth(1.95f,2.75f,dt));
      float heading=atan2f(-velocity.z,velocity.x);
      yaw+=active*atan2f(sinf(heading-yaw),cosf(heading-yaw));
      pitch=lerpf(pitch,atan2f(velocity.y,hypotf(velocity.x,velocity.z)),active);
      bank=active*.22f*sinf(5.2f*dt+j);
      float burst=active*(.45f+.55f*expf(-2*dt));
      tail=lerpf(tail,.64f*sinf(2*PI*(3.8f*dt+.50f*(1-expf(-5*dt)))+j),burst);
    }
    M34 body=mTrans(p)*mRotY(yaw)*mRotZ(pitch)*mRotX(bank)*mScale(1.08f-j*.38f);
    place(room.fishBody[j],body);
    place(room.fishTail[j],body*mTrans({-.066f,0,0})*mRotY(tail));
  }
}
} // namespace Flat
