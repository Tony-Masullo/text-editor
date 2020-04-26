#include "ECClient.h"	
#include <iostream>
#include <string>
#include <cctype>
#include <fstream>

using namespace  std;

ECClient :: ECClient(ECTextViewImp *wndw, string fileName) 
{
    m_window = wndw;
    m_window->Init();
    m_window->Attach(this);
    /* yufeng has weird bug where first row is shifted up each time. 
       so add black status row so entered text is in right spot */
    m_window->AddStatusRow("","", false);
    m_fileName = fileName;
    // if file run exists, read from it, and populate view
    this->readFromFile();
    // After blank row entered, enter loop to take keyboard input until ctrl-q hit
    m_window->Show();
}
   
void ECClient :: Update(){
    m_cursorX = m_window->GetCursorX();
    m_cursorY = m_window->GetCursorY();
    int pressedKey = m_window->GetPressedKey();
    m_window->ClearStatusRows();
    m_window->InitRows();
    m_window->AddStatusRow("","", false);

    // delegate command handling to handler
    this->HandleCommand(pressedKey);
    // the update is done, so set new cursor positions, and show the new text in the view
    m_window->SetCursorY(m_cursorY);
    m_window->SetCursorX(m_cursorX);
    for (string row: m_rows){
        m_window->AddRow(row);
    }
    this->writeToFile();
}

void ECClient:: HandleCommand(int pressedKey){
    switch (pressedKey){
        case KEY_ACTION::CTRL_Z:
            {
                this->Undo();
            }
            break;
        case KEY_ACTION::CTRL_Y:
            {
                this->Redo();
            }
            break;
        case KEY_ACTION::ENTER:
            {
                this->EnterAt();
            }
            break;
        case KEY_ACTION::BACKSPACE:
            {
                this->RemoveTextAt();
            }
            break;
        case KEY_ACTION::ARROW_LEFT:
            if (m_cursorX > 0){
                m_cursorX--;
            }
            break;
        case KEY_ACTION::ARROW_RIGHT:
            if ((int)m_rows[m_cursorY].size() > m_cursorX){
                m_cursorX++;
            }
            break;
        case KEY_ACTION::ARROW_UP:
            if (m_cursorY > 0){
               m_cursorY--;
                m_cursorX = m_rows[m_cursorY].length();
            }
            break;
        case KEY_ACTION::ARROW_DOWN:
            if ((int)m_rows.size() - 1 > m_cursorY) {
                m_cursorY++;
                m_cursorX = m_rows[m_cursorY].length();
            }
            break;
        default:
        {
            char keyChar{ pressedKey };
            this->InsertTextAt(keyChar);
        }       
    }
}

void ECClient ::InsertTextAt(char charToIns)
{
  // your code
  m_histCmds.ExecuteCmd(new insertTextAtCommand(charToIns, m_cursorX, m_cursorY, m_rows));
}
void ECClient ::RemoveTextAt()
{
  // your code
  m_histCmds.ExecuteCmd(new removeTextAtCommand(m_cursorX, m_cursorY, m_rows));
}
void ECClient ::EnterAt()
{
  // your code
  m_histCmds.ExecuteCmd(new enterAtCommand(m_cursorX, m_cursorY, m_rows));
}
bool ECClient ::Undo()
{
  // your code
  return m_histCmds.Undo();
}

bool ECClient ::Redo()
{
  // your code
  return m_histCmds.Redo();
}

void ECClient ::readFromFile()
{
    string line;
    ifstream textFile;
    textFile.open(m_fileName);
    if (textFile.is_open()){
        while(getline(textFile,line)){
           m_rows.push_back(line);
        }
        textFile.close();
        // show text from file in view upon start of execution
        for (string row: m_rows){
           m_window->AddRow(row);
        }
        // update cursor positions to end of text from read file
        int lastRow = (int)m_rows.size() - 1;
        int lastCol = m_rows[lastRow].length() - 1;
        m_cursorX = lastCol + 1;
        m_cursorY = lastRow;
        m_window->SetCursorX(m_cursorX); 
        m_window->SetCursorY(m_cursorY); 
    }    
}

void ECClient ::writeToFile()
{
    ofstream outFile;
    outFile.open(m_fileName);
    for(string row: m_rows){
        outFile << row << endl;
    }
    outFile.close();
}



// **********************************************************
// Commands
// NOTE: rows(rows), cursorX(cursorX), cursorY(cursorY) -> initialization is done here at the same time as declaration; otherwise an error will be thrown
// This is because references need to be declared and initialized right away. 
// For more understanding: https://www.geeksforgeeks.org/can-c-reference-member-be-declared-without-being-initialized-with-declaration/

insertTextAtCommand:: insertTextAtCommand(char charToIns, int &cursorX, int &cursorY, vector<string> &rows): m_rows(rows), m_cursorX(cursorX), m_cursorY(cursorY)
    {
        m_charToIns = charToIns;
    }
