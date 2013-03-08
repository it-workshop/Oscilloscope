#include <math.h>
#include <iostream>

#define DAC_N 2
#define N 220

#define abs(x) (x > 0 ? x : (-x))

using std::cout;
using std::endl;

double f1(double x)
{
    return(sin(2 * x));
}

double f2(double x)
{
    return(sin(3 * x));
}

double (*f[DAC_N])(double x) = {f1, f2};

int main(int argc, char** argv)
{
	if(argc == 1)
		cout << "Usage: " << argv[0] << " [c/h]" << endl;
	else if(argv[1][0] == 'h')
	{
		cout << "#ifndef FUNCTION_H" << endl;
		cout << "#define FUNCTION_H" << endl;

		cout << "#define DAC_N " << DAC_N << endl;
		cout << "#define N " << N << endl;

		cout << "#endif" << endl;
	}
	else if(argv[1][0] == 'c')
	{
		cout << "#include <avr/io.h>" << endl
			<< "#include <util/delay.h>" << endl
			<< "#include <avr/interrupt.h>" << endl
			<< "#include \"function.h\"" << endl;

		unsigned int set_min = 0, set_max = 4095;
		double maxx = 2 * M_PI, zoom = maxx / (N * 1.), offset = -maxx / 2;

		double V[DAC_N][N];
		double min[DAC_N], max[DAC_N], t;
		unsigned int x, DAC_CN;

		for(DAC_CN = 0; DAC_CN < DAC_N; DAC_CN++)
		{
			min[DAC_CN] = 0;
			max[DAC_CN] = 0;
			for(x = 0; x < N; x++)
			{
				t = f[DAC_CN](x * zoom + offset);
				if(!x || min[DAC_CN] > t)
					min[DAC_CN] = t;
				if(!x || max[DAC_CN] < t)
					max[DAC_CN] = t;
				V[DAC_CN][x] = t;
			}
		}

		cout << "uint16_t values[DAC_N][N] = {" << endl;
		
		for(DAC_CN = 0; DAC_CN < DAC_N; DAC_CN++)
		{
			if(DAC_CN) cout << "," << endl;
			cout << "{";
			for(x = 0; x < N; x++)
			{
				V[DAC_CN][x] = ((V[DAC_CN][x] - min[DAC_CN]) / (max[DAC_CN] - min[DAC_CN]) * (set_max - set_min)) + set_min;
				if(x) cout << ", ";
				cout << round(V[DAC_CN][x]);
			}
			cout << "}";
		}
		
		cout << "};" << endl;
		
		cout << "//list of ports (multiple dac)" << endl;
		cout << "volatile unsigned char* DAC_CMDDDRS[DAC_N] = {&DDRA, &DDRB};" << endl;
		cout << "volatile unsigned char* DAC_CMDPORTS[DAC_N] = {&PORTA, &PORTB};" << endl;
		
		return(0);
	}
}
