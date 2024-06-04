#include <string>
#include "h26X_div.h"
#include "h26xParserInterface.h"
#include <memory>
#include <iostream>
#include <dlfcn.h>
int main(){
     H26X_DIV h26XDiv;
     unsigned int pb_frame_num_=0;
     unsigned int i_frame_num_=0;
     bool only_dec_iframe_=false;
     unsigned int sn=0;
     h26XDiv.init("vlc.h264");
    uint8_t *buffer = new uint8_t[CACH_LEN];
    uint8_t *iFrame_bufer = new uint8_t[CACH_LEN];
    int iFrame_len = 0;   
    int len = 0;
    int send_len = 0;
    void* handle = dlopen("libh26xparser.so", RTLD_NOW);
	 if (!handle) {
    	std::cerr << "Error loading libh26xparser: " << dlerror() << std::endl;
    	return 1;
  	}
	printf("dlsym!\n");
    
	H26xParser*(*CreatH26xParser)() = (H26xParser *(*)())dlsym(handle, "CreatH26xParser");
  	if (!CreatH26xParser) {
    	std::cerr << "Error finding symbol: " << dlerror() << std::endl;
    	return 1;
  	}
    void (*DestroyH26xParser)(H26xParser *)= (void (*)(H26xParser *))dlsym(handle, "DestroyH26xParser");
  	if (!DestroyH26xParser) {
    	std::cerr << "Error finding symbol: " << dlerror() << std::endl;
    	return 1;
  	}


    std::shared_ptr<H26xParser> h26xParser=std::shared_ptr<H26xParser>(CreatH26xParser(),[=](H26xParser *h26xParser){ DestroyH26xParser(h26xParser);});
    
    h26xParser->init(H26xParser::H26x_Type::H264);
    while (1) {
        if (iFrame_len == 0) {
            if ((len = h26XDiv.getOneNal(buffer + send_len, CACH_LEN - send_len)) <= 0) {
                break;
            }
            int type;

            type = h26XDiv.checkNal(buffer + send_len, len);

            send_len += len;
            if (type == NALU_TYPE_SPS || type == NALU_TYPE_SEI || type == NALU_TYPE_PPS) {
                continue;
            }
            if (type == NALU_TYPE_SLICE) {
                pb_frame_num_++;
            } else if (type == NALU_TYPE_IDR) {
                i_frame_num_++;
            }
            if (only_dec_iframe_) {
                if (type != NALU_TYPE_IDR) {
                    send_len = 0;
                    continue;
                } else {
                    memcpy(iFrame_bufer, buffer, send_len);
                    iFrame_len = send_len;
                }
            }

        } else {
            memcpy(buffer, iFrame_bufer, iFrame_len);
            send_len = iFrame_len;
            i_frame_num_++;
            sn++;
        }
        int colorRang=h26xParser->getColorRang(buffer,send_len,H26xParser::H26x_Type::H264);
        if(colorRang<0){
            printf("can't get colorange!\n");
        }else if(colorRang==0){
            printf("limited color range!\n");
        }else if(colorRang==1){
            printf("full color range!\n");
        }
    }
    return 0;
}