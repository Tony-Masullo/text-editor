#include "ECTextViewImp.h"
#include "ECObserver.h"
#include "ECCommand.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

class ECClient : public ECObserver
{
public:
    ECClient(ECTextViewImp *wndw, string fileName);
    void Update();
    void HandleCommand(int pressedKey);
    void InsertTextAt(char charToIns); 
    void RemoveTextAt();     
    void EnterAt();                    
    bool Undo();  
    bool Redo(); 
    void writeToFile();
private:
    ECTextViewImp *window;
    vector<string> rows;
    int cursorX;
    int cursorY;
    ECCommandHistory histCmds;
    string fileName;
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
    int lastPos;
    int &cursorX;
    int &cursorY;
    vector<string> &rows;
};

class enterAtCommand : public ECCommand
{
public:
    enterAtCommand(int &cursorX, int &cursorY, vector<string> &rows);
    void Execute();
    void UnExecute();
private:
    string stringToMove;
    int &cursorX;
    int &cursorY;
    vector<string> &rows;
};