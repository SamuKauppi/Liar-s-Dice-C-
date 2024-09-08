#include "Cup.h"
#include "algorithm"

Cup::Cup()
{
    dices.reserve(cup_size);
    dices.resize(cup_size); 
    roll_dice();
}

void Cup::roll_dice()
{
    std::for_each(dices.begin(), dices.end(), [](int& i) {
        i = rand() % 6 + 1; 
        });
}

void Cup::reduce_size()
{
	cup_size--;
    if (cup_size >= 0)
        dices.resize(cup_size);
}

int Cup::how_many_of_x_dice(int x)
{
    int count = 0;
    std::for_each(dices.begin(), dices.end(), [&count, x](const int& i) {
        if (i == x || i == 1)
        {
            ++count;
        }
        });

    return count;
}


