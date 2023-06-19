#ifndef PA_OBJECT_ATTRIBUTE_
#define PA_OBJECT_ATTRIBUTE_

class PaObjectAttribute
{
public:
    char name[255];
    char description[255];
    virtual ~PaObjectAttribute(){};
};

#endif // PA_OBJECT_ATTRIBUTE_
