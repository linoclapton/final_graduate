/***************************************/
/*       @author   : Li Yan            */
/*       @version  : 9                 */
/*       @created  : 2017-12-12 17:21  */
/*       @modified : 2017-12-12 17:29  */
/***************************************/
#include <fstream>
#include <iostream>

using namespace std;

void swapByteOrder(int& i)
{
    i = (i >> 24) |
         ((i<<8) & 0x00FF0000) |
         ((i>>8) & 0x0000FF00) |
         (i << 24);
}

int main(){
    ifstream fin("C60Large.vol",iostream::binary);
    int x,y,z;
    fin>>x>>y>>z;
    swapByteOrder(x);
    swapByteOrder(y);
    swapByteOrder(z);
    cout<<x<<" "<<y<<" "<<z<<endl;
    fin.close();
    return 0;
}
