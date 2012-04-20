#include <math.h>
#include <iostream>

#define abs(x) (x > 0 ? x : (-x))

using std::cout;
using std::endl;

double f(double x)
{
    return(sin(cos(x)));
}

int main()
{
    cout << "#include <avr/io.h>" << endl
        << "#include <util/delay.h>" << endl
        << "#include <avr/interrupt.h>" << endl;

    unsigned int N = 100, set_min = 0, set_max = 4095;
    double maxx = 2 * M_PI, zoom = maxx / (N * 1.), offset = -maxx / 2;

    double* V = new double[N];
    double min = 0, max = 0, t;
    unsigned int x;

    for(x = 0; x < N; x++)
    {
        t = f(x * zoom + offset);
        if(!x || min > t)
            min = t;
        if(!x || max < t)
            max = t;
        V[x] = t;
    }

    cout << "uint8_t N = " << N << ";" << endl;
    cout << "uint16_t values[] = {";
    
    for(x = 0; x < N; x++)
    {
        V[x]=((V[x] - min) / (max - min) * (set_max - set_min)) + set_min;
        if(x) cout << ", ";
        cout << round(V[x]);
    }
    
    cout << "};" << endl;
    
    return(0);
}
