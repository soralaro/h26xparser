#include "h26xParserInterface.h"
class H26xParserT:public H26xParser{
    public:
    H26xParserT();
    virtual ~H26xParserT();
    virtual int getColorRang(u_int8_t *frame,int len) override;
    virtual void init() override;
    private:
    h264_parser_t *handle_;
};
H26xParserT::H26xParserT(){
    handle_=nullptr;
}
H26xParserT::~H26xParserT(){
    if(!handle_){
        h264_parser_destroy(handle_);
    }
}
void H26xParserT::init(){
    handle_ = h264_parser_create();
}

int H26xParserT::getColorRang(u_int8_t *frame,int len){
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
        return new H26xParserT;
    }
    void DestroyH26xParser(H26xParser *h26xParser){
        H26xParserT *h26xParserT=dynamic_cast<H26xParserT *>(h26xParser);
        if(!h26xParserT)
            delete h26xParserT;
    }
}
