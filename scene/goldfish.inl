// Anatomical stations from the caudal peduncle to the snout, in metres.
V3 flank(float t,float angle,int variety){
  // Back and belly traced separately. The head sits below the top of the peduncle.
  static const float upper[]={.015f,.013f,.017f,.022f,.026f,.027f,.026f,.023f,.017f,.008f,-.006f};
  static const float lower[]={-.011f,-.012f,-.018f,-.023f,-.028f,-.031f,-.032f,-.031f,-.029f,-.024f,-.013f};
  static const float breadth[]={.006f,.008f,.013f,.017f,.020f,.023f,.024f,.023f,.020f,.015f,.004f};
  const float q=clampf(t,0,.99999f)*10;int j=int(q);float k=q-j;
  auto interp=[&](const float* v){float a=v[std::max(0,j-1)],b=v[j],c=v[j+1],d=v[std::min(10,j+2)];return .5f*((2*b)+(-a+c)*k+(2*a-5*b+4*c-d)*k*k+(-a+3*b-3*c+d)*k*k*k);};
  float c=cosf(angle),top=interp(upper),bottom=interp(lower);
  return {-.068f+.163f*t,(top+bottom)*.5f+(top-bottom)*.5f*c*(variety?1.10f:1.f),interp(breadth)*sinf(angle)};

}
void makeFish(){
  int skin[]={mat(51,{.9,.3,.06}),mat(52,{.89,.70,.45})};
  int fin=s.addMat(53,{.86,.47,.20},.24,{.9,.66,.40},.56,{0,0,0},0,0,0,0,0,0,.60);
  int eye=mat(62,{.245,.174,.109},.31),lip=mat(59,{.74,.47,.28},.44);
  for(int n=0;n<2;n++){
    Mesh body,tail;
    addGrid(body,80,72,0,1,0,1,[&](float u,float v){return flank(v,2*PI*u,n);},skin[n]);
    // Cup individual overlapping scales against the flank; leave snout and gill covers bare.
    for(int side:{-1,1})for(int row=0;row<13;row++){
      float a=.21f+row*.212f;
      for(int col=0;col<19;col++){
        float t=.14f+.0315f*(col+(row%2)*.47f);
        if(t<.21f && (row<4 || row>9))continue;
        addGrid(body,8,5,-1,1,0,1,[&,a,t,side](float u,float v){
          float span=.135f*sqrtf(std::max(.00001f,1-u*u));
          float tt=t+.020f*u;
          float ang=(a+span*(2*v-1))*side;
          V3 p=flank(tt,ang,n);
          float raise=.00023f+.00068f*(1-u)*.5f*sqrtf(std::max(0.f,1-u*u))*sinf(PI*v);
          return p+V3(-.28f,cosf(ang),sinf(ang))*raise;
        },skin[n]);}}
    for(int d:{-1,1}){
      // A shallow cornea follows the head. Radius is also the iris shader coordinate.
      addGrid(body,56,16,0,1,.0001f,1,[&](float az,float radius){
        float phi=2*PI*az;
        float t=.884f+radius*.031f*cosf(phi);
        float angle=d*(1.39f+radius*.264f*sinf(phi));
        V3 p=flank(t,angle,n);
        V3 normal=norm(cross(flank(t+.0001f,angle,n)-flank(t-.0001f,angle,n),
                             flank(t,angle+.001f,n)-flank(t,angle-.001f,n)));
        if(normal.z*d<0)normal=normal*-1.f;
        return p+normal*(.00019f+.0010f*(1-radius*radius));
      },eye);
      float cheek=.745f;
      auto operculum=[&](float u,float v){float a=d*(.75f+1.87f*v);
        float t=cheek+.135f*u-.061f*sinf(PI*v);V3 p=flank(t,a,n);
        return p+V3(0,cosf(a),sinf(a))*(.0004f+.0028f*(1-u)*sinf(PI*v));};
      addGrid(body,30,32,0,1,0,1,operculum,skin[n]);
      std::vector<V3> edge;
      for(int k=0;k<=28;k++)edge.push_back(operculum(0,k/28.f));
      cord(body,edge,.00055f,mat(59,{.35,.22,.15},.48),5);
      V3 nose=flank(.954f,1.02f*d,n);soft(body,nose,{.0018,.0011,.00135},mat(59,{.32,.17,.075},.46),1);
      for(int g=0;g<2;g++){
        float t0=g?.455f:.696f;V3 anchor=flank(t0,(g?2.68f:2.04f)*d,n);
        auto paired=[=](float u,float v){float a=(-1.18f+.88f*u),r=(.020f+.012f*sinf(PI*u)+.004f*u)*v;
          return anchor+V3(-r*cosf(a),r*sinf(a)-.003f*v,d*(.002f+.016f*v+.0018f*sinf(u*5+v*3)*v));};
        addGrid(body,40,17,0,1,0,1,paired,fin);
      }
    }
    // The front ray rises steeply; successive rays rake aft towards the tail.
    addGrid(body,55,19,0,1,0,1,[&](float u,float v){float t=.105f+.448f*u;V3 p=flank(t,0,n);
      float h=.0015f+.034f*powf(std::max(0.f,sinf(PI*.58f*u)),1.88f)*(1.f-.33f*smooth(.88f,1.f,u));
      return p+V3(-.009f*v-.006f*sinf(PI*u)*v,h*v,.0026f*sinf(5*u+v*2)*v);},fin);
    addGrid(body,26,15,0,1,0,1,[&](float u,float v){V3 p=flank(.12f+.18f*u,PI,n);
      return p+V3(-.006f*v,-(.003f+.016f*sinf(PI*u))*v,.002f*sinf(u*4)*v);},fin);
    // Sample the trailing outline of each lobe from the fork to the outer tip
    // and back to its leading edge. Curved rays span from the fleshy caudal base.
    addGrid(tail,110,34,-1,1,0,1,[=](float u,float v){
      const float rear[]={.036f,.042f,.058f,.076f,.082f,.064f,.042f};
      const float rise[]={.001f,.009f,.019f,.028f,.034f,.036f,.031f};
      float q=std::min(fabsf(u)*6.f,5.9999f);int i=int(q);float f=q-i;
      auto spline=[&](const float* a){float A=a[std::max(0,i-1)],B=a[i],C=a[i+1],D=a[std::min(6,i+2)];return .5f*((2*B)+(-A+C)*f+(2*A-5*B+4*C-D)*f*f+(-A+3*B-3*C+D)*f*f*f);};
      float sign=u<0?-1:1, yy=.011f*u*(1-v)+sign*spline(rise)*v*(u<0?1.23f:1.f);
      return V3(-v*spline(rear),yy+.003f+.0023f*sinf(PI*v)*u,.004f*sinf(4*u+4*v)*v+.0035f*v*v*sinf(8*u));},fin);
    V3 mouth=flank(1,PI*.5f,n);mouth.z=0;
    soft(body,mouth+V3(-.0011f,.0011f,0),{.0032,.0023,.0045},lip,1);
    soft(body,mouth+V3(-.0009f,-.00145f,0),{.00285,.0016,.0043},lip,1);
    fishBody[n]=s.addProto(body);fishTail[n]=s.addProto(tail);
  }
}
