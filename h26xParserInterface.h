#ifndef H26XPARSERINTERFACE 
#define H26XPARSERINTERFACE 
#include "media/h264/h264-util.h"
#include "media/h264/h264-parser.h"
#include "media/h265/h265-sps.h"
#include <string>
typedef unsigned char uint8_t;  
class H26xParser{
    public:
    H26xParser();
    virtual ~H26xParser();
    int getColorRang(u_int8_t *frame,int len);
    int init();
    private:
    h264_parser_t *handle_;
};
extern "C"{
H26xParser *CreatH26xParser();
void DestroyH26xParser(H26xParser *h26xParser);
}
#endif