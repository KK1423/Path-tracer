#include <iostream>
#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <fstream>
//#include <Mouse.hpp>
#define Triangle = 4;
#define pi = 3.14159;
using namespace std;
sf::RenderWindow window(sf::VideoMode(1000, 1000), "Raytracer");
int passnumber = 0;
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
float rad(float degr)
{
    degr = (degr*3.14159/180.0);
    return degr;
}
float invsqrt( float number )
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
class d3Vector
{

public:
    float x,y,z;
    d3Vector(float X = 0,float Y = 0,float Z = 0):x(X), y(Y), z(Z) {}
    d3Vector scalarmultiply (float b)
    {
        return d3Vector((this->x*b),(this->y*b),(this->z*b));
    }
    d3Vector individualmultiply (d3Vector coefficients)
    {
        d3Vector result;
        result = d3Vector((this->x)*coefficients.x,(this->y)*coefficients.y,(this->z)*coefficients.z);
        return result;
    }
    float dot(d3Vector b)
    {
        return ((this->x*b.x)+(this->y*b.y)+(this->z*b.z));
    }
    float magnitude()
    {
        return 1.0/invsqrt(x*x+y*y+z*z);
    }
    d3Vector normalize()
    {
        float d = invsqrt(x*x+y*y+z*z);
        return d3Vector(x*d,y*d,z*d);
    };
    float project(d3Vector aO,d3Vector aV)
    {
        d3Vector b=this->subtract(aO);
        aV=aV.normalize();
        return (b.dot(aV));
    }
    d3Vector add(d3Vector a)
    {
        return d3Vector((this->x)+a.x,(this->y)+a.y,(this->z)+a.z);
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
    float r;
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
    };
};
class Scene
{
public:
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
        d3Vector endpt = this->endpoint();
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
        d3Vector projected;

