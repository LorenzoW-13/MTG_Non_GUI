#include <iostream>

int recursive1(int number = 1);
int recursive2(int number, int counter = 0);

int recursive1(int number) {
    std::cout << "1:\t" << number << std::endl;
    number = recursive2(number);

    if(number > 0)
        number = recursive1(number);

    return number;
}

int recursive2(int number, int counter) {
    std::cout << "\t\t\tCounter is: " << counter << std::endl;
    number--;
    counter++;

    std::cout << "\t2:\t" << number << std::endl;

    if(number > 0 && counter < 4)
        number = recursive2(number, counter);

    return number;
}

int main() {
    int number;
    
    number = recursive1(10);
    std::cout << "Final:\t" << number << std::endl;

    number = recursive1();
    std::cout << "Final:\t" << number << std::endl;

    number = recursive1(7);
    std::cout << "Final:\t" << number << std::endl;

    number = recursive1(2);
    std::cout << "Final:\t" << number << std::endl;

    number = recursive1(4);
    std::cout << "Final:\t" << number << std::endl;


}