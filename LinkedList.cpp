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
            }else
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
};
