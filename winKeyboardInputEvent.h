#ifndef __WINKEYBOARDINPUTEVENT_H__ 

#define __WINKEYBOARDINPUTEVENT_H__

struct WinKBD_input_event {
    int state; // down 1, up 0
    int value; // keyboard value
};

#endif
