/*
CSV transpose - transpose a CSV file
written by Zink, feel free to use
*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_ROWS 100
#define INITIAL_COLS 100

typedef struct {
	char*** data;  // 3 dimentional matrix ( row, column, string)
	unsigned int rows;
	unsigned int cols;
} CSVMatrix;

void initCSVMatrix(CSVMatrix* matrix, unsigned int rows, unsigned int cols)
{
	matrix->data = (char***)malloc((rows + 1) * sizeof(char**));
	if (matrix->data)
	{
		for (unsigned int i = 0; i < rows; i++)
		{
			matrix->data[i] = (char**)malloc((cols + 1) * sizeof(char*));
			if (matrix->data[i])
			{
				for (unsigned int j = 0; j < cols; j++)
				{
					matrix->data[i][j] = (char*)malloc(256 * sizeof(char));
				}
			}

		}
	}

	matrix->rows = rows;
	matrix->cols = cols;
}

void freeCSVMatrix(CSVMatrix* matrix) 
{
	if (matrix == NULL || matrix -> data == NULL) return;
	for (unsigned int i = 0; i < matrix->rows; i++) 
	{
		if (matrix->data[i])
		{
			for (unsigned int j = 0; j < matrix->cols; j++)
			{
				if (matrix->data[i][j])
				{
					free(matrix->data[i][j]);
					matrix->data[i][j] = NULL;
				}
			}
			free(matrix->data[i]);
			matrix->data[i] = NULL;
		}
	}
	free(matrix->data);
	matrix->data = NULL;
}

CSVMatrix* expandCSVMatrix(CSVMatrix* matrix, unsigned int delta_row, unsigned int delta_col)
{
	if (matrix == NULL || matrix->data == NULL || (delta_row == 0 && delta_col == 0)) return NULL;
	CSVMatrix* tmp = (CSVMatrix*)malloc(sizeof(CSVMatrix));
	initCSVMatrix(tmp, matrix->rows + delta_row, matrix->cols + delta_col);
	if (tmp)
	{
		for (unsigned int ii = matrix->rows, i = 0; i < ii; ++i)
		{
			if (matrix->data[i])
			{
				for (unsigned int jj = matrix->cols, j = 0; j < jj; ++j)
				{
					if (matrix->data[i][j] != NULL || tmp->data[i][j] != NULL)
						strcpy(tmp->data[i][j], matrix->data[i][j]);
				}
			}
		}
		freeCSVMatrix(matrix);
		return tmp;
	}
	else
		return NULL;
}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		printf("Usage: %s <input_file> <output_file>\n", argv[0]);
		return 1;
	}

	// initialize the matrix
	CSVMatrix matrix = { 0 }, * pmatrix = &matrix;
	initCSVMatrix(pmatrix, 1, 1);

	// read input file
	FILE* input_file = fopen(argv[1], "r");
	if (!input_file)
	{
		perror("Error opening input file");
		freeCSVMatrix(pmatrix);
		return 1;
	}

	char line[100000] = { 0 };

	unsigned int row = 0, col = 0;
	while (fgets(line, sizeof(line), input_file)) 
	{
		char* token = strtok(line, ", ");
		col = 0;
		while (token != NULL) 
		{
			// write data into matrix
			if (row >= pmatrix->rows) 
			{
				pmatrix = expandCSVMatrix(pmatrix, 1, 0);
			}
			if (col >= pmatrix->cols)
			{
				pmatrix = expandCSVMatrix(pmatrix, 0, 1);
			}
			if (pmatrix->data[row][col])
			{
				strcpy(pmatrix->data[row][col], token);
				pmatrix->data[row][col][255] = '\0';
			}
			
			col++;
			token = strtok(NULL, ", ");
		}
		row++;
	}
	fclose(input_file);

	// output
	FILE* output_file = fopen(argv[2], "w");
	if (!output_file) 
	{
		perror("Error opening output file");
		freeCSVMatrix(pmatrix);
		return 1;
	}

	for (unsigned int j = 0; j < col; j++) 
	{
		for (unsigned int i = 0; i < row; i++) 
		{
			fprintf(output_file, "%s", pmatrix->data[i][j]);
			if (i < row - 1) fprintf(output_file, ",");
		}
		fprintf(output_file, "\n");
	}
	fclose(output_file);

	// free the memory
	freeCSVMatrix(pmatrix);

	printf("CSV transposed successfully!\n");
	return 0;
}
