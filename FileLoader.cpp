#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <exception>
using namespace std;
string* parse(string s)
{
    while(s[s.length()-1]==' ')
    {
        s = s.substr(0,s.length()-1);
    }
    while(s[0]==' ')
    {
        s = s.substr(1);
    }
    string c(s);
    int spaceCount = 0;
    while(c.length()>0)
    {
        if((int)c[0]==32)
        {
            spaceCount++;
            while((int)c[0]==32)
                c = c.substr(1);
        }
        else
            c = c.substr(1);
    }
    c = s;
    string * outwords = new string[spaceCount+2]();
    outwords[spaceCount+1]="@";
    int currentwordindex = 0;
    int characterindex = -1;
    while(c.length()>0)
    {
        characterindex++;
        if(c[characterindex]==32)
        {
            outwords[currentwordindex] = c.substr(0,characterindex);
            c = c.substr(characterindex+1);
            currentwordindex++;
            while(c[0]==32)
            {
                c = c.substr(1);
            }
            characterindex = -1;
        }

        if(currentwordindex == spaceCount)
        {
            outwords[currentwordindex] = c;
            return outwords;
        }
    }
    return outwords;
}
float parseforfloat(string in)
{
    int exp = in.find_first_of('.');
    bool negflag = false;
    if(exp<0)
        exp = in.length();
    exp--;
    int traverse = 0;
    float out = 0.0;
    while(traverse<in.length())
    {
        in.length();
        if(in[traverse]!='.')
        {
            if(in[traverse]=='-')
            {
                out*=-1;
                negflag=true;
            }else
            {
                out+=powf(10.0,(float)exp)*((int)in[traverse]-48)*(negflag?-1:1);
            }
            exp--;
        }
        traverse++;
    }
    return out;
}
d3Vector get_vec(string line)
{
    string* words = parse(line);
    d3Vector out;
    out.x = parseforfloat(words[0]);
    out.y = parseforfloat(words[1]);
    out.z = parseforfloat(words[2]);
    return out;
}
Scene readFile (string filename)
{
    LinkedList<d3Vector> vecs;
    LinkedList<Triangle> tris;
    int tricount = 0;
    ifstream file(filename);
    if(!file.is_open())
    {
        cout<<"404:File Not Found"<<endl;
        throw 404;
        return Scene(0,0,0,0);
    }
    string line;
    while(!file.eof())
    {
        getline(file,line);
        if(line[0]!='#') //ignore comment lines
        {
            switch(line[0])
            {
            case 'v':
                vecs.addmember(get_vec(line.substr(2)));
                break;
            case 'f':
                d3Vector vec = get_vec(line.substr(2));
                //cout<<vec.z<<endl;
                d3Vector a,b,c;
                a = vecs.getData(vec.x-1);
                b = vecs.getData(vec.y-1);
                c = vecs.getData(vec.z-1);
                /*cout<<a.x<<" "<<a.y<<" "<<a.z<<" "<<endl;
                cout<<b.x<<" "<<b.y<<" "<<b.z<<" "<<endl;
                cout<<c.x<<" "<<c.y<<" "<<c.z<<" "<<endl;*/
                tricount++;
                tris.addmember(Triangle(a,
                                        b,
                                        c,
                                        Material(1,1,1,255,255,255)));
                break;
            }
        }
    }
    cout<<"triangle count: "<<tricount<<endl;
    return Scene(0,0,tricount,tris.getArray(Triangle(0)));
}
