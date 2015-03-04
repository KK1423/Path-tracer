#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <atomic>
//#include <x86intrin.h>
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
    return sqrt(n);
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
    float get(int &a)
    {
        switch(a)
        {
        case 1:
            return this->x;
        case 2:
            return this->y;
        case 3:
            return this->z;
        }
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
d3Vector max(d3Vector a, d3Vector b)
{
    if(a.x<b.x)
        a.x = b.x;
    if(a.y<b.y)
        a.y = b.y;
    if(a.z<b.z)
        a.z = b.z;
    return a;
}
d3Vector max(d3Vector a, d3Vector b,d3Vector c)
{
    return max(a,max(b,c));
}
d3Vector min(d3Vector a, d3Vector b)
{
    if(a.x>b.x)
        a.x = b.x;
    if(a.y>b.y)
        a.y = b.y;
    if(a.z>b.z)
        a.z = b.z;
    return a;
}
d3Vector min(d3Vector a, d3Vector b,d3Vector c)
{
    return min(a,min(b,c));
}
class TriBVHData
{
public:
    d3Vector ma,mi;
    d3Vector center;
    TriBVHData(){};
};
class Triangle
{
public:
    TriBVHData* BVHdata;
    d3Vector a,b,c;
    Material material;
    bool notnullflag = true;
    Triangle() {};
    Triangle(int a)
    {
        notnullflag = false;
    };
    Triangle(d3Vector a_, d3Vector b_, d3Vector c_, Material m_): a(a_), b(b_),c(c_),material(m_)
    {

        BVHdata = new TriBVHData;
        BVHdata->center = (a.add(b).add(c)).scalarmultiply(0.333333333333);
        BVHdata->ma = max(a,b,c);
        BVHdata->mi = min(a,b,c);

    };
    float surfaceArea()
    {
        return 0.5*(((b.subtract(a)).crossproduct(c.subtract(a))).magnitude());
    }
};
class Scene
{
public:
    int passnumber = 0;
    int recipjitter = 3200;
    Sphere* spherepointer;
    int spherenum;
    Triangle* trianglepointer;
    int trianglenum;
    d3Vector ambientcolor;
    Scene(int sphereQuantity,Sphere* spheres, int trianglenum_, Triangle* triangles):spherenum(sphereQuantity),spherepointer(spheres),trianglepointer(triangles),trianglenum(trianglenum_) {}
};

float* getx(Triangle &a)
{
    return &(a.BVHdata->center.x);
}
float* gety(Triangle &a)
{
    return &(a.BVHdata->center.y);
}
float* getz(Triangle &a)
{
    return &(a.BVHdata->center.z);
}
d3Vector maximum(Triangle* a,int length)
{
    d3Vector out = a->BVHdata->ma;
    for(int i=1;i<length;i++)
    {
        out = max(out,a[i].BVHdata->ma);
    }
    return out;
}
d3Vector minimum(Triangle* a,int length)
{
    d3Vector out = a->BVHdata->mi;
    for(int i=1;i<length;i++)
    {
        out = min(out,a[i].BVHdata->mi);
    }
    return out;
}
#include "LinkedList.cpp"
class BVHNode
{
public:
    BVHNode* left = 0;
    BVHNode* right = 0;
    int l;
    d3Vector ma,mi;
    bool isleaf = false;
    Triangle* array = 0;
    BVHNode(Triangle* in, int length)
    {
        if(length<3)
        {
            isleaf = true;
            array = in;
            l = length;
        }
        d3Vector mi = minimum(in,length);
        d3Vector ma = maximum(in,length);
        d3Vector d = ma.subtract(mi);
        int axis = (d.x>d.y&&d.x>d.z)?(1):((d.y>d.x&&d.y>d.z)?(2):(3));
        Sorttris(in,axis == 1?(getx):((axis == 2)?(gety):(getz)));
        float comparision = (mi.add(d.scalarmultiply(0.5))).get(axis);
        int i = 0;
        while(in[i++].BVHdata->center.get(axis)){};
        i--;
        left = new BVHNode(in,i);
        right = new BVHNode(in+i,length-i);
    }
};
class BVH
{
public:
    int passnumber = 0;
    int recipjitter = 3200;
    Sphere* spherepointer;
    BVHNode* root = 0;
    d3Vector ambientcolor = d3Vector(0,0,0);
    BVH(Scene &in)
    {
        spherepointer = in.spherepointer;
        int count = 0;
        Triangle* a = in.trianglepointer;
        while(a->notnullflag)
        {count++;
        a++;}
        root = new BVHNode(in.trianglepointer,count);
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
    inline float intersectwith(Sphere &sphere)
    {

        float t = V.dot(sphere.pos.subtract(O));
        if(t<=0)
        {
            return 0.0;
        }
        float distnce = (V.normalize().scalarmultiply(t).subtract(sphere.pos.subtract(O))).magnitudesq();
        if(distnce<sphere.rsqr)
        {
            float deltaT = fastsqrt((sphere.rsqr)-(distnce));
            if(deltaT+0.1>=t)
            {
                deltaT *=-1;
            }
            return t-deltaT;
        }
        else
        {
            return 0.0;
        }
    }
    float intersectwith(Triangle triangle)
    {
        // Algorithm credit to Tomas Moller and Ben Trumbore
        d3Vector edge1 = triangle.b.subtract(triangle.a);
        d3Vector edge2 = triangle.c.subtract(triangle.a);
        d3Vector pvec = V.crossproduct(edge2);
        float invdet = edge1.dot(pvec);
        if(invdet == 0)return 0;
        invdet = 1.0/invdet;
        d3Vector tvec = O.subtract(triangle.a);
        float u = tvec.dot(pvec) * invdet;
        if(u<0||u>1)return 0;
        d3Vector qvec = tvec.crossproduct(edge1);
        float v = V.dot(qvec) * invdet;
        if(v<0||u+v>1)return 0;
        return (edge2.dot(qvec))*invdet;
    }
    hitdata intersectwith(Scene &scene)
    {
        float lowestt = 1005;
        float hitcache;
        bool sphere = true;
        Sphere * hitsphere;
        Triangle * hittriangle;
        Sphere * sphereptr = scene.spherepointer;
        Triangle * triangleptr = scene.trianglepointer;
        while(sphereptr)
        {
            hitcache = intersectwith(*sphereptr);
            if(hitcache)
                if(hitcache<lowestt)
                {
                    lowestt = hitcache;
                    hitsphere = sphereptr;
                }
            sphereptr = sphereptr->nextsphere;
        }
        while(triangleptr->notnullflag)
        {
            hitcache = intersectwith(*triangleptr);
            if(hitcache>0)
                if(hitcache<lowestt)
                {
                    lowestt=hitcache;
                    hittriangle = triangleptr;
                    sphere = false;
                }
            triangleptr++;
        }
        hitdata lowesthit;
        if(lowestt==1005)
        {
            lowesthit.hit = false;
            return lowesthit;
        };
        lowesthit.hit = true;
        lowesthit.coord = O.add(V.scalarmultiply(lowestt));
        if(sphere)
        {
            lowesthit.material = hitsphere->material;
            lowesthit.normal = lowesthit.coord.subtract(hitsphere->pos).normalize();//.scalarmultiply(deltaT<0?-1:1);
        }
        else
        {
            lowesthit.material = hittriangle->material;
            lowesthit.normal = (hittriangle->b.subtract(hittriangle->a)).crossproduct(hittriangle->c.subtract(hittriangle->a)).normalize();
        }
        //lowesthit.material.diffuseglossy = sphere?1:0;
        return lowesthit;
    }
    bool intersectwith(d3Vector mi,d3Vector ma)
    {
        float temp;
        float tmin = (mi.x-O.x)/V.x;
        float tmax = (ma.x-O.x)/V.x;
        if(tmin>tmax)
        {
            temp = tmax;
            tmax = tmin;
            tmin = temp;
        }
        float tymin = (mi.y-O.y)/V.y;
        float tymax = (ma.y-O.y)/V.y;
        if(tymin>tymax)
        {
            temp = tymax;
            tymax = tymin;
            tymin = temp;
        }
        if((tmin>tymax)||(tymin>tmax)) return false;
        if(tymin>tmin)tmin = tymin;
        if(tymax<tmax)tmax = tymax;
        float tzmin = (mi.z-O.z)/V.z;
        float tzmax = (ma.z-O.z)/V.z;
        if (tzmin > tzmax)
        {
            temp = tzmax;
            tzmax = tzmin;
            tzmin = temp;
        }
        if ((tmin > tzmax) || (tzmin > tmax))return false;
        if (tzmin > tmin)tmin = tzmin;
        if (tzmax < tmax)tmax = tzmax;
        if (tmax <= 0) return false;
        return true;
    }
    hitdata intersectwith(BVHNode &in)
    {
        hitdata cache;
        cache.hit = false;
        if(in.isleaf)
        {
            hitdata lowesthit;
            float lowestt = 1e10;
            float hitt;
            Triangle* hittriangle;
            lowesthit.hit = false;

            for(int i = 0;i<in.l;i++)
            {
                hitt = intersectwith(in.array[i]);
                if(hitt)
                    if(hitt<lowestt)
                    {
                        hittriangle = in.array+i;
                        lowestt = hitt;
                    }
            }
            if(lowestt = 1e10)return cache;
            cache.hit = true;
            cache.coord = O.add(V.scalarmultiply(lowestt));
            cache.material = hittriangle->material;
            cache.normal = (hittriangle->b.subtract(hittriangle->a)).crossproduct(hittriangle->c.subtract(hittriangle->a)).normalize();
            cache.t = lowestt;
            return cache;

        }
        hitdata cache2;
        cache2.hit = false;
        if(intersectwith(in.left->mi,in.left->ma))
        {
            cache = intersectwith(*in.left);
        }
        if(intersectwith(in.right->mi,in.right->ma))
        {
            cache2 = intersectwith(*in.right);
            if(cache2.hit)
            {
                if(cache2.t<cache.t)
                    cache = cache2;
            }
        }
        return cache;
    }
    hitdata intersectwith(BVH &in)
    {
        Sphere* sphereptr = in.spherepointer;
        Sphere* hitsphere = 0;
        float lowestt = 1005;
        float hitcache;
        while(sphereptr)
        {
            hitcache = intersectwith(*sphereptr);
            if(hitcache)
                if(hitcache<lowestt)
                {
                    lowestt = hitcache;
                    hitsphere = sphereptr;
                }
            sphereptr = sphereptr->nextsphere;
        }
        hitdata BVHhit = intersectwith(*in.root);
        if(hitsphere == 0&&!BVHhit.hit)
        {
            hitdata f;
            f.hit =false;
            return f;
        }
        if(lowestt<BVHhit.t&&hitsphere!=0)
        {
            hitdata spherehit;
            spherehit.coord = O.add(V.scalarmultiply(lowestt));
            spherehit.hit = true;
            spherehit.normal = spherehit.coord.subtract(hitsphere->pos).normalize();
            spherehit.material = hitsphere->material;
            return spherehit;
        }
        if(lowestt>BVHhit.t&&BVHhit.hit)
        {
            return BVHhit;
        }
    }
};
#include "FileLoader.cpp"
int p;
int *g_seed = new int(clock()*p);
int getsignedrand()
{
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
    //finaldirection = cache.subtract(startdiff);
    //finaldirection = finaldirection.rotatearoundorgin(0,0);
    finaldirection = finaldirection.normalize();
    return ray(d3Vector(9,0,2)/*.add(startdiff)*/,finaldirection);

}
d3Vector getcolor(Material m)
{
    return d3Vector(m.r,m.g,m.b);
}
d3Vector get_glossy_color(int depth,hitdata objecthit,ray cameraray,BVH &scen);
d3Vector get_diffuse_color(int depth,hitdata objecthit,ray cameraray,BVH &scen);
d3Vector get_transmit_color(int depth,hitdata objecthit,ray cameraray,BVH &scen, float ior);
d3Vector trace(ray inray,BVH &scene,int depth)
{
    hitdata objecthit;
    //cout<<"k"<<endl;
    objecthit = inray.intersectwith(scene);

    if((!objecthit.hit)||(depth==0))
    {
        return scene.ambientcolor;
    }
    d3Vector outcolor;
    //return objecthit.material.diffuseglossy>0.5?d3Vector(0,0,255):d3Vector(255,0,0);
    /* if(objecthit.material.r==84)
     {
         return get_transmit_color(depth,objecthit,inray,scene);
     }*/
    if(false)//objecthit.material.alpha<0.1)
    {
        return get_transmit_color(depth,objecthit,inray,scene,1.1);
    }
    outcolor = get_diffuse_color(depth,objecthit,inray,scene);
    if(objecthit.material.diffuseglossy!=1)
    {
        outcolor = outcolor.scalarmultiply((float)1 - objecthit.material.diffuseglossy);
        outcolor = (outcolor).add(get_glossy_color(depth,objecthit,inray,scene).scalarmultiply(objecthit.material.diffuseglossy));
    } //objecthit.normal = objecthit.normal.normalize();
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
d3Vector get_diffuse_color(int depth,hitdata objecthit,ray cameraray,BVH &scen)
{
    return trace(ray(objecthit.coord,diffusevec(objecthit.normal)),scen,depth-1).individualmultiply((getcolor(objecthit.material)).scalarmultiply(1/255.0));
}
d3Vector get_glossy_color(int depth,hitdata objecthit,ray cameraray,BVH &scen)
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
bool insideflag = false;
d3Vector get_transmit_color(int depth,hitdata objecthit,ray cameraray,BVH &scen,float ior = 1.3)
{
    cameraray.V = cameraray.V.normalize();
    d3Vector normal = objecthit.normal;
    ior = objecthit.material.ior;
    float cosI = -normal.dot(cameraray.V);
    float n = cosI<0?ior:1/ior;
    bool flag = insideflag;
    if(cosI<0)
    {
        cosI = -cosI;
    }
    else
    {
        insideflag = true;
    }
    float cosT2 = 1.0-(n*n*(1.0-(cosI*cosI)));
    if(cosT2>0)
    {
        d3Vector outv =  (cameraray.V.scalarmultiply(n)).add((normal).scalarmultiply((n*cosI)-(fastsqrt(cosT2))));
        //Beer's law
        if(flag)
        {
            d3Vector transmitcolor = trace(ray(objecthit.coord.add(outv.scalarmultiply(0)),outv.normalize()),scen,depth);
            float coefficient = -(cameraray.O.subtract(objecthit.coord)).magnitude();
            d3Vector color = getcolor(objecthit.material).scalarmultiply(0.02 * coefficient);
            d3Vector multipliers(powf(2.7,color.x),powf(2.7,color.y),powf(2.7,color.z));
            insideflag = false;
            return transmitcolor.individualmultiply(multipliers);
        }
        return trace(ray(objecthit.coord.add(outv.scalarmultiply(0)),outv.normalize()),scen,depth);
    }
    return get_glossy_color(depth,objecthit,cameraray,scen);
}
Scene generateScene (char* str)
{
    Scene proto(readFile(str));
    cout<<(proto.trianglepointer[0].notnullflag?"true":"false")<<endl;
    cout<<proto.trianglepointer<<endl;
    Sphere *spheres[] =
    {
        new Sphere(10, 0,0,15, Material(1,0,0,255,255,255,5)),
        new Sphere(1, 0,0,100, Material(1,0.8,1.4,84,255,255)),
        new Sphere(1, 3.7,-0.8,0, Material(1,0.8,1.4,84,255,255)),
        new Sphere(2, 0,2.0,-0.2, Material(1,0.6,1.4,190,64,190)),
        new Sphere(3, -2,-3,0, Material(1,0,0,255,255,76)),
        //new Sphere(1, -1.8,0.04,3, Material(1,0,0,255,255,255,2))
        new Sphere(1,  5,0,1.5, Material(0,0.93,1.0,0,250,0))
    };
    for(int i = 0; i<=3 ; i++)
    {
        spheres[i]->nextsphere = spheres[i+1];
    }
    spheres[1]->nextsphere = 0;
    proto.spherepointer = spheres[0];
    return proto;

}
Scene sce(0,0,0,0);
d3Vector normalizeColor(d3Vector in)
{
    return d3Vector(in.x>255?255:in.x,in.y>255?255:in.y,in.z>255?255:in.z);
}
d3Vector computePixel (int x,int y,BVH seed,d3Vector old)
{
    //cout<<"k"<<endl;
    //static int64_t passnumber = 0;
    int pass = seed.passnumber;
    d3Vector Pixel;
    d3Vector Tracedcolor =trace(getCameraRay(x,y,1000,1000,3200,1,8),seed,3) ;
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
void computeSection(int xstart,int xend, int ystart, int yend,d3Vector **opImage,BVH sce)
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
}/*
int main(int argv,char* argc[])
{
    Scene sce = generateScene("tracercube.obj");
    Triangle* tris = sce.trianglepointer;
    Sorttris(tris,getx);
    for(int i = 0; i<300; i++)
    {
        cout<<*getx(tris[i])<<endl;
        if(i!=299)
            if(*getx(tris[i+1])>=*getx(tris[i]))
        {

        }else
        {
         cout<<"fail";
        }
    }
    int a;
    cin>>a;
    return 0;
}*/

int main(int argv,char* argc[])
{
    //if(samples == 1){
    //  system("start Raytracer2.exe");return 0;}

    Scene sce(0,0,0,0);
    if(argv>=2)
    {
        sce = generateScene(argc[1]);
    }
    else
    {
        sce = generateScene("tracercube.obj");
    }
    BVH renderBVH(sce);
    //Scene sce = generateScene("tracercube.obj");
    int samples;
    cin>>samples;
    sce.recipjitter = 40000000;
    BVH sce2 = sce;
    BVH sce3 = sce2;
    BVH sce4 = sce3;
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
        cout<<"\r"<<clock()-t<<"  ";
        t = clock();
        p = sce.passnumber;
        thread section2(computeSection,501,1299,0,500,regimage,renderBVH);
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
        cout<<sce.passnumber;
    }
    dump(regimage,1299,999);
    return 0;
}
