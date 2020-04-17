#include "ECTextViewImp.h"
#include "ECObserver.h"
#include "ECCommand.h"
#include <string>
#include <vector>

using namespace std;

class ECClient : public ECObserver
{
public:
    ECClient(ECTextViewImp *wndw);
    void Update();
    void HandleCommand(int pressedKey);
    void InsertTextAt(char charToIns); 
    void RemoveTextAt();                         
    bool Undo();  
    bool Redo(); 
private:
    ECTextViewImp *window;
    vector<string> rows;
    int cursorX;
    int cursorY;
    ECCommandHistory histCmds;
};   


// **********************************************************
// Implements Commands for editing
class insertTextAtCommand : public ECCommand
{
public:
    insertTextAtCommand(char charToIns, int &cursorX, int &cursorY, vector<string> &rows);
    void Execute();
    void UnExecute();
private:
    char charToIns;
    int &cursorX;
    int &cursorY;
    vector<string> &rows;
};

class removeTextAtCommand : public ECCommand
{
public:
    removeTextAtCommand(int &cursorX, int &cursorY, vector<string> &rows);
    void Execute();
    void UnExecute();
private:
    char previousChar;
    int &cursorX;
    int &cursorY;
    vector<string> &rows;
};