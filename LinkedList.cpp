template<class TYPE> class listnode
{
public:
    TYPE data;
    listnode<TYPE>* nextnode = 0;
    listnode<TYPE>* lastnode = 0;
    listnode(TYPE* a,listnode<TYPE>* b)
    {
        if(b!=0)
        {
            b->nextnode = this;
            lastnode = b;
        }
        data = *a;
    }
    void die(listnode<TYPE>** startptr,listnode<TYPE>** endptr)
    {
        if(nextnode!=0)
        {
            nextnode->lastnode = lastnode;
        }
        else
        {
            *endptr = lastnode;
        }
        if(lastnode!=0)
        {
            lastnode->nextnode = nextnode;
        }
        else
        {
            *startptr = nextnode;
        }
        delete this;
    }
    void deleteall()
    {
        if(nextnode!=0)
        {
            nextnode->deleteall();
            nextnode = 0;
        }
        delete this;
    }
};
template<class TYPE> class LinkedList
{
public:
    listnode<TYPE>* startptr = 0;
    listnode<TYPE>* endptr = 0;
    LinkedList() {}
    void addmember(TYPE data)
    {
        listnode<TYPE>* traverse = startptr;
        if(traverse == 0)
        {
            startptr = new listnode<TYPE>(&data,0);
            return;
        }
        else
        {
            if(endptr == 0)
            {
                while(traverse->nextnode!=0)
                {
                    traverse = traverse->nextnode;
                }
                endptr = traverse;
            }
            else
            {
                traverse = endptr;
            }
            traverse->nextnode = new listnode<TYPE>(&data,traverse);
            endptr = traverse->nextnode;
        }
    }
    listnode<TYPE>* getNode(int index)
    {
        listnode<TYPE>* traverse = startptr;
        if(index == 0)return traverse;
        int i = 0;
        while(i!=index&&traverse!=0)
        {
            i++;
            traverse = traverse->nextnode;
        }
        return traverse;
    }
    TYPE getData(int index)
    {
        return getNode(index)->data;
    }
    TYPE* getArray(TYPE delim)
    {
        int memberCount = 0;
        listnode<TYPE>* traverse = startptr;
        if(traverse!=0)memberCount++;
        else
        {
            TYPE *fail = new TYPE;
            *fail = delim;
            return fail;
        }
        while(traverse->nextnode!=0)
        {
            traverse = traverse->nextnode;
            memberCount++;
        }
        TYPE *outArray = new TYPE[memberCount+1];
        outArray[memberCount] = delim;
        traverse = startptr;
        memberCount = 0;
        while(traverse!=0)
        {
            outArray[memberCount] = traverse->data;
            traverse = traverse->nextnode;
            memberCount++;
        }
        return outArray;
    }
    ~LinkedList()
    {
        startptr->deleteall();
    }
};
inline void swap(Triangle &a,Triangle &b)
{
    Triangle tmp = a;
    a = b;
    b = tmp;
}
int Split(Triangle* arrstart,int a,int b,float p, float* (*getattribute)(Triangle&))
{
    int left = a;
    int right = b;
    while(left<right)
    {
        while(p<(*(getattribute(arrstart[right])))&&right>left)right--;
        swap(arrstart[right],arrstart[left]);
        while(p>=(*(getattribute(arrstart[left])))&&left<right)left++;
        swap(arrstart[right],arrstart[left]);
    }
    return left;
}
void Sorttris(Triangle* arrstart,int a, int b, float* (*getattribute)(Triangle&))
{
    int random;
    if(b-a>0)
    {
        random = a+(rand()%(b-a));
    }else
    {
        return;
    }
    swap(arrstart[a],arrstart[random]);
    float pivot = *getattribute(arrstart[a]);
    Triangle piv = arrstart[a];
    int splitpoint;
    if(b>a)
    {
        splitpoint = Split(arrstart,a,b,pivot,getattribute);
        arrstart[splitpoint]=piv;
        Sorttris(arrstart,a,splitpoint-1,getattribute);
        Sorttris(arrstart,splitpoint+1,b,getattribute);
    }
}
void Sorttris(Triangle* arrstart, float* (*getattribute)(Triangle&))
{
    int count = 0;
    while(arrstart[count++].notnullflag){};
    count-=2;
    Sorttris(arrstart,0,count,getattribute);
}
