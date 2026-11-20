void textileCushion(V3 c,V3 r,int cloth,M34 rot,float phase){
  auto cover=[=](float u,float v,float side){
    float x=sp(u,.96f),y=sp(v,.96f);
    float edge=std::max(0.f,(1-u*u)*(1-v*v));
    float crown=powf(edge,.54f);
    float gather=.011f*sinf(18*v+5*u+phase)*expf(-sq((fabsf(u)-.88f)*6))+
      .009f*sinf(20*u-4*v+phase)*expf(-sq((fabsf(v)-.83f)*5));
    return c+rot.dir({r.x*x*(1-.089f*(1-v*v)*u*u),r.y*y*(1-.085f*(1-u*u)*v*v),
      side*(.003f+r.z*crown)+gather});};
  for(float d:{-1.f,1.f})addGrid(room,72,68,-1,1,-1,1,[=](float u,float v){return cover(u,v,d);},cloth);
  for(int d:{-1,1})for(int j=0;j<2;j++){std::vector<V3> points;
    for(int k=0;k<=80;k++){float t=-1+k/40.f;points.push_back(j?cover(t,d,1):cover(d,t,1));}
    cord(room,points,.0031f,cloth);}
}
void caneBed(float x,float z,int frame){
  int rattan[4];for(int i=0;i<4;i++)rattan[i]=mat(58,V3(.53,.34,.17)*(.77f+.105f*i));
  // Oval-section strips weave in four directions through the same panel.
  const float y=.681f,halfW=.606f,halfH=.159f,step=.017f;
  for(int k=0;k<4;k++){
    float a=k*PI/4,dx=cosf(a),dy=sinf(a),nx=-dy,ny=dx;
    for(int j=-42;j<=42;j++){
      float offset=j*(k%2?step*1.416f:step);
      float lo=-2,hi=2;
      auto clip=[&](float origin,float dir,float limit){if(fabsf(dir)<.001f){if(fabsf(origin)>limit)hi=lo-1;return;}
        float l=(-limit-origin)/dir,h=(limit-origin)/dir;if(l>h)std::swap(l,h);lo=std::max(lo,l);hi=std::min(hi,h);};
      clip(nx*offset,dx,halfW);clip(ny*offset,dy,halfH);
      if(hi-lo<.008f)continue;
      int cuts=std::max(3,int((hi-lo)/.004));
      addGrid(room,2,cuts,-1,1,0,1,[=](float u,float v){float t=lo+(hi-lo)*v, side=offset+u*(k%2?.0014f:.0021f);return V3(x+nx*side+dx*t,y+ny*side+dy*t,z+.0013f*k+.0016f*sinf(PI*t/step+j*PI));},rattan[(k+abs(j))%4]);}}
  for(float d:{-1.f,1.f}){soft(room,{x,y+d*.171f,z},{.64,.020,.024},frame,.18);
    soft(room,{x+d*.617f,y,z},{.025,.174,.024},frame,.21);}
}
void libraryWall(int oak,int linen,int dark){
  int colors[7];V3 inks[]={{.19,.24,.21},{.50,.27,.15},{.65,.59,.46},{.48,.43,.31},{.126,.195,.218},{.31,.18,.14},{.53,.40,.25}};
  for(int i=0;i<7;i++)colors[i]=mat(Weave,inks[i]);
  // A modest wall shelf and a few unevenly spaced, linen-bound books.
  for(float z:{1.48f,2.11f}){
    box({-1.56f,z,-3.21f},{1.09f,.026f,.31f},oak);
    for(float x:{-1.94f,-1.17f}){rod({x,z-.018f,-3.09f},{x,z-.20f,-3.35f},.0075f,dark);}
    float x=-2.045f;
    for(int k=0;k<(z>2?10:15);k++){float w=rng.r(.015,.044),h=rng.r(.154,.291),d=rng.r(.138,.199);int m=colors[(rng.i(7))];
      x+=w*.54f;
      V3 c(x,z+.014f+h*.5f,-3.30f+d*.48f);
      box(c,{w,h-.007f,d},linen);
      for(float sign:{-1.f,1.f})box(c+V3(sign*w*.47f,0,0),{.0032f,h,d+.004f},m);
      soft(room,c+V3(0,0,d*.5f),{w*.52f,h*.501f,.0042f},m,.28);
      for(int line=0;line<3;line++)box(c+V3(0,h*.27f-line*.008f,d*.5f+.0044f),{w*(.62f-line*.08f),.0015f,.001f},linen);
      x+=w*.55f;}
    if(z>2){V3 pp(-1.30,2.126,-3.18);lathe(pp,{{.041,0},{.056,.014},{.058,.091},{.047,.104}},MT.terracotta);
      for(int i=0;i<7;i++){Mesh l=lavender(rng);s.put(s.addProto(l),mTrans(pp+V3(0,.092,0))*mRotY(i*2.4f)*mRotZ(.2f*sinf(i))*mScale(.57f));}}
  }
}
void courtyard(){
  std::vector<V3> ends; Mesh trunk;
  int sprigs[6];for(int j=0;j<6;j++){Mesh m=sprig(rng,rng.r(.25,.46),13+rng.i(4),rng.r(.046,.085),.55,MT.birchLeaf,MT.twig,.20,4);sprigs[j]=s.addProto(m);}
  std::function<void(V3,V3,float,float,int)> grow;
  grow=[&](V3 p,V3 dir,float l,float r,int depth){
    V3 mid=p+dir*(l*.49f),e=p+dir*l+V3(rng.r(-.16,.16)*l,.04f*l,rng.r(-.18,.18)*l);
    addTube(trunk,{p,mid,e},[=](float t){return r*(1-.54f*t);},depth<3?9:6,MT.barkBirch);
    if(depth>=4){for(int k=0;k<6;k++)s.put(sprigs[rng.i(6)],mTrans(lerp(mid,e,k*.2f))*mRotY(rng.r(0,6.28))*mRotX(rng.r(-.6,.48))*mScale(rng.r(.86,1.46))); return;}
    for(int j=0;j<4;j++){float a=j*1.57f+rng.r(-.42,.42);grow(lerp(mid,e,.22f+.24f*j),norm(dir*.68f+V3(cosf(a)*.84f,rng.r(-.3,.58),sinf(a)*.88f)),l*rng.r(.56,.82),r*.57f,depth+1);}
  };
  grow({-4.65,-2.10f,1.92f},norm(V3(.15,1,-.21)),2.53f,.093f,0);
  s.put(s.addProto(trunk),M34());
}
void tankGarden(float x,float z){
  int granite[6], stems[6];for(int j=0;j<6;j++){
    Mesh m=rock(rng,j%2?MT.stoneWet:MT.stoneMoss,rng.r(.52,.81));granite[j]=s.addProto(m);
    Mesh stem=sprig(rng,.16f+rng.r(0,.15),27+rng.i(9),.027f,.28f,MT.sprig,MT.stemGreen,-.14f,4);stems[j]=s.addProto(stem);}
  for(int bank:{-1,1}){
    for(int j=0;j<32;j++){float dx=rng.r(.12,.48),r=rng.r(.019,.082),zz=rng.r(-.166,.12);
      if(j==0){dx=.342;r=.117;zz=-.04;}
      s.put(granite[j%6],mTrans({x+bank*dx,.402f+r*.18f,z+zz})*mRotZ(.20f*bank)*mRotY(rng.r(0,6.28))*mScale({r*1.38f,r,r*.83f}));}
    for(int j=0;j<125;j++){float dx=rng.r(.105,.49),r=rng.r(.51,1.31),zz=rng.r(-.198,-.01);
      s.put(stems[j%6],mTrans({x+bank*dx,.40f,z+zz})*mRotY(rng.r(0,6.28))*mRotZ(rng.r(-.39,.39))*mScale(r));}}
  // Some small floating leaves with fine descending rootlets below the visible waterline.
  for(int i=0;i<64;i++){float dx=rng.r(-.48,.48),dz=rng.r(-.18,.08),rr=rng.r(.004,.014);
    if(dx>-.18f&&dx<.24f&&dz>-.09f)continue;
    for(int j=0;j<3;j++)addBlob(room,{x+dx+j*.004f,.820f,z+dz+j*.002f},{rr,.0012f,rr*.78f},9,6,.09,3,i,MT.lilyPad);
    for(int j=0;j<3;j++){V3 p(x+dx,.821f,z+dz);addTube(room,curvePts(p,p+V3(.001,-.015,.003),p+V3(-.003,-.035,0),p+V3(rng.r(-.009,.009),rng.r(-.068,-.022),.004),9),[](float t){return .00043f*(1-.9f*t);},4,MT.stemPale);}}
}
