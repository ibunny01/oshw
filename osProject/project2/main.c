
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>


#define ARR_SIZE 1000000
static int arr[ARR_SIZE];
static int idx = 0;
static int stat[ARR_SIZE];
int cs1 = 0;
int cs2 = 0;

int init_module(){
	int id1 =1, id2 = 2;
	memset(arr, 0, sizeof(arr));	
	t1 = kthread_create(incrementer, (void*)id1, "inc1");
	t2 = kthread_create(incrementer, (void*)id2, "inc2");
	if (t1&&t2){
		printk(KERN_INFO "Starting..\n");
		wake_up_process(t1);
		wake_up_process(t2);
	} else {
		printk(KERN_EMERG "Error\n");
	}
}

int incrementer(void *ptr){
	printk(KERN_INFO "Consumer TID %d\n", (int)ptr);
	while(idx < ARR_SIZE){
		arr[idx++] += 1;
		schedule();
	}

	if ((int)ptr == 1){
		cs1++;
	} else{
		cs2++;
	}


	return 0;
}

int cleanup_module(){
	for (int i = 0; i< ARR_SIZE; i++){
		stat[arr[i]] ++;
	}

	for (int i = 0; i < ARR_SIZE; i++){
		if (stat[i] > 0)
			printk("stat[%d]: %d\n", i,stat[i]);
	}

	printk("cs1: %d, cs2: %d, cs1+cs2: %d\n", cs1, cs2, cs1+cs2);

	return 0;

}



