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
private:
    ECTextViewImp *window;
    vector<string> rows;
};   