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
    void readFromFile();
    void writeToFile();
private:
    ECTextViewImp *m_window;
    vector<string> m_rows;
    int m_cursorX;
    int m_cursorY;
    ECCommandHistory m_histCmds;
    string m_fileName;
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
    char m_charToIns;
    int &m_cursorX;
    int &m_cursorY;
    vector<string> &m_rows;
};

class removeTextAtCommand : public ECCommand
{
public:
    removeTextAtCommand(int &cursorX, int &cursorY, vector<string> &rows);
    void Execute();
    void UnExecute();
private:
    char m_previousChar;
    int m_lastPos;
    int &m_cursorX;
    int &m_cursorY;
    vector<string> &m_rows;
};

class enterAtCommand : public ECCommand
{
public:
    enterAtCommand(int &cursorX, int &cursorY, vector<string> &rows);
    void Execute();
    void UnExecute();
private:
    string m_stringToMove;
    int &m_cursorX;
    int &m_cursorY;
    vector<string> &m_rows;
};