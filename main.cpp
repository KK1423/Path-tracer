#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <atomic>
#define Triangle = 4;
#define pi = 3.14159;
using namespace std;
class Material
{
public:
    float alpha,diffuseglossy,ior,r,g,b,emission;
    Material() {}
    Material(float a, float d,float iOr, int re,int gr, int bl)
    {
        alpha = a;
        diffuseglossy = d;
        r = re;
        g = gr;
        b = bl;
        ior = iOr;
        emission = 0;
    }
    Material(float a, float d,float iOr, int re,int gr, int bl,float em)
    {
        alpha = a;
        diffuseglossy = d;
        r = re;
        g = gr;
        b = bl;
        ior = iOr;
        emission = em;
    }
};
inline float rad(float degr)
{
    degr = (degr*0.017453);
    return degr;
}
inline float invsqrt( float number )
{
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y  = number;
    i  = * ( long * ) &y;                       // evil floating point bit level hacking
    i  = 0x5f3759df - ( i >> 1 );
    y  = * ( float * ) &i;
    y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//      y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

    return y;
}
inline float fastsqrt(float n)
{
    return n*invsqrt(n);
}
class d3Vector
{

public:
    float x,y,z;
    d3Vector(float X = 0,float Y = 0,float Z = 0):x(X), y(Y), z(Z) {}
    d3Vector scalarmultiply (float b)
    {
        return d3Vector((x*b),(y*b),(z*b));
    }
    d3Vector individualmultiply (d3Vector coefficients)
    {
        d3Vector result;
        result = d3Vector((x)*coefficients.x,(y)*coefficients.y,(z)*coefficients.z);
        return result;
    }
    float dot(d3Vector b)
    {
        return ((x*b.x)+(y*b.y)+(z*b.z));
    }
    float magnitude()
    {
        return fastsqrt(x*x+y*y+z*z);
    }
    float magnitudesq()
    {
        return (x*x+y*y+z*z);
    }
    d3Vector normalize()
    {
        float d = invsqrt(x*x+y*y+z*z);
        return d3Vector(x*d,y*d,z*d);
    };
    float project(d3Vector aO,d3Vector aV)
    {
        d3Vector b=subtract(aO);
        aV=aV.normalize();
        return (b.dot(aV));
    }
    d3Vector add(d3Vector a)
    {
        return d3Vector((x)+a.x,(y)+a.y,(z)+a.z);
    }
    d3Vector subtract(d3Vector a)
    {
        return d3Vector((x)-a.x,(y)-a.y,(z)-a.z);
    }
    d3Vector rotatearoundorgin(float zenith,float attitude)
    {
        d3Vector out(0,0,0);
        d3Vector in = *this;
        float cosz = cosf(rad(zenith));
        float sinz = sinf(rad(zenith));
        float cosy = cosf(rad(attitude));
        float siny = sinf(rad(attitude));
        out = d3Vector((cosy*cosz)*in.x-(sinz)*in.y-(siny*cosz)*in.z,(cosy*sinz)*in.x+(cosz)*in.y-(siny*cosz)*in.z,(siny)*in.x+(cosy)*in.z);
        return out;
    }
    d3Vector crossproduct(d3Vector a)
    {
        return d3Vector(y*a.z-z*a.y,z*a.x-x*a.z,x*a.y-y*a.x);
    }
};
class Sphere
{
public:
    float r,rsqr;
    d3Vector pos;
    Material material;
    Sphere * nextsphere;
    Sphere() {};
    Sphere(float R,float X,float Y,float Z,Material m): r(R)
    {
        material = m;
        pos.x = X;
        pos.y = Y;
        pos.z = Z;
        rsqr = r*r;
    };
};
class Scene
{
public:
    int passnumber = 0;
    int recipjitter = 3200;
    Sphere* spherepointer;
    int spherenum;
    d3Vector ambientcolor;
    Scene(int sphereQuantity,Sphere* spheres)
    {
        spherenum = sphereQuantity;
        spherepointer = spheres;
    }
};
struct hitdata
{
    bool hit;
    float t;
    d3Vector coord;
    d3Vector normal;
    Material material;
};
class ray
{
public:
    d3Vector O,V;
    ray(d3Vector o,d3Vector v) : O(o),V(v) {};
    d3Vector endpoint()
    {
        return O.add(V);
    }
    ray rotatearoundorgin(int zenith,int attitude)
    {
        return(ray(O.rotatearoundorgin(zenith,attitude),V.rotatearoundorgin(zenith,attitude)));
    }
    ray scalearound(ray cize)
    {
        d3Vector endpt = endpoint();
        d3Vector orgin = this->O;
        endpt = endpt.subtract(cize.O);
        orgin = orgin.subtract(cize.O);
        endpt = endpt.individualmultiply(cize.V);
        orgin = orgin.individualmultiply(cize.V);
        endpt = endpt.add(cize.O);
        orgin = orgin.add(cize.O);
        return ray(orgin,endpt.subtract(orgin));
    }
    hitdata intersectwith(Sphere sphere)
    {

        float t = V.dot(sphere.pos.subtract(O));
        hitdata outdata;
        outdata.hit = false;
        outdata.t = 110;
        if(t<=0)
        {
            return outdata;
        }
        float distnce = (V.normalize().scalarmultiply(t).subtract(sphere.pos.subtract(O))).magnitudesq();
        if(distnce<sphere.rsqr)
        {
            float deltaT = fastsqrt((sphere.rsqr)-(distnce));
            /*if(t<=deltaT)
            {
                if(t>-deltaT)
                {
                    deltaT = (*(int*)&deltaT)^2147483648;
                }
                else
                {
                    return outdata;
                }
            }*/
            outdata.hit = true;
            outdata.t = t-deltaT;
        }
        return outdata;
    }
    hitdata intersectwith(Scene scene)
    {
        float lowestt = 1005;
        hitdata lowesthit;
        hitdata hitcache;
        Sphere hitsphere;
        lowesthit.hit = false;
        Sphere * sphereptr = scene.spherepointer;
        while(sphereptr!=0)
        {
            hitcache = intersectwith(Sphere(*sphereptr));
            if(hitcache.hit)
            if(hitcache.t<lowestt)
            {
                lowesthit=hitcache;
                lowestt=hitcache.t;
                hitsphere = *sphereptr;
            }
            sphereptr = sphereptr->nextsphere;
        }
        lowesthit.material = hitsphere.material;
        lowesthit.coord = O.add(V.scalarmultiply(lowestt));
        lowesthit.normal = lowesthit.coord.subtract(hitsphere.pos).normalize();//.scalarmultiply(deltaT<0?-1:1);
        return lowesthit;
    }
};
int p;
int *g_seed = new int(clock()*p);
 int getsignedrand() {
    *g_seed = (214013**g_seed+2531011);
    return ((*g_seed>>16)&0x7FFF)%400;
}
int gesignedrand()
{
    int returnvalue;
    int randState;
    randState = std::hash<std::thread::id>()(std::this_thread::get_id())^(int64_t)randState;
    returnvalue = (randState%400)*(randState&1?1:-1);
    return returnvalue;
}
ray getCameraRay (int x, int y, int width, int height, float recipjitter, float flength, float fdistance)
{

    float xfraction = ((float)x)/((float)width);
    float yfraction = ((float)y)/((float)height);
    xfraction-=0.65;
    yfraction-=0.5;
    d3Vector finaldirection(-flength,xfraction,-yfraction);
    finaldirection = finaldirection.normalize();
    d3Vector cache = finaldirection.scalarmultiply(fdistance);
    d3Vector startdiff(getsignedrand()/float(recipjitter),getsignedrand()/float(recipjitter),getsignedrand()/float(recipjitter));
    finaldirection = cache.subtract(startdiff);
    //finaldirection = finaldirection.rotatearoundorgin(0,0);
    finaldirection = finaldirection.normalize();
    return ray(d3Vector(9,0,2).add(startdiff),finaldirection);

}
d3Vector getcolor(Material m)
{
    return d3Vector(m.r,m.g,m.b);
}
d3Vector get_glossy_color(int depth,hitdata objecthit,ray cameraray,Scene scen);
d3Vector get_diffuse_color(int depth,hitdata objecthit,ray cameraray,Scene scen);
d3Vector get_transmit_color(int depth,hitdata objecthit,ray cameraray,Scene scen);
d3Vector trace(ray inray,Scene scene,int depth)
{
    hitdata objecthit;
    //cout<<"k"<<endl;
    objecthit = inray.intersectwith(scene);
    if((!objecthit.hit)||(depth==0))
    {
        return scene.ambientcolor;
    }
    d3Vector outcolor;
    /* if(objecthit.material.r==84)
     {
         return get_transmit_color(depth,objecthit,inray,scene);
     }*/
    outcolor = get_diffuse_color(depth,objecthit,inray,scene);
    outcolor = outcolor.scalarmultiply((float)1 - objecthit.material.diffuseglossy);
    outcolor = (outcolor).add(get_glossy_color(depth,objecthit,inray,scene).scalarmultiply(objecthit.material.diffuseglossy));
    //objecthit.normal = objecthit.normal.normalize();
    //return objecthit.normal.scalarmultiply(255);
    return outcolor.add(getcolor(objecthit.material).scalarmultiply(objecthit.material.emission));
}
int nth = 2;
d3Vector cosweightedpointinhemisphere()
{
    float r = getsignedrand()*0.0025;
    float t = getsignedrand()*0.01570796;
    d3Vector discpoint(r*sinf(t),r*cosf(t),0);
    discpoint.z = fastsqrt(1.0-((discpoint.x*discpoint.x)+(discpoint.y*discpoint.y)));
    return discpoint;
}
d3Vector diffusevec(d3Vector normal)
{
    d3Vector hemispherepoint = cosweightedpointinhemisphere();
    d3Vector tangent(normal.y,-1.0*normal.x,0);
    d3Vector bitangent = normal.crossproduct(tangent);
    tangent = tangent.normalize();
    bitangent = bitangent.normalize();
    //matrix multiplication
    return (bitangent.scalarmultiply(hemispherepoint.x)).add((tangent.scalarmultiply(hemispherepoint.y)).add(normal.scalarmultiply(hemispherepoint.z)));
}
d3Vector get_diffuse_color(int depth,hitdata objecthit,ray cameraray,Scene scen)
{
    return trace(ray(objecthit.coord,diffusevec(objecthit.normal)),scen,depth-1).individualmultiply((getcolor(objecthit.material)).scalarmultiply(1/255.0));
}
d3Vector get_glossy_color(int depth,hitdata objecthit,ray cameraray,Scene scen)
{
    cameraray.V = cameraray.V.normalize();
    ray nextray(d3Vector(0,0,0),d3Vector(0,0,0));
    nextray.O = objecthit.coord.add(objecthit.normal.scalarmultiply(0.01));
    float f = -2.0*cameraray.V.dot(objecthit.normal);
    nextray.V = cameraray.V.add(objecthit.normal.scalarmultiply(f));
    nextray.V = nextray.V.add((d3Vector(getsignedrand()/8000.0,getsignedrand()/8000.0,getsignedrand()/8000.0)));
    d3Vector objectcolor(objecthit.material.r,objecthit.material.g,objecthit.material.b);
    return objectcolor.individualmultiply(trace(nextray,scen,depth - 1).scalarmultiply(0.003921568627));
}
d3Vector get_transmit_color(int depth,hitdata objecthit,ray cameraray,Scene scen)
{
    float n = 1.0;
    float cosI = objecthit.normal.dot(cameraray.V);
    float sinT2 = n*n*(1.0 - cosI*cosI);
    if(sinT2>1.0)
    {
        return get_glossy_color(depth,objecthit,cameraray,scen);
    }
    float scalar = n+fastsqrt(1.0-sinT2);
    d3Vector outv =(cameraray.V.scalarmultiply(n).subtract(objecthit.normal.scalarmultiply(scalar)));
    return trace(ray(objecthit.coord.add(outv.scalarmultiply(0.01)),outv),scen,depth-1);
}
Scene generateScene ()
{
    Sphere *spheres[] =
    {
        new Sphere(1, 3.7,-0.8,0, Material(1,0.8,1.4,84,255,255)),
        new Sphere(2, 0,2.0,-0.2, Material(1,0.6,1.4,190,64,190)),
        new Sphere(3, -2,-3,0, Material(1,0,0.5,255,255,76)),
        //new Sphere(1, -1.8,0.04,3, Material(1,0,0,255,255,255,2))
        new Sphere(10, 0,0,15, Material(1,0,0,255,255,255,5)),
        new Sphere(1,  1,0,2, Material(1,0.93,0.5,240,255,240)),
        new Sphere(0.1,  3,1,2, Material(1,0.93,0.5,240,255,240))
    };
    for(int i = 0; i<=4 ; i++)
    {
        spheres[i]->nextsphere = spheres[i+1];
    }
    spheres[5]->nextsphere = 0;
    Scene proto(4,spheres[0]);
    return proto;

}
Scene sce(generateScene());
d3Vector normalizeColor(d3Vector in)
{
    return d3Vector(in.x>255?255:in.x,in.y>255?255:in.y,in.z>255?255:in.z);
}
d3Vector computePixel (int x,int y,Scene seed,d3Vector old)
{
    //cout<<"k"<<endl;
    //static int64_t passnumber = 0;
    int pass = seed.passnumber;
    d3Vector Pixel;
    d3Vector Tracedcolor =trace(getCameraRay(x,y,1000,1000,3200,1,8),seed,5) ;
    Pixel = ((old.scalarmultiply(pass)).add(Tracedcolor)).scalarmultiply(1.0/(float)(1+pass));
    return Pixel;
}//trace(getCameraRay(x,y,1000,1000,0,1,3),seed,5)
void dump(d3Vector **inImage,int width,int height)
{
    ifstream cachefile("cache");
    if(!cachefile.is_open())
    {
        cachefile.close();
        ofstream cacheout("cache");
        int d = 0;
        cacheout.write(reinterpret_cast<char*>(&d),1);
        cacheout.close();
        dump(inImage,width,height);
        return;
    }
    char str;
    cachefile.read(&str,1);
    cachefile.close();
    string str2 = "Raytraceimage1.ppm";
    str2[13]=(int)str+48;
    ofstream outputFile(str2,std::ios_base::openmode::_S_bin);
    outputFile<<"P6 "<<endl
    <<width<<" "<<height<<" 255"<<endl;
    for(int i = 0; i<height; i++)
    {
        for(int j = 0; j<width; j++)
        {
            d3Vector outdata= normalizeColor(inImage[j][i]);
            int a = outdata.x;
            int b = outdata.y;
            int c = outdata.z;//normalizeColor(inImage[j][i]);
            outputFile.write(reinterpret_cast<char*>(&a),1);
           // outputFile<<" ";
            outputFile.write(reinterpret_cast<char*>(&b),1);
            //outputFile<<" ";
            outputFile.write(reinterpret_cast<char*>(&c),1);
            //outputFile<<" ";
        }
        //outputFile<<" ";

    }
    outputFile<<endl;
    outputFile.close();
    outputFile.open("cache");
    str = (char)((int)str+1);
    outputFile.write(&str,1);
    str2 = "start \"\"\"%programfiles(x86)%\\GimpShop\\bin\\gimp-2.8.exe\" "+str2;
    system(str2.c_str());
    cin>>str2;
}
void computeSection(int xstart,int xend, int ystart, int yend,d3Vector **opImage,Scene sce)
{
    int x = xstart;
    int y = ystart;
    p = sce.passnumber;
    while(y<=yend)
    {
        while(x<=xend)
        {
            opImage[x][y]=computePixel(x,y,sce,opImage[x][y]);
            x++;
        }
        x = xstart;
        y++;
    }
}
int main()
{
    int samples;
    cin>>samples;
    //if(samples == 1){
      //  system("start Raytracer2.exe");return 0;}
    Scene sce = generateScene();
    sce.recipjitter = 4000;
    Scene sce2 = sce;
    Scene sce3 = sce2;
    Scene sce4 = sce3;
    sce.ambientcolor = d3Vector(0,0,0);
    srand(5);
    srand(rand());
    d3Vector **regimage = new d3Vector*[1300];
    for(int i = 0; i<1300; i++)
    {
        regimage[i] = new d3Vector[1000];
        for(int j = 0; j<1000; j++)
        {
            regimage[i][j] = 0;//computePixel(i,j,sce,d3Vector(0,0,0));
        }

    }
    srand(rand());
    float t = 0;
    while (sce.passnumber<samples)
    {
        cout<<clock()-t<<endl;
        t = clock();
        p = sce.passnumber;
        thread section2(computeSection,501,1299,0,500,regimage,sce);
        thread section3(computeSection,651,1299,501,999,regimage,sce2);
        thread section4(computeSection,0,650,501,999,regimage,sce3);
        computeSection(0,650,0,500,regimage,sce4);
        section2.join();
        section3.join();
        section4.join();
        sce.passnumber++;
        sce2.passnumber++;
        sce3.passnumber++;
        sce4.passnumber++;
        cout<<sce.passnumber<<endl;
    }
    dump(regimage,1299,999);
    return 0;
}