void insertTextAtCommand:: Execute()
    {
        // your code here
        string s(1, m_charToIns);
        string currString = "";
        // if no text added to view, add first row, with first character
        if ((int)m_rows.size() == 0){
            currString += s;
            m_rows.push_back(currString);
            }
        // otherwise, add this character to existing text in that row
        else{
            currString = m_rows[m_cursorY] + s;
            m_rows.erase(m_rows.begin() + m_cursorY);
            m_rows.insert(m_rows.begin() + m_cursorY, currString);
            }
            m_cursorX++;
    }
void insertTextAtCommand:: UnExecute()
    {
        // if cursor is at the begining of line, backspace moves cursor to one line up.
        if (m_cursorX == 0){
            // only move cursor up one line if it isn't already at the beginning of the document
            if (m_cursorY - 1 >= 0){
                m_cursorY--;
                m_cursorX = m_rows[m_cursorY].length();
            }
        }
        // delete current character from string of this row, and move cursor back one space.
        else{
            string newString = m_rows[m_cursorY].substr(0, m_rows[m_cursorY].length()-1);
            m_rows[m_cursorY] = newString;
            m_cursorX--;
        } 
    }


removeTextAtCommand:: removeTextAtCommand(int &cursorX, int &cursorY, vector<string> &rows): m_rows(rows), m_cursorX(cursorX), m_cursorY(cursorY)
    {
    }
void removeTextAtCommand:: Execute()
    {
        // if cursor is at the begining of line, backspace moves cursor to one line up.
        if (m_cursorX == 0){
            // only move cursor up one line if it isn't already at the beginning of the document
            if (m_cursorY - 1 >= 0){
                m_cursorY--;
                m_cursorX = m_rows[m_cursorY].length(); 
                //move any remaining characters from line below up to line above
                string moveBack = m_rows[m_cursorY + 1];
                m_rows[m_cursorY + 1] = "";
                m_rows[m_cursorY] += moveBack;

            }
        }
        // delete character from string of this row where cursor is currently located, and move cursor back one space.
        else{
            string firstPart = m_rows[m_cursorY].substr(0, m_cursorX);
            m_previousChar = firstPart.back();
            m_lastPos = m_cursorX - 1;
            firstPart.pop_back();
            string secondPart = m_rows[m_cursorY].substr(m_cursorX, m_rows[m_cursorY].length()-1);
            string newString = firstPart + secondPart;
            m_rows[m_cursorY] = newString;
            m_cursorX--;
        }
    }
void removeTextAtCommand:: UnExecute()
    {
        string s(1, m_previousChar);
        string currString = "";
        // if no text added to view, add first row, with first character
        if ((int)m_rows.size() == 0){
            currString += s;
            m_rows.push_back(currString);
            }
        // otherwise, add this character to existing text in that row at its most previous position
        else{
            string firstPart = m_rows[m_cursorY].substr(0, m_lastPos);
            string secondPart = m_rows[m_cursorY].substr(m_lastPos, m_rows[m_cursorY].length()-1);
            string newString = firstPart + s + secondPart;
            m_rows.erase(m_rows.begin() + m_cursorY);
            m_rows.insert(m_rows.begin() + m_cursorY, newString);
            }
            m_cursorX++;
    }

enterAtCommand:: enterAtCommand( int &cursorX, int &cursorY, vector<string> &rows): m_rows(rows), m_cursorX(cursorX), m_cursorY(cursorY)
    {
    }
void enterAtCommand:: Execute()
    {
        // start new line after current line of cursor, and move cursor to that line
        if (m_cursorX == m_rows[m_cursorY].length()){
            // if cursor at end of line, just move to a new blank line and add previous string if redoing command
            m_rows.insert(m_rows.begin() + m_cursorY + 1, m_stringToMove);
            int thisPos = m_rows[m_cursorY].length() - m_stringToMove.length();
            m_rows[m_cursorY] = m_rows[m_cursorY].substr(0, thisPos);
        }
        else{
            // if cursor is in the middle of text, split from current line to next
            string moveToNextLine = m_rows[m_cursorY].substr(m_cursorX);
            m_stringToMove = moveToNextLine;
            m_rows.insert(m_rows.begin() + m_cursorY + 1, moveToNextLine);
            m_rows[m_cursorY] = m_rows[m_cursorY].substr(0, m_cursorX);
        }
        m_cursorY++;
        m_cursorX = m_rows[m_cursorY].length();
    }
void enterAtCommand:: UnExecute()
    {
        m_cursorY--;
        m_rows[m_cursorY] += m_stringToMove;
        m_rows.erase(m_rows.begin() + m_cursorY + 1);
        m_cursorX = m_rows[m_cursorY].length();
    }
