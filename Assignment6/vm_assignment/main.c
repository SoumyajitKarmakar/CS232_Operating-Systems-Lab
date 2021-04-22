/*
Main program for the virtual memory project.
Make all of your modifications to this file.
You may add or rearrange any code or data as you need.
The header files page_table.h and disk.h explain
how to use the page table and disk interfaces.
*/

#include "page_table.h"
#include "disk.h"
#include "program.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

//This variable is for accessing the page table.
static int i = 0;
//This variable is to determine the page replacement policy for the program.
static int mode = -1;
//A variable to store the total number of page faults.
static int pageFaults = 0;
//An array to implement the Second Chance Page replacement policy.
static int* arr = 0;

void page_fault_handler(struct page_table* pt, int page)
{

	int j = 0;
	//We get the total number of frames and pages.
	int tframes = page_table_get_nframes(pt);
	int tpages = page_table_get_npages(pt);

	//Temporary variabe to store the frame number and bit.
	int tempf = 0;
	int tempb = 0;

	//Random page replacement policy.
	if (mode == 0) {

		//Get a random integer within 0 to max number of frames.
		int r = lrand48() % tframes;

		//We loop through the page table to determine if there is another page which is mapped to the particular frame number which we generated randomly.
		for (j = 0; j < tpages; j++) {
			page_table_get_entry(pt, j, &tempf, &tempb);

			//If we find one then we remove the mapping.
			if (tempf == r && tempb != 0) {
				page_table_set_entry(pt, j, r, 0);
				break;
			}
		}
		//Here we set it with the current page which is requested.
		page_table_set_entry(pt, page, r, PROT_READ | PROT_WRITE);
		//Increment the number of page faults.
		pageFaults++;
	}

	//FIFO page replacement policy.
	else if (mode == 1) {

		//The variable i basically keeps a track of where was the last page replacement done. We start from the top of the page table, ie. index 0
		//and move one step at a time till the last frame and then in a circular fashion point to the 0th index again, hence acting like a circular array.
		//Here we loop through the entire page table to check if any other page is mapped to the frame pointed by the i variable.
		for (j = 0; j < tpages; j++) {
			page_table_get_entry(pt, j, &tempf, &tempb);

			//If we find any such page we remove it, ie. set permission bit to 0.
			if (tempf == i && tempb != 0) {
				page_table_set_entry(pt, j, i, 0);
				break;
			}
		}
		//We set the new page.
		page_table_set_entry(pt, page, i, PROT_READ | PROT_WRITE);
		//Increment the page fault counter.
		pageFaults++;
		//Increment the index variable in a circular fashion.
		i = (i + 1) % tframes;
	}

	//This is the Second Chance page replacement policy. Here we have an array which contains 0 or 1 depending on if the page was replaced recently or not.
	//If the page to be replaced has this bit = 1 , it means it was replaced recently. So we set it to 0 and move to the next frame in a circular fashion. If this bit is = 0 then we replace it.
	else if (mode == 2) {
		int k = 0;
		//We loop through the page table for a maximum of 2 times the total number of frames.
		for (k = 0; k < tframes * 2; k++) {
			//If the recent access bit is 1, then we make it 0 and move to next.
			if (arr[i] == 1) {
				arr[i] = 0;
				i = (i + 1) % tframes;
			}
			//Otherwise we replace the page as done before.
			else {

				for (j = 0; j < tpages; j++) {
					page_table_get_entry(pt, j, &tempf, &tempb);

					if (tempf == i && tempb != 0) {

						page_table_set_entry(pt, j, i, 0);
						break;
					}
				}

				page_table_set_entry(pt, page, i, PROT_READ | PROT_WRITE);
				pageFaults++;
				//While replacing we assign the recent access bit to be 1, which denotes that this frame was replaced recently.
				arr[i] = 1;
				i = (i + 1) % tframes;
				break;
			}
		}
		
	}

	//If we face some kind oof error during runtime.
	else {
		printf("Encountered an error. Plz try again.");
		exit(1);
	}

	return;
}

int main( int argc, char *argv[] )
{
	if(argc!=5) {
		printf("use: virtmem <npages> <nframes> <rand|fifo|custom> <sort|scan|focus>\n");
		return 1;
	}

	int npages = atoi(argv[1]);
	int nframes = atoi(argv[2]);
	const char *program = argv[4];

	//
	//Array allcation for the implementation of Second Chance Page Replacement policy.
	arr = (int*)calloc(nframes, sizeof(int));

	//Assign the global variable according to the selected mode.
	if (strcmp(argv[3], "rand") == 0)
		mode = 0;
	else if (strcmp(argv[3], "fifo") == 0)
		mode = 1;
	else if (strcmp(argv[3], "custom") == 0)
		mode = 2;
	//

	struct disk *disk = disk_open("myvirtualdisk",npages);
	if(!disk) {
		fprintf(stderr,"couldn't create virtual disk: %s\n",strerror(errno));
		return 1;
	}


	struct page_table *pt = page_table_create( npages, nframes, page_fault_handler );
	if(!pt) {
		fprintf(stderr,"couldn't create page table: %s\n",strerror(errno));
		return 1;
	}

	char *virtmem = page_table_get_virtmem(pt);

	//char *physmem = page_table_get_physmem(pt);

	if(!strcmp(program,"sort")) {
		sort_program(virtmem,npages*PAGE_SIZE);

	} else if(!strcmp(program,"scan")) {
		scan_program(virtmem,npages*PAGE_SIZE);

	} else if(!strcmp(program,"focus")) {
		focus_program(virtmem,npages*PAGE_SIZE);

	} else {
		fprintf(stderr,"unknown program: %s\n",argv[3]);

	}

	//
	//We print the total number of page faults encountered during the execution.
	printf("\nPage Faults : %d\n", pageFaults);
	//

	page_table_delete(pt);
	disk_close(disk);

	return 0;
}
