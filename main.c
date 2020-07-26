#include"term.h" 
#include"audio.h"


int main(){
        enableRawMode();
        
        int err;
        if (err == init_audio()){
                return 1;
        }
               
        destroy_audio();

        return 0;
}

