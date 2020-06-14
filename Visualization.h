#pragma once
#ifndef Visualization_h
#define Visualization_h
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Operator.h"

void plotResults(int N, Vector& Beat_CAAR, Vector& Meet_CAAR, Vector& Miss_CAAR)
{
	int* xData = (int*)malloc((2 * N + 1) * sizeof(int));
	double* yData1 = (double*)malloc((2 * N + 1) * sizeof(double));
	double* yData2 = (double*)malloc((2 * N + 1) * sizeof(double));
	double* yData3 = (double*)malloc((2 * N + 1) * sizeof(double));
	for (int i = 0; i < 2 * N + 1; i++)
	{
		xData[i] = i - N;
		yData1[i] = Beat_CAAR[i];
		yData2[i] = Meet_CAAR[i];
		yData3[i] = Miss_CAAR[i];
	}
	FILE* gnuplotPipe, * tempDataFile;
	const char* tempDataFileName1 = "CAAR_Beat";
	const char* tempDataFileName2 = "CAAR_Meet";
	const char* tempDataFileName3 = "CAAR_Miss";
	double x, y, x2, y2, x3, y3;
	int i;
	int dataSize = 2 * N + 1;
	const char* gnuplotPath = "gnuplot.exe";
	gnuplotPipe = _popen(gnuplotPath, "w");
	if (gnuplotPipe)
	{
		int* Time = new int(dataSize);
		Time[0] = -N;
		for (i = 1; i < dataSize; i++)
		{
			Time[i] = Time[i - 1] + 1;
		}
		fprintf(gnuplotPipe, "plot \"%s\" with lines, \"%s\" with lines,\"%s\" with lines\n", tempDataFileName1, tempDataFileName2, tempDataFileName3);
		fflush(gnuplotPipe);

		tempDataFile = fopen(tempDataFileName1, "w");
		for (i = 0; i < dataSize; i++) {
			x = Time[i];
			y = Beat_CAAR[i];
			fprintf(tempDataFile, "%lf %lf\n", x, y);
		}
		fclose(tempDataFile);

		tempDataFile = fopen(tempDataFileName2, "w");
		for (i = 0; i < dataSize; i++) {
			x2 = Time[i];
			y2 = Meet_CAAR[i];
			fprintf(tempDataFile, "%lf %lf\n", x2, y2);
		}
		fclose(tempDataFile);

		tempDataFile = fopen(tempDataFileName3, "w");
		for (i = 0; i < dataSize; i++) {
			x3 = Time[i];
			y3 = Miss_CAAR[i];
			fprintf(tempDataFile, "%lf %lf\n", x3, y3);
		}
		fclose(tempDataFile);

		printf("press enter to continue...");
		getchar();
		printf("press enter to continue...");
		getchar();
		printf("press enter to continue...");
		getchar();
		fprintf(gnuplotPipe, "exit \n");
	}
	else {
		printf("gnuplot not found...");
	}
}

#endif // !Visualization_h

