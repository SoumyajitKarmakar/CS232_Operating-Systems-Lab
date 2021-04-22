#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#define MAX_PROCESS 10
#define MAX_LEN 10

typedef struct s{
	char name[10];
	int arr;
	int burst;
	int tat;
	int wait;
}Process;

Process table[MAX_PROCESS];
int N = 0;


void ReadProcessTable(char *name){
	FILE *file = fopen(name, "r");
	if(file == NULL)
    	{
        	printf("Error opening file\n");
        	exit(1);
    	}
	int i = 0;
	while( fscanf(file, "%s %d %d\n", table[i].name, &table[i].arr, &table[i].burst) != EOF )
    	{
        	//printf("%s %d %d\n", table[i].name, table[i].arr, table[i].burst);
    		i++;
		N++;
	}
	fclose(file);
	
}

void PrintProcessTable(){
	
	int i;
	for(i = 0; i < N; i++){
		printf("%s %d %d\n", table[i].name, table[i].arr, table[i].burst);
	}
}

void swap(Process *x, Process *y){
	Process t = *x;
	*x = *y;
	*y = t;
}

void PrintStats(){
        int i;
        float t = 0.0f, w = 0.0f;

        printf("\nTurnaround times: ");
        for(i = 0; i < N; i++){
                printf("%s[%d], ", table[i].name, table[i].tat);
                t += table[i].tat;
        }
        printf("\nWait time: ");
        for(i = 0; i < N; i++){
                printf("%s[%d], ", table[i].name, table[i].wait);
                w += table[i].wait;
        }
        printf("\n\nAverage turnaround time: %.2f\nAverage wait time: %.2f\n", t/N, w/N);
}

void FCFS(){
	
	printf("-------------------------------------------------\n");
	printf("      First Come First Served Scheduling\n");
	printf("-------------------------------------------------\n\n");
	
	int i, j, time = 0;
	
	for(i = 0; i < N; i++){
		for(j = 0; j < N - 1; j++){
			if(table[j].arr > table[j + 1].arr)
				swap(&table[j], &table[j + 1]);
		}
	}

	for(i = 0; i < N; i++){
		printf("[%d - %d] %s running\n", time, time + table[i].burst, table[i].name);
		time += table[i].burst;
		table[i].tat = time - table[i].arr;
		table[i].wait = table[i].tat - table[i].burst;
	}
	
	PrintStats();
	
}

void RR(int q){

	printf("-------------------------------------------------\n");
        printf("             Round Robin Scheduling\n");
        printf("-------------------------------------------------\n\n");
	
	int i, j, tt = 0, qu = q;
	int t[N];
	
	for(i = 0; i < N; i++){
		tt += table[i].burst;
	}
	
	for(i = 0; i < N; i++){
                for(j = 0; j < N - 1; j++){
                        if(table[j].arr > table[j + 1].arr)
                                swap(&table[j], &table[j + 1]);
                }
        }

	for(i = 0; i < N; i++){
		t[i] = 0;
	}

	i = 0;
	j = 0;
	int d = 0, time = 0;
	int h = 0;
	
	while(time != tt){
		if(table[j].arr <= time){
			t[j] = table[j].burst;
			j++;
		}
		if(t[i] != 0){
			if(t[i] >= qu){
				printf("[%d - %d] %s running\n", time, time + qu, table[i].name);
				time += qu;
				t[i] -= qu;
				if(t[i] == 0){
					table[i].tat = time - table[i].arr;
                                	table[i].wait = table[i].tat - table[i].burst;
				}
			}
			else{
				printf("[%d - %d] %s running\n", time, time + t[i], table[i].name);
                	        time += t[i];
				t[i] = 0;
				table[i].tat = time - table[i].arr;
				table[i].wait = table[i].tat - table[i].burst;
			}
		}
		
		i = (i + 1) % N;
	}

	PrintStats();
}

void SRBF(){
	printf("-------------------------------------------------\n");
        printf("         Shortest Remaining Burst First\n");
        printf("-------------------------------------------------\n\n");

        int i, j, tt = 0;
        int t[N];

        for(i = 0; i < N; i++){
                tt += table[i].burst;
        }

        for(i = 0; i < N; i++){
                for(j = 0; j < N - 1; j++){
                        if(table[j].arr > table[j + 1].arr)
                                swap(&table[j], &table[j + 1]);
                }
        }

        for(i = 0; i < N; i++){
         	t[i] = INT_MAX;
        }

	int time = 0, otime = 0;
	j = 0;
	i = 0;
	int k = 0, l = 0;
	
	while(time != tt){
		if(j < N && time == table[j].arr){
			t[j] = table[j].burst;
			j++;
		}
		else{
			if(t[i] == 0){
				t[i] = INT_MAX;
			}
			k = i;
			
			for(l = 0; l < N; l++){
				if(t[l] < t[k])
					k = l;
			}
			if(i != k){
				printf("[%d - %d] %s running\n", otime, time, table[i].name);
				otime = time;
				i = k;
			}
			time++;
			t[i]--;
			if(t[i] == 0)
				table[i].tat = time - table[i].arr;
		}
	}
	printf("[%d - %d] %s running\n", otime, time, table[i].name);
	
	for(i = 0; i < N; i++)
		table[i].wait = table[i].tat - table[i].burst;
	
	PrintStats();
}

int main(int argc, char *argv[]){
	
	ReadProcessTable(argv[1]);
	int x = 1, y = 0, q;

	while(x != 0){
		printf("-------------------------------------------------\n");
		printf("          CPU Scheduling Simulation\n");
		printf("-------------------------------------------------\n\n");
		printf("Select the scheduling algorithm [1,2,3 or 4]:\n");
		printf("1. First Come First Served (FCFS)\n");
		printf("2. Round Robin (RR)\n");
		printf("3. Shortest Remaining Burst First (SRBF)\n");
		printf("4. Exit\n\n");

		scanf("%d", &y);

		switch(y){
			case 1:
				FCFS();
				break;
			case 2:
				printf("Enter the time quantum: ");
				scanf("%d", &q);
				printf("\n");
				RR(q);
				break;
			case 3:
				SRBF();
				break;
			case 4:
				return 0;
			default:
				printf("Enter correct number.\n");
		}
		
		printf("\nHit enter to continue ... \n");
		getchar();
		getchar();
	}
	return 0;
}

