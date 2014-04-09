#include <iostream>
#include <future>
#include <functional>

class PureVirtual
{
public:
    virtual ~PureVirtual() { std::cout << "Destructor from PureVirtual" << std::endl; }
    virtual void virtualJob() = 0;
    virtual int product(int a, int b) = 0;
};

class Worker : public PureVirtual
{
    ~Worker() { std::cout << "Destructor from Worker" << std::endl; }
    void virtualJob()
    {
        for(int i = 0; i < 10; i++){
            std::cout << "Print: " << i << std::endl;
        }
    }

    int product(int a, int b)
    {
        return a * b;
    }
};

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    PureVirtual *pureV = new Worker;

    std::function<void ()> job = std::bind(&PureVirtual::virtualJob, pureV);
    std::function<int (int, int)> product = std::bind(&PureVirtual::product, pureV, std::placeholders::_1, std::placeholders::_2);

    std::async(std::launch::async, job);
    std::future<int> result = std::async(std::launch::async, product, 10, 50);

    std::cout << "Result = " << result.get() << std::endl;

    delete pureV;

    return 0;
}
