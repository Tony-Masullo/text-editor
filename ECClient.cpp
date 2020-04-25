#include "ECClient.h"	
#include <iostream>
#include <string>
#include <cctype>
#include <fstream>

using namespace  std;

ECClient :: ECClient(ECTextViewImp *wndw, string fileName) 
{
    window = wndw;
    window->Init();
    window->Attach(this);
    /* yufeng has weird bug where first row is shifted up each time. 
       so add black status row so entered text is in right spot */
    window->AddStatusRow("","", false);
    // if file run exists, read from it, and populate view
    string line;
    ifstream textFile;
    this->fileName = fileName;
    textFile.open(this->fileName);
    if (textFile.is_open()){
        while(getline(textFile,line)){
            rows.push_back(line);
        }
        textFile.close();
        // show text from file in view upon start of execution
        for (string row: rows){
            window->AddRow(row);
        }
        // update cursor positions to end of text from read file
        int lastRow = (int)rows.size() - 1;
        int lastCol = rows[lastRow].length() - 1;
        this->cursorX = lastCol + 1;
        this->cursorY = lastRow;
        window->SetCursorX(this->cursorX); 
        window->SetCursorY(this->cursorY); 
    }
    // After blank row entered, enter loop to take keyboard input until ctrl-q hit
    window->Show();
}
   
