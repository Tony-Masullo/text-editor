//
//  ECCommand.cpp
//  
//
//  Created by Yufeng Wu on 2/26/20.
//
//

#include "ECCommand.h"

// ******************************************************
// Implement command history

ECCommandHistory :: ECCommandHistory() 
{
  // your code goes here
}

ECCommandHistory :: ~ECCommandHistory()
{
  // your code goes here
  while (!(this->editingHistory.empty())){
    this->editingHistory.pop();
  }
  while (!(this->undoHistory.empty())){
    this->undoHistory.pop();
  }
}

void ECCommandHistory :: ExecuteCmd(ECCommand *pCmd){
  pCmd->Execute();
  this->editingHistory.push(pCmd);
  while(!(this->undoHistory.empty())){
    this->undoHistory.pop();
  }
}

bool ECCommandHistory :: Undo(){
    try{
        ECCommand *unexecuteThis = this->editingHistory.top();
        unexecuteThis->UnExecute();
        this->undoHistory.push(unexecuteThis);
        this->editingHistory.pop();
    }
    catch (exception& e){
        return false;
        // nothing to undo, so return false
    }
    return true;
  
}

bool ECCommandHistory :: Redo(){
    try {
        ECCommand * reexecuteThis = this->undoHistory.top();
        reexecuteThis->Execute();
        this->editingHistory.push(reexecuteThis);
        this->undoHistory.pop();
    }
    catch (exception& e){
        return false;
        // nothing to redo, so return false
    }
    return true;
}


