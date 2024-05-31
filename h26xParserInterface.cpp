#include "h26xParserInterface.h"
H26xParser::H26xParser(){
    handle_=nullptr;
}
H26xParser::~H26xParser(){
    if(!handle_){
        h264_parser_destroy(handle_);
    }
}
int H26xParser::init(){
    handle_ = h264_parser_create();
}

int H26xParser::getColorRang(u_int8_t *frame,int len){
    h264_parser_input(handle_,frame,len);
    if(handle_->ctx._sps->vui_parameters_present_flag > 0){
            if(handle_->ctx._sps->vui.video_full_range_flag > 0){
                return 1;
                //color range full
            }else{
                return 0;
                //color range limited
            }
        }else{
            return -1;
            //can not parse vui param from h264 stream
        }
}

extern "C"{
    H26xParser *CreatH26xParser(){
        return new H26xParser;
    }
    void DestroyH26xParser(H26xParser *h26xParser){
        delete h26xParser;
    }
}
