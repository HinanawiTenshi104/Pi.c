#include<stdio.h>
#include<string.h>
#include<math.h>
int W=60; int H=40;float L=15,R=1.5,Cy=0.5,Ll=6,Lr=1.5,Rl=3.6,Lg=4,Ar=2,K1=30,K2=15,Su=0.0314,Sv=
0.0314,Sa=0.0314,Sv2=0.0314,A,B;char*S=".,-~:;=!*#$@";float D(float x,float y,float z){float a=0
,b=1,c=-1,l=sqrtf(a*a+b*b+c*c);a/=l;b/=l;c/=l;float d=x*a+y*b+z*c;return d<0?0:d;}void E(float x,
float y,float z,float a,float b,float*xp,float*yp,float*zp){float X=x,Y=y*cos(b)-z*sin(b),Z=y*sin
(b)+z*cos(b);*xp=X*cos(a)+Z*sin(a);*yp=Y;*zp=-X*sin(a)+Z*cos(a);}int main(){char o[W*H];float z[W
*H];while(1){memset(o,' ',W*H);for(int i=0;i<W*H;i++)z[i]=1e9;for(float u=0;u<6.28;u+=Su){for(float 
v=0;v<1;v+=Sv){float x=-L/2+v*L,y=R*cos(u)+Cy,z1=R*sin(u),nx=0,ny=cos(u),nz=sin(u),xp,yp,zp,nxp,//
nyp,nzp;E(x,y,z1,A,B,&xp,&yp,&zp);E(nx,ny,nz,A,B,&nxp,&nyp,&nzp);float iz=1/(zp+K2);int px=W/2+K1*
xp*iz,py=H/2-K1*yp*iz,l=D(nxp,nyp,nzp)*(strlen(S)-1);if(px>=0&&px<W&&py>=0&&py<H&&zp+K2<z[py*W+px])
{z[py*W+px]=zp+K2;o[py*W+px]=S[l];}}}for(float u=0;u<6.28;u+=Su){for(float v=0;v<1;v+=Sv){float x=
              Lr*cos(u)-Lg,y=Cy-v*Ll,z1=            Lr*sin(u),nx=cos(u),ny=0,
              nz=sin(u),xp,yp,zp,nxp,nyp            ,nzp;E(x,y,z1,A,B,&xp,&yp
              ,&zp);E(nx,ny,nz,A,B,&nxp,            &nyp,&nzp);float iz=1/(zp
              +K2);int px=W/2+K1*xp*iz,             py=H/2-K1*yp*iz,l=D(nxp,
              nyp,nzp)*(strlen(S)-1);if             (px>=0&&px<W&&py>=0&&py<H
              &&zp+K2<z[py*W+px]){z[py*             W+px]=zp+K2;o[py*W+px]=S[
              l];}}}for(float u=0;u<6.28            ;u+=Su){for(float v=0;v<1;
              v+=Sv){float x=Lr*cos(u)+             Lg,y=Cy-v*Rl,z1=Lr*sin(u),
              nx=cos(u),ny=0,nz=sin(u),             xp,yp,zp,nxp,nyp,nzp;E(x,y,
              z1,A,B,&xp,&yp,&zp);E(nx,             ny,nz,A,B,&nxp,&nyp,&nzp);
              float iz=1/(zp+K2);int px             =W/2+K1*xp*iz,py=H/2-K1*yp
              *iz,l=D(nxp,nyp,nzp)*(/**/             strlen(S)-1);if(px>=0&&px<
              W&&py>=0&&py<H&&zp+K2<z[py             *W+px]){z[py*W+px]=zp+K2;o[
              py*W+px]=S[l];}}}float Yb=              Cy-Rl;for(float a=0;a<1.57;
              a+=Sa){float Xc=Lg+Ar*(1-                 cos(a)),Yc=Yb-Ar*sin(a);for
              (float v=0;v<6.28;v+=Sv2)                  {float nx=cos(v),nz=sin(v),x=
              Xc+Lr*nx,y=Yc,z1=Lr*nz,xp,                   yp,zp,nxp,nyp,nzp;E(x,y,z1,A,B
              ,&xp,&yp,&zp);E(nx,0,nz,A,                    B,&nxp,&nyp,&nzp);float iz=1/(
              zp+K2);int px=W/2+K1*xp*iz                      ,py=H/2-K1*yp*iz,l=D(nxp,nyp,nzp
              )*(strlen(S)-1);if(px>=0&&                        px<W&&py>=0&&py<H&&zp+K2<z[py*W+
              px]){z[py*W+px]=zp+K2;o[py                          *W+px]=S[l];}}}system("cls");for
              (int j=0;j<H;j++){for(int                               i=0;i<W;i++)putchar(o[j*W+i]);
              putchar('\n');}A+=0.1;B+=                                    0.002;}}/*Made by PH 20250314*/
