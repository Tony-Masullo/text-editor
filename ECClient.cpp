#include "ECClient.h"	
#include <iostream>
#include <string>
#include <cctype>

using namespace  std;

ECClient :: ECClient(ECTextViewImp *wndw) 
{
    window = wndw;
    window->Init();
    window->Attach(this);
    /* yufeng has weird bug where first row is shifted up each time. 
       so add black status row so entered text is in right spot */
    window->AddStatusRow("","", false);
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
                // start new line after current line of cursor, and move cursor to that line
                int currRow = cursorY;
                if (cursorX == rows[currRow].length()){
                    // if cursor at end of line, just move to a new blank line
                    rows.insert(rows.begin() + currRow + 1, "");
                    cursorX = 0;
                }
                else{
                    // if cursor is in the middle of text, split from current line to next
                    string moveToNextLine = rows[currRow].substr(cursorX);
                    rows.insert(rows.begin() + currRow + 1, moveToNextLine);
                    rows[currRow] = rows[currRow].substr(0, cursorX);
                    cursorX = moveToNextLine.length();
                }
                cursorY++;
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



// **********************************************************
// Commands

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
        // delete current character from string of this row, and move cursor back one space.
        else{
            this->previousChar = rows[cursorY].back();
            string newString = rows[cursorY].substr(0, rows[cursorY].length()-1);
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
        // otherwise, add this character to existing text in that row
        else{
            currString = rows[cursorY] + s;
            rows.erase(rows.begin() + cursorY);
            rows.insert(rows.begin() + cursorY, currString);
            }
            cursorX++;
    }