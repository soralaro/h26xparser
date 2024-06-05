#ifndef H26XPARSERINTERFACE 
#define H26XPARSERINTERFACE 
#include <string>
typedef unsigned char uint8_t;  
class H26xParser{
    public:
    enum H26x_Type{None,H264,H265};
    H26xParser(){};
    virtual ~H26xParser(){};
    virtual int getColorRang(const u_int8_t *frame,int len,H26x_Type type){return -1;};
    virtual void init(H26x_Type type){};
};
extern "C"{
H26xParser *CreatH26xParser();
void DestroyH26xParser(H26xParser *h26xParser);
}
#endif