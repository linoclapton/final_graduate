#include "iostream"
//using namespace std;
#include "vector.h"
#include "List.h"
#include "Stack.h"
#include "Tree.h"
#include "BST.h"
#include "UF.h";

/* For sockaddr_in */
#include <netinet/in.h>
/* For socket functions */
#include <sys/socket.h>
/* For gethostbyname */
#include <netdb.h>

#include <unistd.h>
#include <string.h>
#include <stdio.h>

int main(int c, char **v)
{
    const char query[] =
        "GET / HTTP/1.0\r\n"
        "Host: www.google.com\r\n"
        "\r\n";
    const char hostname[] = "www.google.com";
    struct sockaddr_in sin;
    struct hostent *h;
    const char *cp;
    int fd;
    ssize_t n_written, remaining;
    char buf[1024];

    /* Look up the IP address for the hostname.   Watch out; this isn't
       threadsafe on most platforms. */
    h = gethostbyname(hostname);
    if (!h) {
        fprintf(stderr, "Couldn't lookup %s: %s", hostname, hstrerror(h_errno));
        return 1;
    }
    if (h->h_addrtype != AF_INET) {
        fprintf(stderr, "No ipv6 support, sorry.");
        return 1;
    }

    /* Allocate a new socket */
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return 1;
    }

    /* Connect to the remote host. */
    sin.sin_family = AF_INET;
    sin.sin_port = htons(80);
    sin.sin_addr = *(struct in_addr*)h->h_addr;
    if (connect(fd, (struct sockaddr*) &sin, sizeof(sin))) {
        perror("connect");
        close(fd);
        return 1;
    }

    /* Write the query. */
    /* XXX Can send succeed partially? */
    cp = query;
    remaining = strlen(query);
    while (remaining) {
      n_written = send(fd, cp, remaining, 0);
      if (n_written <= 0) {
        perror("send");
        return 1;
      }
      remaining -= n_written;
      cp += n_written;
    }

    /* Get an answer back. */
    while (1) {
        ssize_t result = recv(fd, buf, sizeof(buf), 0);
        if (result == 0) {
            break;
        } else if (result < 0) {
            perror("recv");
            close(fd);
            return 1;
        }
        fwrite(buf, 1, result, stdout);
    }

    close(fd);
    return 0;
}

template<typename T>
class Increase{
public:
    virtual void operator()(T& e){e++;}
};