        float t
            = V.dot(sphere.pos.subtract(O));
        projected = V.normalize().scalarmultiply(t);
        float distnce = (projected.subtract(sphere.pos.subtract(O))).magnitude();
        hitdata outdata;
        outdata.hit = false;
        outdata.t = 110;
        if(t<=0)
        {
            return outdata;
        }
        if(distnce<sphere.r)
        {
            float deltaT = sqrtf((sphere.r*sphere.r)-(distnce*distnce));
            if((t-deltaT)<=0)
            {
                if((t+deltaT)>0)
                {
                    deltaT = -deltaT;
                }
                else
                {
                    return outdata;
                }
            }
            outdata.coord = O.add(V.scalarmultiply(t-deltaT));
            outdata.normal = outdata.coord.subtract(sphere.pos);
            outdata.normal = outdata.normal.normalize();//.scalarmultiply(deltaT<0?-1:1);
            outdata.hit = true;
            outdata.material = sphere.material;
            outdata.t = t-deltaT;
        }
        return outdata;
    }
    hitdata intersectwith(Scene scene)
    {
        float lowestt = 10000;
        hitdata lowesthit;
        hitdata hitcache;
        lowesthit.hit = false;
        Sphere * sphereptr = scene.spherepointer;
        while(sphereptr!=0)
        {
            hitcache = intersectwith(Sphere(*sphereptr));
            if(hitcache.hit&&hitcache.t<lowestt)
            {
                lowesthit=hitcache;
                lowestt=hitcache.t;
            }
            sphereptr = sphereptr->nextsphere;
        }

        return lowesthit;
    }
};
ray getcameradata()
{
    static d3Vector position(0,0,0);
    static d3Vector rotation(0,0,0);
    position = position.add(d3Vector(sf::Keyboard::isKeyPressed(sf::Keyboard::Up),sf::Keyboard::isKeyPressed(sf::Keyboard::Right),0).rotatearoundorgin(rotation.x,rotation.y));
    rotation = rotation.add(d3Vector(sf::Mouse::getPosition(window).x + 500,sf::Mouse::getPosition(window).y + 500,0));
    sf::Mouse::setPosition(sf::Vector2i(500,500));
    return ray((position),(rotation));

}
int getsignedrand()
{
    return (rand()%400)*(rand()&1?1:-1);
}
ray getCameraRay (int x, int y, int height, int width, float jitter, float flength, float fdistance)
{

    float xfraction = ((float)x)/((float)width);
    float yfraction = ((float)y)/((float)height);
    xfraction-=0.5;
    yfraction-=0.5;
    d3Vector finaldirection(-flength,xfraction,-yfraction);
    finaldirection = finaldirection.normalize();
    d3Vector cache = finaldirection.scalarmultiply(fdistance);
    d3Vector startdiff(getsignedrand()/8000.0,getsignedrand()/8000.0,getsignedrand()/8000.0);
    finaldirection = cache.subtract(startdiff);
    finaldirection = finaldirection.rotatearoundorgin(0,0);
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
    //dstatic int nth=2;
    srand(nth+1420);
    nth+=rand();
    //cout<<nth<<endl;
    float r = (rand()%1000)/1000.0;
    float t = (rand()%1000)*0.0062831;
    d3Vector discpoint(r*sin(t),r*cos(t),0);
    discpoint.z = sqrtf(1.0-((discpoint.x*discpoint.x)+(discpoint.y*discpoint.y)));
    return discpoint;
}
d3Vector diffusevec(d3Vector normal)
{
    d3Vector hemispherepoint = cosweightedpointinhemisphere();
    d3Vector tangent(normal.y,-1.0*normal.x,0);
    d3Vector bitangent = normal.crossproduct(tangent);
    normal = normal.normalize();
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
    /*d3Vector projectablecameraV = cameraray.V.scalarmultiply(-1.0);
    objecthit.normal = objecthit.normal.normalize();
    nextray.O = objecthit.normal.scalarmultiply(objecthit.normal.dot(projectablecameraV));
    nextray.V = nextray.O.subtract(projectablecameraV);
    nextray.O = nextray.O.add(nextray.V);
    nextray = ray(objecthit.coord,nextray.O);
    nextray.O = nextray.O.add(objecthit.normal.scalarmultiply(0.01));
    nextray.V = nextray.V.normalize();*/
    //return d3Vector(255,0,0);
    objecthit.normal = objecthit.normal.normalize();
    nextray.O = objecthit.coord.add(objecthit.normal.scalarmultiply(0.01));
    float f = -2.0*cameraray.V.dot(objecthit.normal);
    nextray.V = cameraray.V.add(objecthit.normal.scalarmultiply(f));
    //cout<<cameraray.V.dot(objecthit.normal);
    static int nthh=0;
    srand(nthh);
    nthh++;
    nextray.V = nextray.V.add((d3Vector(getsignedrand()/8000.0,getsignedrand()/8000.0,getsignedrand()/8000.0)));
    //std::cout << nextray.V.x;
    d3Vector objectcolor(objecthit.material.r,objecthit.material.g,objecthit.material.b);
    return objectcolor.individualmultiply(trace(nextray,scen,depth - 1).scalarmultiply(1.0/255.0));
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
    float scalar = n+sqrtf(1.0-sinT2);
    d3Vector outv =(cameraray.V.scalarmultiply(n).subtract(objecthit.normal.scalarmultiply(scalar)));
    return trace(ray(objecthit.coord.add(outv.scalarmultiply(0.01)),outv),scen,depth-1);
}
Scene generateScene ()
{
    Sphere *spheres[5] =
    {
        new Sphere(1, 3.7,-0.8,0, Material(1,0.8,1.4,84,255,255)),
        new Sphere(2, 0,2.0,-0.2, Material(1,0.6,1.4,190,64,190)),
        new Sphere(3, -2,-3,0, Material(1,0,0.5,255,255,76)),
        //new Sphere(1, -1.8,0.04,3, Material(1,0,0,255,255,255,2))
        new Sphere(10, 0,0,15, Material(1,0,0,255,255,255,5)),
        new Sphere(1,  1,0,2, Material(1,0.93,0.5,240,255,240))
    };
    spheres[0]->nextsphere = spheres[1];
    spheres[1]->nextsphere = spheres[2];
    spheres[2]->nextsphere = spheres[3];
    spheres[3]->nextsphere = spheres[4];
    spheres[4]->nextsphere = 0;
    Scene proto(4,spheres[0]);
    return proto;

}
Scene sce(generateScene());
sf::Color vectocolor (d3Vector in)
{
    //cout<<"k"<<endl;
    return sf::Color(in.x,in.y,in.z,255);
}
d3Vector normalizeColor(d3Vector in)
{
    return d3Vector(in.x>255?255:in.x,in.y>255?255:in.y,in.z>255?255:in.z);
}
d3Vector computePixel (int x,int y,Scene seed,d3Vector old)
{
    //cout<<"k"<<endl;
    //static int64_t passnumber = 0;
    int pass = passnumber;
    d3Vector Pixel;
    d3Vector Tracedcolor =trace(getCameraRay(x,y,1000,1000,0,1,8),seed,5) ;
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
    ofstream outputFile(str2,ofstream::openmode::_S_bin);
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
    str2 = "OpenSeeIt.exe "+str2;
    system(str2.c_str());
}
int main()
{
    int samples;
    cin>>samples;
    //if(samples == 1){
      //  system("start Raytracer2.exe");return 0;}
    Scene sce = generateScene();
    sce.ambientcolor = d3Vector(0,0,0);
    srand(5);
    srand(rand());
    sf::Image shape;
    d3Vector **regimage = new d3Vector*[1000];
    for(int i = 0; i<1000; i++)
    {
        regimage[i] = new d3Vector[1000];
        for(int j = 0; j<1000; j++)
        {
            regimage[i][j] = 0;//computePixel(i,j,sce,d3Vector(0,0,0));
        }

    }
    int n = 1;
    sf::Texture texture;
    texture.create(1000,1000);
    sf::Sprite sprite(texture);
    bool mousein = false;
    shape.create(1000,1000,sf::Color::Black);
    srand(rand());
    float t = 0;
    while (passnumber<samples)
    {
        int x=0,y=0;
        sf::Event event;
        n++;
        cout<<clock()-t<<endl;
        t = clock();
        while(x<999)
        {
            //srand(rand()%1000);
            while(y<999)
            {
                sf::Color oldcolor = shape.getPixel(x,y);
                d3Vector old(regimage[x][y]);
                d3Vector result(computePixel(x,y,sce,old));
                shape.setPixel(x,y,vectocolor(normalizeColor(result)));
                regimage[x][y]=result;
                y++;
            }
            y = 0;
            x++;
            if(x == 997)
            {
                if(window.pollEvent(event))
                {
                    if(event.type == sf::Event::Closed)
                    {
                        window.close();
                    }
                }
            }
        }
        x = 0;
        passnumber++;
        cout<<passnumber<<endl;
        texture.update(shape);
        sprite.setTexture(texture,false);
        window.draw(sprite);
        window.display();
    }
    window.close();
    dump(regimage,999,999);
    return 0;
}


