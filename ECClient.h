#include "ECTextViewImp.h"
#include "ECObserver.h"
#include "ECCommand.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

/* Composite Structure 
    ECClient (or the editor) - composed of ECPages;
    ECPages - composed of rows;
    rows (a vector<strings>) - composed of strings; 
*/

// Implements Composite Structure
class ECParagraph;
class ECPage;

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
    void addPage();
    void insertIntoPage(char charToIns);
    void removeFromPage();
    void enterAtPage();
    void pageUP();
    void pageDOWN();
private:
    int m_viewWidth;
    int m_viewHeight;
    ECTextViewImp *m_window;
    vector<ECPage> m_pages;
    vector<string> m_rows;
    int m_currPageNum;
    ECPage *m_currPage;
    int m_cursorX;
    int m_cursorY;
    ECCommandHistory m_histCmds;
    string m_fileName;
};   

class ECPage
{
public:
    ECPage(ECClient *client);
    void addParagraph();
    void insertIntoParagraph(char charToIns);
    void paragraphUP();
    void paragraphDOWN();
private:
    ECClient *m_client;
    int m_currParagraphNum;
    ECParagraph *m_currParagraph;
    vector<ECParagraph> m_paragraphs;
};

class ECParagraph
{
public:
    ECParagraph(ECPage *page);
    // insert and remove will be same as insertTextAt, etc.
    void insertIntoRow(char charToIns); 
    void removeFromRow(); 
    void rowUP();
    void rowDOWN();
private:
    ECPage *m_page;
    int m_currRow;
    vector<string> m_rows;
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