template<typename T>
void increase(Vector<T> &v){
    v.traverse(Increase<T>());
}
void convert(Stack<char> s,__int64 n,int base){
    static char digit[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    while(n>0){
        s.push(digit[n%base]);
        n /= base;
    }
}
//[lo,hi)
bool parenthese(const char exp[],int lo,int hi){
    Stack<char> s;    
    for(int i=lo;i<hi;i++){
        if(exp[i]=='(') s.push('(');
        else if(!s.empty()) s.pop();
        else return false;
    }
    return s.empty();
}
template <typename T>
class Visit{
public:
    virtual void operator()(T& e){std::cout<<e<<' ';}
};

class A{
public:
    const int i;
    A():i(1){b=1;}
    int b;
    //void print() const{cout<<b+1<<endl;}
    void print() {std::cout<<b<<std::endl;}

};
class B{
public:
    char c;
    int b;
    short a;
    double d;
};
class Base{
public:
    //virtual void print()=0;
    const int a;
    Base(int b):a(b){}
};
class Derived : public Base{
    //virtual void print(){}
public:
    int b;
    void print();
};
inline void  Derived::print(){
    std::cout<<b<<std::endl;
}
static void fn1(){
    std::cout<<"fn1"<<std::endl;
}
void fn2(int array[]){
    int b[10];
    array = b;
    std::cout<<sizeof(array)<<std::endl;
}
template <typename T>
void swap(T &a,T &b){
    T tmp;
    tmp = a;
    a = b;
    b = tmp;
}
typedef int INT5[5]; 
int (* madan(int b))[5]{
    INT5 *c; 
    return c;
}
int max(int x,int y){
    return x>y?x:y;
}
typedef int (*pFun)(int,int);
void syntaxTest();

int main(){

    return 0;
}

void testUF(){
    UF uf(10); 
    uf.uni(0,1);
    uf.uni(1,2);
    using namespace std;
    cout<<uf.getCount()<<' '<<uf.connected(0,2)<<' '<<uf.connected(1,5)<<endl;
}
void syntaxTest(){
    /*int a[] ={3,2,1};
    Vector<int> v(a,0,3);
    v.sort(0,3);
    std::cout<<v[0]<<' '<<v[1]<<' '<<v[2]<<std::endl;*/
    //A a;
    //a.print();
    //int b[3] = {0};
    //int *c = &b[0];
    //fn2(c);
    int array[100]={1};
    int (&array_ref)[100] = array;
    1[array] = 10;
    std::cout<<1[array]<<std::endl;
    std::cout<<sizeof array<<" "<<sizeof array_ref<<std::endl;
    Base t(5),t1(6);
    std::cout<<t.a<<t1.a<<std::endl;
    int (*(*m)(int))[5];
    pFun q = max;
    std::cout<<(*max)(1,2)<<std::endl;
    char *str[] = {"Welcome","to","Fortemedia","Nanjing"};
    char **p = str+1;
    str[0] = (*p++) + 1;
    str[1] = *(p+1);
    str[2] = p[1] + 3;
    str[3] = p[0] + (str[2] - str[1]);
    std::cout<<str[0]<<std::endl;
    std::cout<<str[1]<<std::endl;
    std::cout<<str[2]<<std::endl;
    std::cout<<p[0]<<std::endl;
    std::cout<<str[3]<<std::endl;
    /*List<int> l;
    l.insertAsLast(5);
    l.insertAsLast(4);
    l.insertAsLast(2);
    l.insertAsLast(1);
    int k = 0,i = 0,j=0;
    a[++i] = a[++i] = a[++i];
    std::cout<<a[0]<<' '<<a[1]<<' '<<a[2]<<std::endl;
    std::cout<<v[0]<<' '<<v[1]<<' '<<v[2]<<std::endl;
    std::cout<<l[0]<<' '<<l[1]<<' '<<l[2]<<' '<<l[3]<<std::endl;
    l.insertionSort(l.first(),4);
    std::cout<<l.search(3,4,l.last())->data<<std::endl;
    std::cout<<l[0]<<' '<<l[1]<<' '<<l[2]<<' '<<l[3]<<std::endl;*/
    //A a = {1,'b'};
    //cout<<a.b<<endl; 
    //Tree ²âÊÔ
    /*
    BinTree<int> tree(1);
    BinNode<int> *root = tree.root();
    BinNode<int> *t = tree.insertAsLC(root,2);
    tree.insertAsLC(t,4);
    tree.insertAsRC(t,5);
    t = tree.insertAsRC(root,3);
    tree.insertAsLC(t,6);
    tree.insertAsRC(t,7);
    traversePre(root,Visit<int>());
    cout<<endl;
    travPre_I2(root,Visit<int>());
    cout<<endl;
    travIn_I1(root,Visit<int>());
    cout<<endl;
    travPost_I1(root,Visit<int>());
    cout<<endl;
    root->travLevel(Visit<int>());
    cout<<endl;
    cout<<root->lChild->height<<endl;
    BST<int> bst(4);
    bst.insert(1);
    bst.insert(5);
    root = bst.root();
    root->travLevel(Visit<int>());
    cout<<bst.search(5)->data<<endl;*/
    //increase<int>(v);
    //std::cout<<v.disordered()<<std::endl;
    //std::cout<<v[0]<<' '<<v[1]<<' '<<v[2]<<std::endl;

}