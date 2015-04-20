#ifndef MAININCLUDED
#define MAININCLUDED
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <windows.h>
//#include <x86intrin.h>
#define pi = 3.14159;
class Material
{
public:
    float alpha,diffuseglossy,ior,r,g,b,emission;
    Material();
    Material(float a, float d,float iOr, int re,int gr, int bl);
    Material(float a, float d,float iOr, int re,int gr, int bl,float em);
};
inline float rad(float degr);
inline float invsqrt( float number );
inline float fastsqrt(float n);
class d3Vector
{

public:
    float x,y,z;
    d3Vector(float X = 0,float Y = 0,float Z = 0);
    d3Vector scalarmultiply (float b);
    d3Vector individualmultiply (d3Vector coefficients);
    float get(int a);
    float dot(d3Vector b);
    float magnitude();
    float magnitudesq();
    d3Vector normalize();
    float project(d3Vector aO,d3Vector aV);
    d3Vector add(d3Vector a);
    d3Vector subtract(d3Vector a);
    d3Vector rotatearoundorgin(float zenith,float attitude);
    d3Vector crossproduct(d3Vector a);
};
class Sphere
{
public:
    float r,rsqr;
    d3Vector pos;
    Material material;
    Sphere * nextsphere;
    Sphere();
    Sphere(float R,float X,float Y,float Z,Material m);
    ~Sphere();
};
//d3Vector max(d3Vector a, d3Vector b);
//d3Vector max(d3Vector a, d3Vector b,d3Vector c);
//d3Vector min(d3Vector a, d3Vector b);
//d3Vector min(d3Vector a, d3Vector b,d3Vector c);
class TriBVHData
{
public:
    d3Vector ma,mi;
    d3Vector center;
    TriBVHData();
};
class Triangle
{
public:
    TriBVHData* BVHdata;
    d3Vector a,b,c;
    Material material;
    d3Vector normal;
    bool notnullflag = true;
    Triangle();
    Triangle(int a);
    Triangle(d3Vector a_, d3Vector b_, d3Vector c_, Material m_);
    float surfaceArea();
};
class Scene
{
public:
    int passnumber = 0;
    int recipjitter = 3200;
    Sphere* spherepointer = 0;
    int spherenum;
    Triangle* trianglepointer;
    int trianglenum;
    d3Vector ambientcolor;
    Scene(int sphereQuantity,Sphere* spheres, int trianglenum_, Triangle* triangles);
};

float* getx(Triangle &a);
float* gety(Triangle &a);
float* getz(Triangle &a);
d3Vector maximum(Triangle* a,int length);
d3Vector minimum(Triangle* a,int length);
d3Vector centroidAverage(Triangle* a,int length);
class BVHNode
{
public:
    BVHNode* left = 0;
    BVHNode* right = 0;
    int l;
    d3Vector ma,mi;
    bool isleaf = false;
    Triangle* array = 0;
    BVHNode(Triangle* in,int a, int length,int de = 0);
    ~BVHNode();
};
class BVH
{
public:
    int passnumber = 0;
    int recipjitter = 3200;
    Sphere* spherepointer = 0;
    BVHNode* root = 0;
    d3Vector ambientcolor = d3Vector(0,0,0);
    BVH();
    BVH(Scene &in);
    ~BVH();
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
    ray(d3Vector o,d3Vector v);
    d3Vector endpoint();
    ray rotatearoundorgin(int zenith,int attitude);
    ray scalearound(ray cize);
    inline float intersectwith(Sphere &sphere);
    float intersectwith(Triangle &triangle);
    hitdata intersectwith(Scene &scene);
    bool intersectwith(d3Vector &mi,d3Vector &ma);
    inline hitdata intersectwith(BVHNode &in);
    inline hitdata intersectwith(BVH &in);
};
int getsignedrand();
int gesignedrand();
ray getCameraRay (int x, int y, int width, int height, float recipjitter, float flength, float fdistance);
class d3vecImage
{
public:
    int x,y;
    d3Vector **start;
    d3vecImage(int _x,int _y);
    void setpixel(int inx,int iny,d3Vector in);
};
class Camera
{
public:
    d3Vector mx,my,mz,start;
    float recipjitter;
    float fdistance;
    float flength;
    Camera();
};
ray MatrixRay(int x,int y,int height,int width, Camera &cam);
d3Vector getcolor(Material &m);
inline float abs(float in);
d3Vector trace(ray inray,BVH &scene,int depth);
d3Vector cosweightedpointinhemisphere();
d3Vector diffusevec(d3Vector &normal);
d3Vector get_diffuse_color(int depth,hitdata &objecthit,ray &cameraray,BVH &scen);
d3Vector get_glossy_color(int depth,hitdata &objecthit,ray &cameraray,BVH &scen);
d3Vector get_transmit_color(int depth,hitdata &objecthit,ray &cameraray,BVH &scen,float ior = 1.3);
Scene generateScene (char* str, Material &m);
d3Vector normalizeColor(d3Vector &in);
d3Vector computePixel (int x,int y,BVH &seed,d3Vector old);
d3Vector computePixel (int x,int y,BVH &seed,Camera cam,d3Vector old);
std::string intToString(int n);
void dump(d3Vector **inImage,int width,int height);
typedef struct
{
    BVH inBVH;
    Camera precam;
    d3vecImage* image;
    int sx,sy,w,h;
    HANDLE renderMutex;
}renderable;
DWORD WINAPI RenderForGui(LPVOID inparam);
void computeSection(int xstart,int xend, int ystart, int yend,d3Vector **opImage,BVH *sce);
#endif
