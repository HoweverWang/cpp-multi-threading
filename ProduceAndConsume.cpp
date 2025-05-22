#include "ProducerAndConsumer.h"

int main()
{
    class ProducerAndConsumer PC(10, 3, 3);
    std::this_thread::sleep_for(std::chrono::seconds(60));
    return 0;
}