void ECClient :: Update(){
    this->cursorX = window->GetCursorX();
    this->cursorY = window->GetCursorY();
    int pressedKey = window->GetPressedKey();

    window->ClearStatusRows();
    window->InitRows();
    window->AddStatusRow("","", false);

    // delegate command handling to handler
    this->HandleCommand(pressedKey);
    // the update is done, so set new cursor positions, and show the new text in the view
    window->SetCursorY(cursorY);
    window->SetCursorX(cursorX);
    for (string row: rows){
        window->AddRow(row);
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
            if (cursorX > 0){
                cursorX--;
            }
            break;
        case KEY_ACTION::ARROW_RIGHT:
            if ((int)rows[cursorY].size() > cursorX){
                cursorX++;
            }
            break;
        case KEY_ACTION::ARROW_UP:
            if (cursorY > 0){
                cursorY--;
                cursorX = rows[cursorY].length();
            }
            break;
        case KEY_ACTION::ARROW_DOWN:
            if ((int)rows.size() - 1 > cursorY) {
                cursorY++;
                cursorX = rows[cursorY].length();
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
  histCmds.ExecuteCmd(new insertTextAtCommand(charToIns, this->cursorX, this->cursorY, this->rows));
}
void ECClient ::RemoveTextAt()
{
  // your code
  histCmds.ExecuteCmd(new removeTextAtCommand(this->cursorX, this->cursorY, this->rows));
}
void ECClient ::EnterAt()
{
  // your code
  histCmds.ExecuteCmd(new enterAtCommand(this->cursorX, this->cursorY, this->rows));
}
bool ECClient ::Undo()
{
  // your code
  return this->histCmds.Undo();
}

bool ECClient ::Redo()
{
  // your code
  return this->histCmds.Redo();
}

void ECClient ::writeToFile()
{
    ofstream outFile;
    outFile.open(this->fileName);
    for(string row: rows){
        outFile << row << endl;
    }
    outFile.close();
}



// **********************************************************
// Commands
// NOTE: rows(rows), cursorX(cursorX), cursorY(cursorY) -> initialization is done here at the same time as declaration; otherwise an error will be thrown
// This is because references need to be declared and initialized right away. 
// For more understanding: https://www.geeksforgeeks.org/can-c-reference-member-be-declared-without-being-initialized-with-declaration/

insertTextAtCommand:: insertTextAtCommand(char charToIns, int &cursorX, int &cursorY, vector<string> &rows): rows(rows), cursorX(cursorX), cursorY(cursorY)
    {
        this->charToIns = charToIns;
    }
void insertTextAtCommand:: Execute()
    {
        // your code here
        string s(1, this->charToIns);
        string currString = "";
        // if no text added to view, add first row, with first character
        if ((int)rows.size() == 0){
            currString += s;
            rows.push_back(currString);
            }
        // otherwise, add this character to existing text in that row
        else{
            currString = rows[cursorY] + s;
            rows.erase(rows.begin() + cursorY);
            rows.insert(rows.begin() + cursorY, currString);
            }
            cursorX++;
    }
void insertTextAtCommand:: UnExecute()
    {
        // if cursor is at the begining of line, backspace moves cursor to one line up.
        if (cursorX == 0){
            // only move cursor up one line if it isn't already at the beginning of the document
            if (cursorY - 1 >= 0){
                cursorY--;
                cursorX = rows[cursorY].length();
            }
        }
        // delete current character from string of this row, and move cursor back one space.
        else{
            string newString = rows[cursorY].substr(0, rows[cursorY].length()-1);
            rows[cursorY] = newString;
            cursorX--;
        } 
    }


removeTextAtCommand:: removeTextAtCommand(int &cursorX, int &cursorY, vector<string> &rows): rows(rows), cursorX(cursorX), cursorY(cursorY)
    {
    }
void removeTextAtCommand:: Execute()
    {
        // if cursor is at the begining of line, backspace moves cursor to one line up.
        if (cursorX == 0){
            // only move cursor up one line if it isn't already at the beginning of the document
            if (cursorY - 1 >= 0){
                cursorY--;
                cursorX = rows[cursorY].length(); 
                //move any remaining characters from line below up to line above
                string moveBack = rows[cursorY + 1];
                rows[cursorY + 1] = "";
                rows[cursorY] += moveBack;

            }
        }
        // delete character from string of this row where cursor is currently located, and move cursor back one space.
        else{
            string firstPart = rows[cursorY].substr(0, cursorX);
            this->previousChar = firstPart.back();
            this->lastPos = cursorX - 1;
            firstPart.pop_back();
            string secondPart = rows[cursorY].substr(cursorX, rows[cursorY].length()-1);
            string newString = firstPart + secondPart;
            rows[cursorY] = newString;
            cursorX--;
        }
    }
void removeTextAtCommand:: UnExecute()
    {
        string s(1, this->previousChar);
        string currString = "";
        // if no text added to view, add first row, with first character
        if ((int)rows.size() == 0){
            currString += s;
            rows.push_back(currString);
            }
        // otherwise, add this character to existing text in that row at its most previous position
        else{
            string firstPart = rows[cursorY].substr(0, this->lastPos);
            string secondPart = rows[cursorY].substr(this->lastPos, rows[cursorY].length()-1);
            string newString = firstPart + s + secondPart;
            rows.erase(rows.begin() + cursorY);
            rows.insert(rows.begin() + cursorY, newString);
            }
            cursorX++;
    }

enterAtCommand:: enterAtCommand( int &cursorX, int &cursorY, vector<string> &rows): rows(rows), cursorX(cursorX), cursorY(cursorY)
    {
    }
void enterAtCommand:: Execute()
    {
        // start new line after current line of cursor, and move cursor to that line
        if (cursorX == rows[cursorY].length()){
            // if cursor at end of line, just move to a new blank line and add previous string if redoing command
            rows.insert(rows.begin() + cursorY + 1, this->stringToMove);
            int thisPos = rows[cursorY].length() - this->stringToMove.length();
            rows[cursorY] = rows[cursorY].substr(0, thisPos);
        }
        else{
            // if cursor is in the middle of text, split from current line to next
            string moveToNextLine = rows[cursorY].substr(cursorX);
            this->stringToMove = moveToNextLine;
            rows.insert(rows.begin() + cursorY + 1, moveToNextLine);
            rows[cursorY] = rows[cursorY].substr(0, cursorX);
        }
        cursorY++;
        cursorX = rows[cursorY].length();
    }
void enterAtCommand:: UnExecute()
    {
        cursorY--;
        rows[cursorY] += this->stringToMove;
        rows.erase(rows.begin() + cursorY + 1);
        cursorX = rows[cursorY].length();
    }
