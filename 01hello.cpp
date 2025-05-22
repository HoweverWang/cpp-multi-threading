#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional> // for std::ref
#include <cmath>
#include <mutex>
#include <thread>
#include <atomic>

using namespace std;

void printHello(string& msg, int& index) {
    index++;
    cout << msg << " " << index << endl;
}

int main()
{
    string s1 = "hello";
    int num = 1;
    thread thread1(printHello, ref(s1), ref(num));
    if (thread1.joinable()) thread1.join();
    cout << num << endl;
    return 0;
}
