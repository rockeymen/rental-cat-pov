// Camera marks and the toy's route, in metres. The turn out of the aquarium
// passes to the left of its stand before the cat crosses the rug.
#pragma once
namespace Film {
struct Mark {float t;V3 p;};
template<size_t N> static V3 sample(const Mark (&m)[N],float t){
  if(t<=m[0].t)return m[0].p;
  if(t>=m[N-1].t)return m[N-1].p;
  size_t i=0;while(t>m[i+1].t)++i;
  size_t j=i+1;
  float dt=m[j].t-m[i].t,u=(t-m[i].t)/dt,u2=u*u,u3=u2*u;
  auto tangent=[&](size_t k){
    if(k==0||k==N-1)return V3();
    V3 a=(m[k].p-m[k-1].p)/(m[k].t-m[k-1].t),b=(m[k+1].p-m[k].p)/(m[k+1].t-m[k].t);
    V3 v;for(int axis=0;axis<3;axis++)v[axis]=a[axis]*b[axis]>0?2*a[axis]*b[axis]/(a[axis]+b[axis]):0;
    return v;
  };
  return m[i].p*(2*u3-3*u2+1)+m[j].p*(-2*u3+3*u2)+dt*(tangent(i)*(u3-2*u2+u)+tangent(j)*(u3-u2));
}
struct Kick {float time,duration; V3 start,end;};
static const Kick kicks[]={
  {1.25f,2.10f,{-.19f,.076f,2.06f},{-.43f,.076f,.91f}},
  {4.00f,2.36f,{-.43f,.076f,.91f},{1.27f,.076f,-.45f}}
};
static V3 toy(float t){
  const Kick& k=kicks[t<kicks[1].time?0:1];
  float elapsed=std::max(0.f,t-k.time),u=clampf(elapsed/k.duration,0,1);
  V3 p=lerp(k.start,k.end,u*(2-u)); // Impulse at contact, then rolling resistance.
  float hop=clampf(elapsed/.18f,0,1);p.y+=.022f*4*hop*(1-hop);
  // The rug is 24 mm above the floor; take its fringe on the second roll.
  p.y+=.024f*smooth(.28f,.40f,.54f-p.z);
  return p;
}
static M34 pawPose(float t){
  const Kick& k=kicks[t<2.7f?0:1];
  float dt=t-k.time;
  if(dt<-.53f || dt>.49f)return mTrans({0,-2,0});
  V3 f=norm(k.end-k.start),side={-f.z,0,f.x};
  const Mark swing[]={{-.53f,{.13f,-.015f,-.50f}},{-.22f,{.082f,.042f,-.26f}},
    {0,{0,0,0}},{.09f,{-.016f,.018f,.065f}},{.26f,{.042f,.043f,-.11f}},{.49f,{.14f,-.016f,-.50f}}};
  V3 b=sample(swing,dt);
  V3 tip=k.start-f*.070f+f*b.z+side*b.x;tip.y=.065f+b.y;
  return mTrans(tip)*mRotY(atan2f(-f.x,-f.z));
}
static CamPose camera(float t){
  static const Mark route[]={{0,{-.13,.272,2.93}},{.7,{-.14,.244,2.64}},{1.25,{-.13,.235,2.44}},
    {1.57,{-.13,.24,2.43}},{2.65,{-.28,.229,1.94}},{3.52,{-.58,.225,1.46}},
    {4.0,{-.65,.219,1.24}},{4.42,{-.62,.232,1.25}},{5.5,{.30,.222,.72}},{6.65,{1.01,.25,.12}},
    {7.52,{1.46,.49,-.27}},{8,{1.587,.559,-.348}},{9.7,{1.62,.573,-.327}},
    {10.45,{1.60,.50,-.18}},{11.3,{1.0,.26,-.23}},{12.12,{.58,.215,-.84}},
    {12.92,{.29,.195,-1.54}},{13.11,{.24,.151,-1.70}},{13.46,{.018,.91,-2.041}},
    {13.73,{-.15,.77,-2.32}},{13.85,{-.19,.682,-2.39}},{14.04,{-.21,.75,-2.43}},
    {15.4,{-.39,.735,-2.60}},{17,{-.41,.675,-2.66}},{20,{-.41,.661,-2.66}}};
  // Angles are unwrapped so turning back on the sofa never flips the lens.
  static const Mark lens[]={{0,{-4,-7,.90}},{.68,{-4,-11,.64}},{1.25,{-9,-20,.43}},
    {1.62,{-9,-12,.65}},{2.65,{-8,-7,.88}},{3.52,{12,-13,.57}},
    {4.0,{32,-18,.43}},{4.4,{43,-7,.88}},{5.5,{41,-5,1.1}},{6.65,{17,5,.91}},
    {7.52,{9,8,.51}},{8,{-3.2,5.9,.363}},{9.7,{14,4,.42}},{10.45,{-6,1,.70}},
    {11.3,{-49,1,1.70}},{12.12,{-35,6,1.45}},{12.92,{-30,14,.92}},{13.11,{-31,18,.91}},
    {13.46,{-38,-14,.66}},{13.85,{-64,-7,.80}},{14.45,{-108,0,1.35}},
    {15.4,{-143,-2,1.26}},{16.6,{-157.5,-1.4,1.18}},{20,{-157.5,-1.4,1.18}}};
  V3 pos=sample(route,t),l=sample(lens,t);
  float stride=smooth(.73,1.37,t)*(1-smooth(6.3,7.65,t))+.70f*smooth(10.65,11.52,t)*(1-smooth(12.75,13.08,t));
  float rhythm=2*PI*2.65f*t;
  pos.y+=stride*(.010f*sinf(rhythm)+.004f*sinf(2*rhythm));
  pos.x+=.009f*stride*sinf(rhythm*.5f);
  float rest=smooth(15.0f,17.f,t);pos.y+=.0025f*rest*sinf(t*2.35f);
  float yaw=l.x*PI/180, pitch=l.y*PI/180+.008f*stride*sinf(rhythm-.44f);
  V3 f(sinf(yaw)*cosf(pitch),sinf(pitch),-cosf(yaw)*cosf(pitch));
  V3 r=norm(cross(f,{0,1,0})),u=cross(r,f);
  float roll=.012f*stride*sinf(rhythm*.5f)-.101f*rest+.031f*expf(-sq((t-13.55f)/.27f));
  return {pos,r*cosf(roll)+u*sinf(roll),u*cosf(roll)-r*sinf(roll),f,l.z,.0055f-.0031f*smooth(5,7.6,t)+.0021f*smooth(10.5,13,t)};
}
static float lids(float t){return .12f*smooth(15.5,17,t)+1.18f*smooth(17.45,19.55,t);}
}
