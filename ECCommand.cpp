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
  while (!(m_editingHistory.empty())){
    m_editingHistory.pop();
  }
  while (!(m_undoHistory.empty())){
    m_undoHistory.pop();
  }
}

void ECCommandHistory :: ExecuteCmd(ECCommand *pCmd){
  pCmd->Execute();
  m_editingHistory.push(pCmd);
  while(!(m_undoHistory.empty())){
    m_undoHistory.pop();
  }
}

bool ECCommandHistory :: Undo(){
    try{
        ECCommand *unexecuteThis = m_editingHistory.top();
        unexecuteThis->UnExecute();
        m_undoHistory.push(unexecuteThis);
        m_editingHistory.pop();
    }
    catch (exception& e){
        return false;
        // nothing to undo, so return false
    }
    return true;
  
}

bool ECCommandHistory :: Redo(){
    try {
        ECCommand * reexecuteThis = m_undoHistory.top();
        reexecuteThis->Execute();
        m_editingHistory.push(reexecuteThis);
        m_undoHistory.pop();
    }
    catch (exception& e){
        return false;
        // nothing to redo, so return false
    }
    return true;
}