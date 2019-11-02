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
#include <time.h>
#include <unistd.h>

// pointer to the function we're using to pick a new frame,
// based on the chosen algorithm
int (*get_frame_func)(struct page_table *pt);

struct disk *disk;

int page_fault_count = 0;
int disk_write_count = 0;
int disk_read_count = 0;

int next_fifo_frame = 0;

// returns a random frame
int rand_func(struct page_table *pt) {
	return rand() % page_table_get_nframes(pt);
}

// returns a frame based on a fifo algorithm
int fifo_func(struct page_table *pt) {
	int next_frame = next_fifo_frame;
	next_fifo_frame = (next_fifo_frame + 1) % page_table_get_nframes(pt);
	return next_frame;
}

// returns a frame based on an LRU algorithm
int custom_func(struct page_table *pt) {
	return 0;
}

// demand paging fault handler retrieves and stores pages on the disk
// as needed by the program
void page_fault_handler( struct page_table *pt, int page )
{
	int new_frame;
	int frame;
	int bits;
	int i;
	char* phys_mem = page_table_get_physmem(pt);

	page_table_get_entry(pt, page, &frame, &bits);

	// if there's a physical memory frame assigned to this page, then the
	// exception was caused by a permissions issue.
	if ( bits != 0 ) {
		// add write to the permissions
		printf("write exception on page: %d\n", page);
		page_table_set_entry(pt, page, frame, (bits | PROT_WRITE) );

	// otherwise, there isn't a frame assigned for this page
	} else {
		// pick a new frame based on algorithm
		new_frame = get_frame_func(pt);

		printf("new frame: %d for page: %d\n", new_frame, page);

		// if frame is occupied and PROT_WRITE is set then write to disk
		for (i = 0; i < page_table_get_npages(pt); i++) {

			page_table_get_entry(pt, i, &frame, &bits);

			// if we find a previous frame assignment and it's dirty, write
			// it to the disk
			if ((frame == new_frame) && (bits & PROT_WRITE)) {

				printf("must store dirty page: %d\n", i);
				disk_write(disk, i, &phys_mem[frame*PAGE_SIZE]);
				disk_write_count++;

				// safe to stop looking if we already found an old assignment
				break;
			}
		}

		// read page from physical memory and store in virtual memory
		disk_read(disk, page, &phys_mem[frame*PAGE_SIZE]);
		disk_read_count++;

		// update page table entry for page
		page_table_set_entry(pt, page, new_frame, PROT_READ);
	}

	page_fault_count++;
}

int main( int argc, char *argv[] )
{
	if(argc!=5) {
		printf("use: virtmem <npages> <nframes> <rand|fifo|custom> <sort|scan|focus>\n");
		return 1;
	}

	int npages = atoi(argv[1]);
	int nframes = atoi(argv[2]);
	const char *method = argv[3];
	const char *program = argv[4];

	disk = disk_open("myvirtualdisk",npages);
	if(!disk) {
		fprintf(stderr,"couldn't create virtual disk: %s\n",strerror(errno));
		return 1;
	}

	// no sense in having more frames than pages
	if (nframes > npages) {
		nframes = npages;
	}


	struct page_table *pt = page_table_create( npages, nframes, page_fault_handler );
	if(!pt) {
		fprintf(stderr,"couldn't create page table: %s\n",strerror(errno));
		return 1;
	}

	char *virtmem = page_table_get_virtmem(pt);

	// figure out algorithm choice
	if(!strcmp(method,"rand")) {
		srand(time(0));
		get_frame_func = &rand_func;
	} else if(!strcmp(method,"fifo")) {
		get_frame_func = &fifo_func;
	} else if(!strcmp(method,"custom")) {
		get_frame_func = &custom_func;
	} else {
		fprintf(stderr,"unknown algorithm: %s\n",argv[3]);
	}

	// figure out program choice
	if(!strcmp(program,"sort")) {
		sort_program(virtmem,npages*PAGE_SIZE);
	} else if(!strcmp(program,"scan")) {
		scan_program(virtmem,npages*PAGE_SIZE);
	} else if(!strcmp(program,"focus")) {
		focus_program(virtmem,npages*PAGE_SIZE);
	} else {
		fprintf(stderr,"unknown program: %s\n",argv[4]);

	}

	page_table_delete(pt);
	disk_close(disk);

	printf("Page faults: %d, disk reads: %d, disk writes: %d\n", page_fault_count,
			disk_read_count, disk_write_count);

	return 0;
}
