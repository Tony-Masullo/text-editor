// Test code for editor
#include "ECTextViewImp.h"
#include "ECClient.h"
#include <iostream>
#include <fstream>

using namespace  std;

int myCounter = 0;

int main(int argc, char *argv[])
{
    if (argc > 1){
        ECTextViewImp wndTest;
        ECClient client(&wndTest, argv[1]);
        return 0;
    }
    else{
        // no file input
        return 1;
    }
}
